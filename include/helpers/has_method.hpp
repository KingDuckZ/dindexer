/*
 * Copyright 2015 Michele "King_DuckZ" Santullo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef idFBC29C5127784D35BE62F7BAC16E3687
#define idFBC29C5127784D35BE62F7BAC16E3687

#define define_has_method(method_name,pretty_name) \
	template <typename T> \
	struct Has ## pretty_name ## Method { \
	private: \
		struct TrueType { int a[2]; }; \
		typedef int FalseType; \
		template <typename C> static TrueType has_method ( decltype(&C::method_name) ); \
		template <typename C> static FalseType has_method ( ... ); \
	public: \
		enum { value = sizeof(has_method<T>(0)) == sizeof(TrueType) }; \
	}
#define define_has_typedef(typedef_name,pretty_name) \
	template <typename T> \
	struct Has ## pretty_name ## Typedef { \
	private: \
		struct TrueType { int a[2]; }; \
		typedef int FalseType; \
		template <typename C> static TrueType has_typedef ( const typename C::typedef_name* ); \
		template <typename C> static FalseType has_typedef ( ... ); \
	public: \
		enum { value = sizeof(has_typedef<T>(nullptr)) == sizeof(TrueType) }; \
	}
#define define_has_enum(enum_name,pretty_name) \
	template <typename T> \
	struct Has ## pretty_name ## Enum { \
	private: \
		struct TrueType { int a[2]; }; \
		typedef int FalseType; \
		template <typename C> static TrueType has_enum ( decltype(C::enum_name)* ); \
		template <typename C> static FalseType has_enum ( ... ); \
	public:\
		enum { value = sizeof(has_enum<T>(nullptr)) == sizeof(TrueType) }; \
	}

#endif
