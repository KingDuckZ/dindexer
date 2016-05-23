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
#include "helpers/flatinsertin2dlist.hpp"
#include "helpers/MaxSizedArray.hpp"
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

namespace dinbpostgres {
	using dinhelp::MaxSizedArray;

	struct Settings;

	enum SetDetails {
		SetDetail_Desc = 0x01,
		SetDetail_Type = 0x02,
		SetDetail_CreeationDate = 0x04,
		SetDetail_AppName = 0x08,
		SetDetail_ID = 0x10
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

	class DBSource {
	public:
		explicit DBSource ( const Settings& parDBSettings );
		~DBSource ( void ) noexcept;

		void disconnect ( void );
		std::vector<uint32_t> sets ( void );

		template <SetDetails... D>
		auto set_details ( const std::vector<uint32_t>& parIDs ) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>>;

		template <FileDetails... D>
		auto file_details ( uint32_t parSetID, uint16_t parLevel, boost::string_ref parDir ) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>>;

		std::vector<std::string> paths_starting_by ( uint32_t parGroupID, uint16_t parLevel, boost::string_ref parPath );

	private:
		struct LocalData;
		typedef std::map<SetDetails, std::string> SetDetailsMap;
		typedef std::map<FileDetails, std::string> FileDetailsMap;
		typedef std::vector<std::string> ColumnList;
		typedef std::function<void(std::string&&)> QueryCallback;

		pq::Connection& get_conn ( void );
		void query_no_conditions ( const ColumnList& parCols, boost::string_ref parTable, const std::vector<uint32_t>& parIDs, QueryCallback parCallback );
		void query_files_in_dir ( const ColumnList& parCols, boost::string_ref parDir, uint16_t parLevel, uint32_t parGroupID, QueryCallback parCallback );

		static const SetDetailsMap m_set_details_map;
		static const FileDetailsMap m_file_details_map;

		std::unique_ptr<LocalData> m_local_data;
	};

	namespace implem {
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
	} //namespace implem

	template <SetDetails... D>
	auto DBSource::set_details (const std::vector<uint32_t>& parIDs) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>> {
		using dinhelp::FlatInsertIn2DList;
		typedef std::vector<MaxSizedArray<std::string, sizeof...(D)>> ReturnType;
		typedef void(FlatInsertIn2DList<ReturnType>::*FlatPushBackFunc)(std::string&&);

		const auto columns = implem::make_columns_vec<SetDetails, D...>(m_set_details_map);

		ReturnType list;
		FlatInsertIn2DList<ReturnType> flat_list(&list, sizeof...(D));
		FlatPushBackFunc pback_func = &FlatInsertIn2DList<ReturnType>::push_back;
		this->query_no_conditions(columns, "sets", parIDs, std::bind(pback_func, &flat_list, std::placeholders::_1));
		return list;
	}

	template <FileDetails... D>
	auto DBSource::file_details (uint32_t parSetID, uint16_t parLevel, boost::string_ref parDir) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>> {
		using dinhelp::FlatInsertIn2DList;
		typedef std::vector<MaxSizedArray<std::string, sizeof...(D)>> ReturnType;
		typedef void(FlatInsertIn2DList<ReturnType>::*FlatPushBackFunc)(std::string&&);

		const auto columns = implem::make_columns_vec<FileDetails, D...>(m_file_details_map);

		ReturnType list;
		FlatInsertIn2DList<ReturnType> flat_list(&list, sizeof...(D));
		FlatPushBackFunc pback_func = &FlatInsertIn2DList<ReturnType>::push_back;
		this->query_files_in_dir(columns, parDir, parLevel, parSetID, std::bind(pback_func, &flat_list, std::placeholders::_1));
		return list;
	}
} //namespace dinbpostgres

#endif
