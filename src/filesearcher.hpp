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

#ifndef idB6D385B7779240308449D6081CB790F1
#define idB6D385B7779240308449D6081CB790F1

#include <vector>
#include <functional>
#include <boost/utility/string_ref.hpp>

namespace fastf {
  struct StringWithLength {
    StringWithLength ( const char* parStr, int parLen ) :
      str(parStr),
      len(parLen)
    {
    }

    const char* const str;
    const int len;
  };

  class FileSearcher {
  public:
    typedef std::vector<StringWithLength> ConstCharVecType;
	typedef std::function<bool(const char*, int, bool, bool)> CallbackType;

    explicit FileSearcher ( boost::string_ref parBaseDir );
    ~FileSearcher ( void ) noexcept;

    void Search ( const ConstCharVecType& parExtensions, const ConstCharVecType& parIgnorePaths );

    void SetFollowSymlinks ( bool parFollow ) noexcept { followSymlinks_ = parFollow; }
    bool FollowSymlinks ( void ) const noexcept { return followSymlinks_; }
    void SetRemainInFilesystem ( bool parRemain ) noexcept { remainInFilesystem_ = parRemain; }
    bool RemainInFilesystem ( void ) const noexcept { return remainInFilesystem_; }
	void SetCallback ( CallbackType parCallback );

  private:
  	CallbackType callback_;
    const std::string baseDir_;
    bool followSymlinks_;
    bool remainInFilesystem_;
  };
} //namespace fastf

#endif
