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

#ifndef id810F3D0C21864315B17EF76E83510B6D
#define id810F3D0C21864315B17EF76E83510B6D

#include <stddef.h>
#include <stdint.h>
#include "duckhandy/compatibility.h"

typedef uint32_t Character;

size_t utf8_strlen ( const char* parString ) a_pure;
Character utf8_advance ( const char** parString, const char* parStringEnd );

#endif
