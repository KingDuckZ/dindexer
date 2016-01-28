/* Copyright 2016, Michele Santullo
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
#include "MaxSizedArray.hpp"
#include <memory>
#include <cstdint>
#include <vector>
#include <array>
#include <map>
#include <boost/range/algorithm/copy.hpp>
#include <algorithm>
#include <functional>
#include <boost/utility/string_ref.hpp>

namespace dinlib {
	struct SettingsDB;
} //namespace dinlib

namespace pq {
	class Connection;
} //namespace pq

namespace din {
	enum SetDetails {
		SetDetail_Desc = 0x01,
		SetDetail_Type = 0x02,
		SetDetail_CreeationDate = 0x04,
		SetDetail_AppName = 0x08,
		SetDetail_ID = 0x10
	};

	class DBSource {
	public:
		explicit DBSource ( const dinlib::SettingsDB& parDBSettings );
		~DBSource ( void ) noexcept;

		void disconnect ( void );
		std::vector<uint32_t> sets ( void );

		template <SetDetails... D>
		auto set_details ( const std::vector<uint32_t>& parIDs ) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>>;
		//TODO: replace return value with vector of maxsizedarray
		//auto set_details ( const std::vector<uint32_t>& parIDs ) -> std::array<std::string, sizeof...(D)>;

	private:
		struct LocalData;

		pq::Connection& get_conn ( void );
		void query_push_results ( const std::vector<std::string>& parCols, boost::string_ref parTable, const std::vector<uint32_t>& parIDs, std::function<void(std::string&&)> parCallback );

		std::unique_ptr<LocalData> m_local_data;
	};

	template <SetDetails... D>
	auto DBSource::set_details (const std::vector<uint32_t>& parIDs) -> std::vector<MaxSizedArray<std::string, sizeof...(D)>> {
	//auto DBSource::set_details (const std::vector<uint32_t>& parIDs) -> std::array<std::string, sizeof...(D)> {
		typedef std::vector<MaxSizedArray<std::string, sizeof...(D)>> ReturnType;
		typedef std::map<SetDetails, std::string> DetailsMap;
		typedef const std::string&(DetailsMap::*AtFunc)(const SetDetails&) const;
		typedef void(din::FlatInsertIn2DList<ReturnType>::*FlatPushBackFunc)(std::string&&);

		const std::array<SetDetails, sizeof...(D)> details { D... };
		const DetailsMap details_dic {
			{SetDetail_Desc, "desc"},
			{SetDetail_Type, "type"},
			{SetDetail_CreeationDate, "creation"},
			{SetDetail_AppName, "app_name"},
			{SetDetail_ID, "id"}
		};

		std::vector<std::string> columns;
		columns.reserve(sizeof...(D));
		AtFunc at_func = &DetailsMap::at;
		//std::generate(details.begin(), details.end(), columns.begin(), std::bind(at_func, &details_dic, std::placeholders::_1));
		for (auto detail : details) {
			columns.push_back(details_dic.at(detail));
		}

		ReturnType list;
		FlatInsertIn2DList<ReturnType> flat_list(&list, sizeof...(D));
		FlatPushBackFunc pback_func = &FlatInsertIn2DList<ReturnType>::push_back;
		this->query_push_results(columns, "sets", parIDs, std::bind(pback_func, &flat_list, std::placeholders::_1));
		return std::move(list);
	}
} //namespace din

#endif
