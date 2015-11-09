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
#include <algorithm>
#include <vector>
#include <string>
#include <atomic>
#include <cstdint>
#include <ciso646>

#if !defined(NDEBUG)
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

		std::string path;
		HashType hash;
		uint16_t level;
		bool is_dir;
		bool is_symlink;
	};

	struct Indexer::LocalData {
		typedef std::vector<FileEntry> PathList;

		PathList paths;
		std::string base_path;
		std::atomic<std::size_t> done_count;
		std::size_t file_count;
	};

	bool FileEntry::operator< (const FileEntry& parOther) const {
		return (this->level < parOther.level)
			or (this->level == parOther.level and this->path < parOther.path);
	}

	Indexer::Indexer() :
		m_local_data(new LocalData)
	{
		m_local_data->done_count = 0;
		m_local_data->file_count = 0;
	}

	Indexer::~Indexer() {
	}

	std::size_t Indexer::total_items() const {
		return m_local_data->file_count;
	}

	std::size_t Indexer::processed_items() const {
		return m_local_data->done_count;
	}

	void Indexer::calculate_hash() {
		std::sort(m_local_data->paths.begin(), m_local_data->paths.end());

		HashType dir_hash;
		tiger_init_hash(dir_hash);
		for (auto& cur_itm : m_local_data->paths) {
			if (not cur_itm.is_dir) {
				std::cout << "Hashing " << cur_itm.path << "...";
				tiger_init_hash(cur_itm.hash);
				tiger_file(cur_itm.path, cur_itm.hash, dir_hash);
				std::cout << " --> " << tiger_to_string(cur_itm.hash) << '\n';
			}
		}
	}

	bool Indexer::add_path (const char* parPath, int parLevel, bool parIsDir, bool parIsSymLink) {
		if (parLevel > 0) {
			m_local_data->paths.push_back(FileEntry(parPath, parLevel, parIsDir, parIsSymLink));
			if (not parIsDir) {
				++m_local_data->file_count;
			}
		} else {
			m_local_data->base_path = parPath;
		}
		return true;
	}

#if !defined(NDEBUG)
	void Indexer::dump() const {
		PathName base_path(m_local_data->base_path);

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
} //namespace din
