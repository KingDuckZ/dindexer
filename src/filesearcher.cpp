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

#include "filesearcher.hpp"

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 500
#endif

#include <ftw.h>
#include <iostream>
#include <sys/types.h>
#include <cstring>

namespace fastf {
  extern "C" int PrintName ( const char* parPath, const struct stat* parStat, int parType, FTW* parBuff );

  namespace {
    struct SearchOptions {
      FileSearcher::CallbackType* callback;
      const FileSearcher::ConstCharVecType* extensions;
      const FileSearcher::ConstCharVecType* ignorePaths;
      int search_flags;
    };

    __thread SearchOptions g_searchOptions;

    bool print_def_callback (const char* parPath, int parLevel, bool parDir, bool parSymlink) {
      std::cout << parPath;
      if (parDir) {
        std::cout << '/';
      }
      if (parSymlink) {
        std::cout << " (symlink)";
      }
      std::cout << " (" << parLevel << ')';
	  std::cout << '\n';
      return true;
    }

    int HandleDir (const char* parPath, const struct stat*, FTW* parBuff, bool parCanBeRead, bool parSymlink) {
      if (not (*g_searchOptions.callback)(parPath, parBuff->level, true, parSymlink))
        return FTW_STOP;
      if (!parCanBeRead)
        return FTW_SKIP_SUBTREE;

      const FileSearcher::ConstCharVecType& ignorePaths = *g_searchOptions.ignorePaths;
      for (int z = 0; z < static_cast<int>(ignorePaths.size()); ++z) {
        if (std::strncmp(ignorePaths[z].str, parPath + parBuff->base, ignorePaths[z].len) == 0)
          return FTW_SKIP_SUBTREE;
      }
      return FTW_CONTINUE;
    }

    int HandleFile (const char* parPath, const struct stat*, FTW* parBuff, bool parSymlink) {
      const FileSearcher::ConstCharVecType& extensions = *g_searchOptions.extensions;
      if (extensions.empty()) {
        if (not (*g_searchOptions.callback)(parPath, parBuff->level, false, parSymlink)) {
          return FTW_STOP;
        }
      }
      else {
        for (int z = 0; z < static_cast<int>(extensions.size()); ++z) {
          const int& extLen = extensions[z].len;
          const int pathLen = parBuff->base + std::strlen(parPath + parBuff->base);
          if (std::strncmp(extensions[z].str, parPath + pathLen - extLen, extensions[z].len) == 0) {
            if (not (*g_searchOptions.callback)(parPath, parBuff->level, false, parSymlink)) {
              return FTW_STOP;
            }
            break;
          }
        }
      }
      return FTW_CONTINUE;
    }

    int HandleLink (const char* parPath, const struct stat* parStat, FTW* parBuff, bool parBroken) {
      if (parBroken)
        return FTW_CONTINUE;
      else
        return HandleFile(parPath, parStat, parBuff, true);
    }

    extern "C" int PrintName (const char* parPath, const struct stat* parStat, int parType, FTW* parBuff) {
      struct stat st;
      lstat(parPath, &st);
      const bool is_symlink = S_ISLNK(st.st_mode);

      switch (parType) {
      //parPath is a directory which can't be read
      case FTW_DNR:
      //parPath is a directory
      case FTW_D:
        return HandleDir(parPath, parStat, parBuff, static_cast<bool>(FTW_DNR != parType), is_symlink);

      //parPath is a regular file
      case FTW_F:
        return HandleFile(parPath, parStat, parBuff, is_symlink);

      //The stat(2) call failed on parPath, which is not a symbolic link
      case FTW_NS:
        return FTW_CONTINUE;

      //parPath is a symbolic link pointing to a nonexistent file (This occurs only if FTW_PHYS is not set)
      case FTW_SLN:
      //parPath is a symbolic link, and FTW_PHYS was set in flags
      case FTW_SL:
        return HandleLink(parPath, parStat, parBuff, static_cast<bool>(FTW_SLN == parType));
      }

      return FTW_CONTINUE;
    }
  } //unnamed namespace

  FileSearcher::FileSearcher (const char* parBaseDir) :
    callback_(&print_def_callback),
    baseDir_(parBaseDir),
    followSymlinks_(false),
    remainInFilesystem_(true)
  {
  }

  FileSearcher::~FileSearcher() noexcept {
  }

  void FileSearcher::Search (const ConstCharVecType& parExtensions, const ConstCharVecType& parIgnorePaths) {
    g_searchOptions.search_flags = FTW_ACTIONRETVAL;
    if (remainInFilesystem_)
      g_searchOptions.search_flags |= FTW_MOUNT;
    if (!followSymlinks_)
      g_searchOptions.search_flags |= FTW_PHYS;

    g_searchOptions.extensions = &parExtensions;
    g_searchOptions.ignorePaths = &parIgnorePaths;
    g_searchOptions.callback = &callback_;
    nftw(baseDir_, &PrintName, 15, g_searchOptions.search_flags);
  }

  void FileSearcher::SetCallback (CallbackType parCallback) {
    callback_ = parCallback;
  }
} //namespace fastf
