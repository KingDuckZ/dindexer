/* Copyright 2015, 2016, Michele Santullo
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

#include "dindexer-machinery/scantask/hashing.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "dindexer-machinery/set_listing.hpp"
#include "dindexer-machinery/tiger.hpp"
#include "pathname.hpp"
#include <cassert>
#include <boost/range/empty.hpp>
#include <boost/utility/string_ref.hpp>

//#define INDEXER_VERBOSE

#if defined(INDEXER_VERBOSE)
#	include <iostream>
#endif

namespace mchlib {
	namespace {
		struct ProgressInfo {
			scantask::Hashing::ProgressCallback callback;
			boost::string_ref curr_path;
			uint64_t file_bytes_read;
			uint64_t total_bytes_read;
			uint32_t file_num;

			void notify ( void ) {
				callback(curr_path, file_bytes_read, total_bytes_read, file_num);
			}
		};

		void append_to_vec (std::vector<char>& parDest, const TigerHash& parHash, boost::string_ref parString) {
			const auto old_size = parDest.size();
			parDest.resize(old_size + sizeof(TigerHash) + parString.size());
			std::copy(parHash.byte_data, parHash.byte_data + sizeof(TigerHash), parDest.begin() + old_size);
			std::copy(parString.begin(), parString.end(), parDest.begin() + old_size + sizeof(TigerHash));
		}

		void append_to_vec (std::vector<char>& parDest, boost::string_ref parString) {
			const auto old_size = parDest.size();
			parDest.resize(old_size + parString.size());
			std::copy(parString.begin(), parString.end(), parDest.begin() + old_size);
		}

		void hash_dir (FileRecordData& parEntry, MutableSetListingView& parList, bool parIgnoreErrors, ProgressInfo& parProgressInfo) {
			assert(parEntry.is_directory);

			//Build a blob with the hashes and filenames of every directory that
			//is a direct child of current entry
			std::vector<char> dir_blob;
#if defined(INDEXER_VERBOSE)
			std::cout << "Making initial hash for " << parEntry.abs_path << "...\n";
#endif
			for (auto it = parList.begin(); it != parList.end(); ++it) {
				assert(PathName(parEntry.abs_path) == PathName(it->abs_path).pop_right());

				PathName curr_path(it->path());
				const auto basename = mchlib::basename(curr_path);
				if (it->is_directory) {
					auto cd_list = MutableSetListingView(it);
					assert(boost::empty(cd_list) or cd_list.begin()->abs_path != it->abs_path);

					hash_dir(*it, cd_list, parIgnoreErrors, parProgressInfo);
					append_to_vec(dir_blob, it->hash, basename);
				}
				else {
					append_to_vec(dir_blob, basename);
				}
			}
			tiger_data(dir_blob, parEntry.hash);

#if defined(INDEXER_VERBOSE)
			std::cout << "Got intermediate hash for dir " << parEntry.abs_path <<
				": " << tiger_to_string(parEntry.hash) <<
				' ' << parEntry.mime_type << '\n';
#endif

			//Now with the initial hash ready, let's start hashing files, if any
			for (auto it = first_file(parList); it != parList.end(); ++it) {
				assert(not it->is_directory);
#if defined(INDEXER_VERBOSE)
				std::cout << "Hashing file " << it->abs_path << "...\n";
#endif
				//TODO: notify callback
				try {
					++parProgressInfo.file_num;
					parProgressInfo.curr_path = it->abs_path;
					parProgressInfo.notify();

					tiger_file(it->abs_path, it->hash, parEntry.hash, it->size);
					it->hash_valid = true;
					parProgressInfo.total_bytes_read += it->size;
				}
				catch (const std::ios_base::failure& e) {
					if (parIgnoreErrors) {
						it->unreadable = true;
						it->hash = TigerHash {};
					}
					else {
						throw e;
					}
				}
			}

#if defined(INDEXER_VERBOSE)
			std::cout << "Final hash for dir " << parEntry.abs_path << " is " << tiger_to_string(parEntry.hash) << '\n';
#endif
			parEntry.hash_valid = true;
		}

		void dummy_progress_callback (const boost::string_ref /*parPath*/, uint64_t /*parFileBytes*/, uint64_t /*parTotalBytes*/, uint32_t /*parFileNum*/) {
		}
	} //unnamed namespace

	namespace scantask {
		Hashing::Hashing (std::shared_ptr<FileTreeBase> parFileTree, bool parIgnoreErrors) :
			m_file_tree_task(parFileTree),
			m_progress_callback(&dummy_progress_callback),
			m_ignore_errors(parIgnoreErrors)
		{
			assert(m_file_tree_task);
		}

		Hashing::~Hashing() noexcept {
		}

		std::vector<FileRecordData>& Hashing::on_data_get() {
			return m_file_tree_task->get_or_create();
		}

		void Hashing::on_data_fill() {
			std::vector<FileRecordData>& file_list = m_file_tree_task->get_or_create();
			if (file_list.empty()) {
				return;
			}

			ProgressInfo progr_info;
			progr_info.callback = m_progress_callback;
			progr_info.curr_path = "";
			progr_info.file_bytes_read = 0;
			progr_info.total_bytes_read = 0;
			progr_info.file_num = 0;

			if (file_list.front().is_directory) {
				MutableSetListingView recordlist(file_list.begin(), file_list.end(), 0);
				hash_dir(file_list.front(), recordlist, m_ignore_errors, progr_info);
			}
			else {
				assert(1 == file_list.size());
				auto& curr_file_rec = file_list.front();
				TigerHash dummy {};

				try {
					tiger_file(curr_file_rec.abs_path, curr_file_rec.hash, dummy, curr_file_rec.size);
					curr_file_rec.hash_valid = true;
				}
				catch (const std::ios_base::failure& e) {
					if (m_ignore_errors) {
						curr_file_rec.unreadable = true;
						curr_file_rec.hash = TigerHash {};
					}
					else {
						throw e;
					}
				}
			}
		}

		void Hashing::set_progress_callback (ProgressCallback parFunc) {
			if (parFunc) {
				m_progress_callback = parFunc;
			}
			else {
				m_progress_callback = &dummy_progress_callback;
			}
		}
	} //namespace scantask
} //namespace mchlib
