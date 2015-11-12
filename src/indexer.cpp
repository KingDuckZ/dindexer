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

#include "indexer.hpp"
#include "pathname.hpp"
#include "tiger.hpp"
#include "dbbackend.hpp"
#include "settings.hpp"
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#if defined(WITH_PROGRESS_FEEDBACK)
#	include <atomic>
#	include <condition_variable>
#endif
#include <cstdint>
#include <ciso646>
#include <cassert>

#if defined(INDEXER_VERBOSE)
#	include <iostream>
#endif

namespace din {
	typedef TigerHash HashType;

	struct FileEntry {
		FileEntry ( const char* parPath, int parLevel, bool parIsDir, bool parIsSymLink) :
			path(parPath),
			hash {},
			level(static_cast<uint16_t>(parLevel)),
			is_dir(parIsDir),
			is_symlink(parIsSymLink)
		{
		}

		FileEntry ( const FileEntry& ) = delete;
		FileEntry ( FileEntry&& ) = default;
		FileEntry& operator= ( const FileEntry& ) = delete;
		FileEntry& operator= ( FileEntry&& ) = default;
		bool operator< ( const FileEntry& parOther ) const;
		bool operator== ( const FileEntry& ) const = delete;

		std::string path;
		HashType hash;
		uint64_t file_size;
		uint16_t level;
		bool is_dir;
		bool is_symlink;
	};

	namespace {
		void hash_dir (std::vector<FileEntry>::iterator parEntry, std::vector<FileEntry>::iterator parEnd, const PathName& parCurrDir, std::function<void()> parItemDoneCallback) {
			assert(parEntry != parEnd);
			assert(parEntry->is_dir);
			FileEntry& curr_entry = *parEntry;
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
				while (parEnd != it_entry and it_entry->level == curr_entry_it->level + 1 and parCurrDir == PathName(it_entry->path).pop_right()) {
					PathName curr_subdir(it_entry->path);
					if (it_entry->is_dir) {
						hash_dir(it_entry, parEnd, curr_subdir, parItemDoneCallback);

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
				curr_entry.file_size = 0;
#if defined(INDEXER_VERBOSE)
				std::cout << "Got intermediate hash for dir " << parCurrDir << ": " << tiger_to_string(curr_entry.hash) << '\n';
#endif
			}

			//Now with the initial hash ready, let's start hashing files, if any
			{
				auto it_entry = curr_entry_it;
				while (
					it_entry != parEnd
					and (it_entry->is_dir
						or it_entry->level != curr_entry_it->level + 1
						or PathName(it_entry->path).pop_right() != parCurrDir
					)
				) {
					++it_entry;
				}

				while (it_entry != parEnd and not it_entry->is_dir and it_entry->level == curr_entry_it->level + 1 and PathName(it_entry->path).pop_right() == parCurrDir) {
					assert(not it_entry->is_dir);
#if defined(INDEXER_VERBOSE)
					std::cout << "Hashing file " << it_entry->path << "...";
#endif
					tiger_file(it_entry->path, it_entry->hash, curr_entry_it->hash, it_entry->file_size);
					parItemDoneCallback();
#if defined(INDEXER_VERBOSE)
					std::cout << ' ' << tiger_to_string(it_entry->hash) << '\n';
#endif
					++it_entry;
				}
			}

#if defined(INDEXER_VERBOSE)
			std::cout << "Final hash for dir " << parCurrDir << " is " << tiger_to_string(curr_entry_it->hash) << '\n';
#endif
			//parItemDoneCallback();
		}
	} //unnamed namespace

	struct Indexer::LocalData {
		typedef std::vector<FileEntry> PathList;

		DinDBSettings db_settings;
		PathList paths;
#if defined(WITH_PROGRESS_FEEDBACK)
		std::atomic<std::size_t> done_count;
		std::condition_variable step_notify;
#endif
		std::size_t file_count;
	};

	bool FileEntry::operator< (const FileEntry& parOther) const {
		const FileEntry& o = parOther;
		return
			(level < o.level)
			or (level == o.level and is_dir and not o.is_dir)
			or (level == o.level and is_dir == o.is_dir and path < o.path)

			//sort by directory - parent first, children later
			//(level == o.level and is_dir and not o.is_dir)
			//or (level == o.level and is_dir == o.is_dir and path < o.path)
			//or (level > o.level + 1)
			//or (level + 1 == o.level and is_dir and not o.is_dir and path < o.path)
			//or (level + 1 == o.level and is_dir and not o.is_dir and path == PathName(o.path).dirname())
			//or (level == o.level + 1 and not (o.is_dir and not is_dir and o.path == PathName(path).dirname()))
		;
	}

	Indexer::Indexer (const DinDBSettings& parDBSettings) :
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
#endif
		m_local_data->file_count = 0;
		m_local_data->db_settings = parDBSettings;
	}

	Indexer::~Indexer() {
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
		std::sort(m_local_data->paths.begin(), m_local_data->paths.end());
#if defined(INDEXER_VERBOSE)
		for (auto& itm : m_local_data->paths) {
			itm.hash.part_a = 1;
			itm.hash.part_b = 1;
			itm.hash.part_c = 1;

			if (itm.is_dir)
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
			m_local_data->paths.end(),
			base_path,
			[=]() {
				++m_local_data->done_count;
				m_local_data->step_notify.notify_all();
			}
		);

		assert(m_local_data->done_count == m_local_data->paths.size());
#else
		hash_dir(
			m_local_data->paths.begin(),
			m_local_data->paths.end(),
			base_path,
			[]() {}
		);
#endif

#if defined(INDEXER_VERBOSE)
		for (const auto& itm : m_local_data->paths) {
			assert(not (1 == itm.hash.part_a and 1 == itm.hash.part_b and 1 == itm.hash.part_c));
		}
#endif
	}

	void Indexer::add_to_db (const std::string& parSetName, char parType) const {
#if defined(WITH_PROGRESS_FEEDBACK)
		assert(m_local_data->done_count == m_local_data->paths.size());
#endif
		PathName base_path(m_local_data->paths.front().path);
		std::vector<FileRecordData> data;
		data.reserve(m_local_data->paths.size());
		for (const auto& itm : m_local_data->paths) {
			data.push_back(FileRecordData {
				make_relative_path(base_path, PathName(itm.path)).path(),
				tiger_to_string(itm.hash),
				itm.level,
				itm.file_size,
				itm.is_dir,
				itm.is_symlink
			});
		}

		SetRecordData set_data {parSetName, parType};
		write_to_db(m_local_data->db_settings, data, set_data);
	}

	bool Indexer::add_path (const char* parPath, int parLevel, bool parIsDir, bool parIsSymLink) {
		m_local_data->paths.push_back(FileEntry(parPath, parLevel, parIsDir, parIsSymLink));
		if (not parIsDir) {
			++m_local_data->file_count;
		}
		return true;
	}

#if defined(INDEXER_VERBOSE)
	void Indexer::dump() const {
		PathName base_path(m_local_data->paths.front().path);

		std::cout << "---------------- FILE LIST ----------------\n";
		for (const auto& cur_itm : m_local_data->paths) {
			if (not cur_itm.is_dir) {
				PathName cur_path(cur_itm.path);
				std::cout << make_relative_path(base_path, cur_path).path() << '\n';
			}
		}
		std::cout << "---------------- DIRECTORY LIST ----------------\n";
		for (const auto& cur_itm : m_local_data->paths) {
			if (cur_itm.is_dir) {
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
} //namespace din
