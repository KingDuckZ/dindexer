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

#ifndef id45CDD1DAEF4F42968E3C89F68FDDA9BC
#define id45CDD1DAEF4F42968E3C89F68FDDA9BC

#if defined(__GNUC__)
#	if defined(__clang__)
#		if !defined(__has_attribute)
			//Fall back to version number comparing
#		else
#			if __has_attribute(flatten)
#				define a_flatten __attribute__((flatten))
#			else
#				define a_flatten
#			endif
#			if __has_attribute(always_inline)
#				define a_always_inline __attribute__((always_inline))
#			else
#				define a_always_inline
#			endif
#			if __has_attribute(pure)
#				define a_pure __attribute__((pure))
#			else
#				define a_pure
#			endif
#			if __has_attribute(deprecated)
#				define a_deprecated __attribute__((deprecated))
#			else
#				define a_deprecated
#endif
#		endif
#	else
		//Fix here if you get warnings about unsupported attributes on your compiler
#		define a_flatten __attribute__((flatten))
#		define a_always_inline __attribute__((always_inline))
#		define a_pure __attribute__((pure))
#		define a_deprecated __attribute__((deprecated))
#	endif
#else
#	warning "Unsupported compiler, please fill this section or file a bug"
#	define a_flatten
#	define a_always_inline
#	define a_pure
#	define a_deprecated
#endif

#endif
