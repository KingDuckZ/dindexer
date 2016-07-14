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

#ifndef id63F35BA8B3C94A129291D963ABE66018
#define id63F35BA8B3C94A129291D963ABE66018

#include "dindexer-machinery/recorddata.hpp"
#include "flatinsertin2dlist.hpp"
#include "duckhandy/MaxSizedArray.hpp"
#include "backends/db_backend.hpp"
#include <memory>
#include <cstdint>
#include <vector>
#include <array>
#include <map>
#include <boost/range/algorithm/copy.hpp>
#include <algorithm>
#include <functional>
#include <boost/utility/string_ref.hpp>

namespace pq {
	class Connection;
} //namespace pq

namespace dindb {
	using dhandy::MaxSizedArray;

	enum SetDetails {
		SetDetail_Desc = 0x01,
		SetDetail_Type = 0x02,
		SetDetail_CreationDate = 0x04,
		SetDetail_AppName = 0x08,
		SetDetail_ID = 0x10,
		SetDetail_DiskLabel = 0x20,
		SetDetail_FSUuid = 0x40
	};

	enum FileDetails {
		FileDetail_ID = 0x0001,
		FileDetail_Path = 0x0002,
		FileDetail_Level = 0x0004,
		FileDetail_GroupID = 0x0008,
		FileDetail_IsDir = 0x0010,
		FileDetail_IsSymLink = 0x0020,
		FileDetail_Size = 0x0040,
		FileDetail_Hash = 0x0080,
		FileDetail_IsHashValid = 0x0100,
		FileDetail_ATime = 0x0200,
		FileDetail_MTime = 0x0400,
		FileDetail_Unreadable = 0x800,
		FileDetail_MimeType = 0x1000,
		FileDetail_Charset = 0x2000
	};

	std::vector<GroupIDType> find_all_sets ( pq::Connection& parDB );

	template <SetDetails... D>
	auto find_set_details ( pq::Connection& parDB, const std::vector<GroupIDType>& parIDs ) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>>;

	template <FileDetails... D>
	auto find_file_details ( pq::Connection& parDB, GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir ) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>>;

	std::vector<std::string> find_paths_starting_by ( pq::Connection& parDB, GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath );

	namespace implem {
		typedef std::vector<std::string> ColumnList;
		typedef std::function<void(std::string&&)> QueryCallback;
		typedef std::map<SetDetails, std::string> SetDetailsMap;
		typedef std::map<FileDetails, std::string> FileDetailsMap;

		extern const SetDetailsMap g_set_details_map;
		extern const FileDetailsMap g_file_details_map;

		template <class M, M... Details>
		inline
		std::vector<std::string> make_columns_vec (const std::map<M, std::string>& parDic) {
			//typedef const std::string&(SetDetailsMap::*AtFunc)(const SetDetails&) const;

			std::vector<std::string> columns;
			columns.reserve(sizeof...(Details));

			const std::array<M, sizeof...(Details)> details { {Details...} };
			//AtFunc at_func = &SetDetailsMap::at;
			//std::generate(details.begin(), details.end(), columns.begin(), std::bind(at_func, &details_dic, std::placeholders::_1));
			for (auto detail : details) {
				columns.push_back(parDic.at(detail));
			}
			return columns;
		}

		void query_no_conditions ( pq::Connection& parDB, const ColumnList& parCols, boost::string_ref parTable, const std::vector<GroupIDType>& parIDs, QueryCallback parCallback );
		void query_files_in_dir ( pq::Connection& parDB, const ColumnList& parCols, boost::string_ref parDir, uint16_t parLevel, GroupIDType parGroupID, QueryCallback parCallback );
	} //namespace implem

	template <SetDetails... D>
	inline
	auto find_set_details (pq::Connection& parDB, const std::vector<GroupIDType>& parIDs) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>> {
		using dhandy::FlatInsertIn2DList;
		typedef std::vector<MaxSizedArray<std::string, sizeof...(D)>> ReturnType;
		typedef void(FlatInsertIn2DList<ReturnType>::*FlatPushBackFunc)(std::string&&);

		const auto columns = implem::make_columns_vec<SetDetails, D...>(implem::g_set_details_map);

		ReturnType list;
		FlatInsertIn2DList<ReturnType> flat_list(&list, sizeof...(D));
		FlatPushBackFunc pback_func = &FlatInsertIn2DList<ReturnType>::push_back;
		implem::query_no_conditions(parDB, columns, "sets", parIDs, std::bind(pback_func, &flat_list, std::placeholders::_1));
		return list;
	}

	template <FileDetails... D>
	inline
	auto find_file_details (pq::Connection& parDB, GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>> {
		using dhandy::FlatInsertIn2DList;
		typedef std::vector<MaxSizedArray<std::string, sizeof...(D)>> ReturnType;
		typedef void(FlatInsertIn2DList<ReturnType>::*FlatPushBackFunc)(std::string&&);

		const auto columns = implem::make_columns_vec<FileDetails, D...>(implem::g_file_details_map);

		ReturnType list;
		FlatInsertIn2DList<ReturnType> flat_list(&list, sizeof...(D));
		FlatPushBackFunc pback_func = &FlatInsertIn2DList<ReturnType>::push_back;
		implem::query_files_in_dir(parDB, columns, parDir, parLevel, parSetID, std::bind(pback_func, &flat_list, std::placeholders::_1));
		return list;
	}
} //namespace dindb

#endif
