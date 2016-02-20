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

#ifndef idAE026EED2DF54B118943156FB36AA46E
#define idAE026EED2DF54B118943156FB36AA46E

#include <boost/iterator/filter_iterator.hpp>
#include <functional>

namespace mchlib {
	struct FileRecordData;

	template <typename I>
	using globbing_iterator =
		boost::filter_iterator<
			std::function<bool(const FileRecordData&)>,
			I
		>;

	template <typename I>
	globbing_iterator<I> make_globbing_iterator ( I parBegin, I parEnd, const char* parGlob );

	namespace implem {
		bool glob_matches ( const FileRecordData& parData, const char* parGlob );
	} //namespace implem

	template <typename I>
	inline
	globbing_iterator<I> make_globbing_iterator (I parBegin, I parEnd, const char* parGlob) {
		return boost::make_filter_iterator<std::function<bool(const FileRecordData&)>>(
			std::bind(&implem::glob_matches, std::placeholders::_1, parGlob),
			parBegin,
			parEnd
		);
	}

	template <typename I>
	class Glob {
	public:
		using const_iterator = globbing_iterator<I>;

		explicit Glob ( I parBegin, I parEnd ) :
			m_beg(parBegin),
			m_end(parEnd),
			m_pattern("*")
		{
		}

		Glob ( const char* parPattern, I parBegin, I parEnd ) :
			m_beg(parBegin),
			m_end(parEnd),
			m_pattern(parPattern)
		{
		}

		~Glob ( void ) = default;

		const_iterator begin ( void ) const {
			return make_globbing_iterator(m_beg, m_end, m_pattern);
		}

		const_iterator end ( void ) const {
			return make_globbing_iterator(m_end, m_end, m_pattern);
		}

		void set_pattern ( const char* parPattern ) {
			if (parPattern)
				m_pattern = parPattern;
			else
				m_pattern = "*";
		}

	private:
		I m_beg;
		I m_end;
		const char* m_pattern;
	};
} //namespace mchlib

#endif
