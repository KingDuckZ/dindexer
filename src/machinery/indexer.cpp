/* Copyright 2015, Michele Santullo
 * This file is part of "dindexer".
 *
 * "dindexer" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "dindexer" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "dindexer".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dindexer-machinery/indexer.hpp"
#include "pathname.hpp"
#include "dindexer-machinery/tiger.hpp"
#include "dindexer-common/settings.hpp"
#include "dindexer-machinery/filestats.hpp"
#include "mimetype.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>
#if defined(WITH_PROGRESS_FEEDBACK)
#	include <atomic>
#	include <condition_variable>
#endif
#include <cstdint>
#include <ciso646>
#include <cassert>
#include <boost/iterator/filter_iterator.hpp>
#include <sstream>
#include <iterator>

#if defined(INDEXER_VERBOSE)
#	include <iostream>
#endif
#include <boost/utility/string_ref.hpp>

namespace mchlib {
	using HashType = decltype(FileRecordData::hash);

	namespace {
		typedef std::vector<FileRecordData>::iterator FileEntryIt;

		void hash_dir (FileEntryIt parEntry, FileEntryIt parBegin, FileEntryIt parEnd, const PathName& parCurrDir, std::function<void(std::size_t)> parNextItemCallback, bool parIgnoreErrors, MimeType& parMime) {
			assert(parEntry != parEnd);
			assert(parEntry->is_directory);
			FileRecordData& curr_entry = *parEntry;
			auto& curr_entry_it = parEntry;

			//Build a blob with the hashes and filenames of every directory that
			//is a direct child of current entry
			{
				std::vector<char> dir_blob;
				auto it_entry = curr_entry_it;

				while (
					it_entry != parEnd and (
						it_entry->level == curr_entry.level
						or parCurrDir != PathName(it_entry->path).pop_right()
					//and (not it_entry->is_dir or (it_entry->level <= curr_entry.level
					//and parCurrDir != PathName(it_entry->path).pop_right()))
				)) {
					assert(it_entry->level >= curr_entry.level);
					++it_entry;
				}

#if defined(INDEXER_VERBOSE)
				std::cout << "Making initial hash for " << parCurrDir << "...\n";
#endif
				curr_entry.mime_full = parMime.analyze(it_entry->path);
				while (parEnd != it_entry and it_entry->level == curr_entry_it->level + 1 and parCurrDir == PathName(it_entry->path).pop_right()) {
					PathName curr_subdir(it_entry->path);
					if (it_entry->is_directory) {
						hash_dir(it_entry, parBegin, parEnd, curr_subdir, parNextItemCallback, parIgnoreErrors, parMime);

						std::string relpath = make_relative_path(parCurrDir, curr_subdir).path();
						const auto old_size = dir_blob.size();
						dir_blob.resize(old_size + sizeof(HashType) + relpath.size());
						std::copy(it_entry->hash.byte_data, it_entry->hash.byte_data + sizeof(HashType), dir_blob.begin() + old_size);
						std::copy(relpath.begin(), relpath.end(), dir_blob.begin() + old_size + sizeof(HashType));
					}
					else {
						std::string relpath = make_relative_path(parCurrDir, curr_subdir).path();
						const auto old_size = dir_blob.size();
						dir_blob.resize(old_size + relpath.size());
						std::copy(relpath.begin(), relpath.end(), dir_blob.begin() + old_size);
					}
					++it_entry;
				}

				tiger_data(dir_blob, curr_entry.hash);
				curr_entry.size = 0;
#if defined(INDEXER_VERBOSE)
				std::cout << "Got intermediate hash for dir " << parCurrDir <<
					": " << tiger_to_string(curr_entry.hash) <<
					' ' << curr_entry.mime_type << '\n';
#endif
			}

			//Now with the initial hash ready, let's start hashing files, if any
			{
				auto it_entry = curr_entry_it;
				while (
					it_entry != parEnd
					and (it_entry->is_directory
						or it_entry->level != curr_entry_it->level + 1
						or PathName(it_entry->path).pop_right() != parCurrDir
					)
				) {
					++it_entry;
				}

				while (it_entry != parEnd and not it_entry->is_directory and it_entry->level == curr_entry_it->level + 1 and PathName(it_entry->path).pop_right() == parCurrDir) {
					assert(not it_entry->is_directory);
#if defined(INDEXER_VERBOSE)
					std::cout << "Hashing file " << it_entry->path << "...";
#endif
					parNextItemCallback(it_entry - parBegin);
					try {
						tiger_file(it_entry->path, it_entry->hash, curr_entry_it->hash, it_entry->size);
						it_entry->hash_valid = true;
						it_entry->mime_full = parMime.analyze(it_entry->path);
						auto mime_pair = split_mime(it_entry->mime_full);
						it_entry->mime_type = mime_pair.first;
						it_entry->mime_charset = mime_pair.second;
					}
					catch (const std::ios_base::failure& e) {
						if (parIgnoreErrors) {
							it_entry->unreadable = true;
							it_entry->hash = HashType {};
							if (it_entry->mime_full.get().empty()) {
								it_entry->mime_full = "unknown";
								it_entry->mime_type = boost::string_ref(it_entry->mime_full.get());
								it_entry->mime_charset = boost::string_ref(it_entry->mime_full.get());
							}
						}
						else {
							throw e;
						}
					}

#if defined(INDEXER_VERBOSE)
					std::cout << ' ' << tiger_to_string(it_entry->hash) << ' ' <<
						"Mime type: \"" << it_entry->mime_type << "\"\n";
#endif
					++it_entry;
				}
			}

#if defined(INDEXER_VERBOSE)
			std::cout << "Final hash for dir " << parCurrDir << " is " << tiger_to_string(curr_entry_it->hash) << '\n';
#endif
			curr_entry_it->hash_valid = true;
			{
				curr_entry_it->mime_full = parMime.analyze(curr_entry_it->path);
				auto mime_pair = split_mime(curr_entry_it->mime_full);
				curr_entry_it->mime_type = mime_pair.first;
				curr_entry_it->mime_charset = mime_pair.second;
			}
		}

		template <bool FileTrue=true>
		struct IsFile {
			bool operator() ( const FileRecordData& parEntry ) const { return parEntry.is_directory xor FileTrue; }
		};

		FileRecordData make_file_record_data (const char* parPath, const fastf::FileStats& parSt) {
			return FileRecordData(
				parPath,
				parSt.atime,
				parSt.mtime,
				parSt.level,
				parSt.is_dir,
				parSt.is_symlink
			);
		}

		bool file_record_data_lt (const FileRecordData& parLeft, const FileRecordData& parRight) {
			const FileRecordData& l = parLeft;
			const FileRecordData& r = parRight;
			return
				(l.level < r.level)
				or (l.level == r.level and l.is_directory and not r.is_directory)
				or (l.level == r.level and l.is_directory == r.is_directory and l.path < r.path)

				//sort by directory - parent first, children later
				//(level == o.level and is_dir and not o.is_dir)
				//or (level == o.level and is_dir == o.is_dir and path < o.path)
				//or (level > o.level + 1)
				//or (level + 1 == o.level and is_dir and not o.is_dir and path < o.path)
				//or (level + 1 == o.level and is_dir and not o.is_dir and path == PathName(o.path).dirname())
				//or (level == o.level + 1 and not (o.is_dir and not is_dir and o.path == PathName(path).dirname()))
			;
		}
	} //unnamed namespace

	struct Indexer::LocalData {
		typedef std::vector<FileRecordData> PathList;

		PathList paths;
#if defined(WITH_PROGRESS_FEEDBACK)
		std::atomic<std::size_t> done_count;
		std::atomic<std::size_t> processing_index;
		std::condition_variable step_notify;
#endif
		std::size_t file_count;
		bool ignore_read_errors;
	};

	Indexer::Indexer() :
		m_local_data(new LocalData)
	{
#if !defined(NDEBUG)
		//assert(FileEntry("/a/b/c", 3, true, false) < FileEntry("/a/b", 2, true, false));
		//assert(FileEntry("/a/b/c", 3, true, false) < FileEntry("/a/b/c/file.txt", 4, false, false));
		//assert(FileEntry("/a/b/c", 3, true, false) < FileEntry("/a/b/c/file.c", 4, false, false));
		//assert(FileEntry("/a/b/c/d", 4, true, false) < FileEntry("/a/b", 2, true, false));
		//assert(FileEntry("/a/b/c/d", 4, true, false) < FileEntry("/a/b/c", 3, true, false));
		//assert(FileEntry("/a/b/c/1.txt", 4, true, false) < FileEntry("/a/b/c/2.txt", 4, true, false));
		//assert(not (FileEntry("/a/b/file.txt", 3, false, false) < FileEntry("/a/b", 2, true, false)));
		//assert(not (FileEntry("/a", 1, true, false) < FileEntry("/a/b", 2, true, false)));
		//assert(not (FileEntry("/a/b/1.txt", 3, false, false) < FileEntry("/a/b/c/f.txt", 4, true, false)));
		//assert(not (FileEntry("/a/b/c/file.c", 4, false, false) < FileEntry("/a/b/c", 3, true, false)));
#endif
#if defined(WITH_PROGRESS_FEEDBACK)
		m_local_data->done_count = 0;
		m_local_data->processing_index = 0;
#endif
		m_local_data->file_count = 0;
	}

	Indexer::~Indexer() noexcept {
	}

	std::size_t Indexer::total_items() const {
		return m_local_data->file_count;
	}

#if defined(WITH_PROGRESS_FEEDBACK)
	std::size_t Indexer::processed_items() const {
		return m_local_data->done_count;
	}
#endif

	void Indexer::calculate_hash() {
		PathName base_path(m_local_data->paths.front().path);
		std::sort(m_local_data->paths.begin(), m_local_data->paths.end(), &file_record_data_lt);
		MimeType mime;

#if defined(INDEXER_VERBOSE)
		for (auto& itm : m_local_data->paths) {
			itm.hash.part_a = 1;
			itm.hash.part_b = 1;
			itm.hash.part_c = 1;

			if (itm.is_directory)
				std::cout << "(D) ";
			else
				std::cout << "(F) ";
			std::cout << itm.path << " (" << itm.level << ")\n";
		}
		std::cout << "-----------------------------------------------------\n";
#endif

#if defined(WITH_PROGRESS_FEEDBACK)
		m_local_data->done_count = 0;
		hash_dir(
			m_local_data->paths.begin(),
			m_local_data->paths.begin(),
			m_local_data->paths.end(),
			base_path,
			[=](std::size_t parNext) {
				++m_local_data->done_count;
				m_local_data->processing_index = parNext;
				m_local_data->step_notify.notify_all();
			},
			m_local_data->ignore_read_errors,
			mime
		);

		assert(m_local_data->done_count == m_local_data->file_count);
#else
		hash_dir(
			m_local_data->paths.begin(),
			m_local_data->paths.begin(),
			m_local_data->paths.end(),
			base_path,
			[](std::size_t) {},
			m_local_data->ignore_read_errors,
			mime
		);
#endif

#if defined(INDEXER_VERBOSE)
		for (const auto& itm : m_local_data->paths) {
			assert(not (1 == itm.hash.part_a and 1 == itm.hash.part_b and 1 == itm.hash.part_c));
		}
#endif
	}

	bool Indexer::add_path (const char* parPath, const fastf::FileStats& parStats) {
		m_local_data->paths.push_back(
			make_file_record_data(parPath, parStats));
		if (not parStats.is_dir) {
			++m_local_data->file_count;
		}
		return true;
	}

#if defined(INDEXER_VERBOSE)
	void Indexer::dump() const {
		PathName base_path(m_local_data->paths.front().path);

		std::cout << "---------------- FILE LIST ----------------\n";
		for (const auto& cur_itm : m_local_data->paths) {
			if (not cur_itm.is_directory) {
				PathName cur_path(cur_itm.path);
				std::cout << make_relative_path(base_path, cur_path).path() << '\n';
			}
		}
		std::cout << "---------------- DIRECTORY LIST ----------------\n";
		for (const auto& cur_itm : m_local_data->paths) {
			if (cur_itm.is_directory) {
				PathName cur_path(cur_itm.path);
				std::cout << make_relative_path(base_path, cur_path).path() << '\n';
			}
		}
	}
#endif

	bool Indexer::empty() const {
		return m_local_data->paths.size() < 2;
	}

#if defined(WITH_PROGRESS_FEEDBACK)
	std::condition_variable& Indexer::step_notify() {
		return m_local_data->step_notify;
	}
#endif

#if defined(WITH_PROGRESS_FEEDBACK)
	std::string Indexer::current_item() const {
		if (m_local_data->paths.empty() or 0 == m_local_data->processing_index)
			return std::string();

		PathName base_path(m_local_data->paths.front().path);
		PathName ret_path(m_local_data->paths[m_local_data->processing_index].path);
		return make_relative_path(base_path, ret_path).path();
	}
#endif

	std::string Indexer::operator[] (std::size_t parIndex) const {
		if (parIndex >= m_local_data->file_count) {
			std::ostringstream oss;
			oss << "Requested index " << parIndex << " is out of range: only " << m_local_data->file_count << " items are available";
			throw std::out_of_range(oss.str());
		}

		auto it = boost::make_filter_iterator<IsFile<>>(m_local_data->paths.begin(), m_local_data->paths.end());
		assert(not m_local_data->paths.empty());
		std::advance(it, parIndex);
		return make_relative_path(PathName(m_local_data->paths.front().path), PathName(it->path)).path();
	}

	void Indexer::ignore_read_errors (bool parIgnore) {
		m_local_data->ignore_read_errors = parIgnore;
	}

	const std::vector<FileRecordData>& Indexer::record_data() const {
#if defined(WITH_PROGRESS_FEEDBACK)
		assert(m_local_data->done_count == m_local_data->file_count);
#endif
		return m_local_data->paths;
	}
} //namespace mchlib