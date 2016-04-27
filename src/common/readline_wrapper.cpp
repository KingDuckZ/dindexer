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

#include "dindexer-common/readline_wrapper.hpp"
#include <cstring>
#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>
#include <cassert>
#include <ciso646>
#include <memory>

#define WITH_MULTITHREAD_READLINE

#if defined(WITH_MULTITHREAD_READLINE)
#	include <mutex>
#endif

namespace dinlib {
	namespace {
		const ReadlineWrapper* g_current_wrapper = nullptr;
		bool g_readline_initialized = false;
#if defined(WITH_MULTITHREAD_READLINE)
		std::mutex g_lock_wrapper;
#endif

		char* custom_generator (const char* parText, int parState) {
			assert(g_current_wrapper);
			assert(parText);

			static std::size_t list_index;
			static std::vector<std::string> possible_values;

			if (not parState) {
				list_index = 0;
				possible_values = g_current_wrapper->matches(parText);
			}
			if (static_cast<std::size_t>(parState) >= possible_values.size()) {
				possible_values.clear();
				list_index = 0;
				return nullptr;
			}
			else {
				const auto& match = possible_values[list_index];
				++list_index;
				return strdup(match.c_str());
			}
		}

		//char** custom_completion (const char* parText, int parStart, int parEnd) {
		//	char** matches = nullptr;

		//	if (0 == parStart) {
		//		matches = rl_completion_matches(const_cast<char*>(parText), &custom_generator);
		//	}
		//	else {
		//		//See the hack described here:
		//		//http://cc.byexamples.com/2008/06/16/gnu-readline-implement-custom-auto-complete/
		//		rl_bind_key('\t', &rl_abort);
		//	}
		//	return matches;
		//}
	} //unnamed namespace

	ReadlineWrapper::ReadlineWrapper (MatchesCallback parMatchesCallback) :
		m_fetch_matches(parMatchesCallback)
	{
		if (not g_readline_initialized) {
			g_readline_initialized = true;
			//rl_attempted_completion_function = &custom_completion;
			rl_completion_entry_function = &custom_generator;
			rl_bind_key('\t', &rl_complete);
		}
	}

	ReadlineWrapper::~ReadlineWrapper() noexcept {
	}

	std::string ReadlineWrapper::read (const std::string& parMessage) const {
		typedef std::unique_ptr<char, void(*)(void*)> RawCharMemory;

#if defined(WITH_MULTITHREAD_READLINE)
		std::lock_guard<std::mutex> guard(g_lock_wrapper);
#endif

		g_current_wrapper = this;
		RawCharMemory line(readline(parMessage.c_str()), &std::free);

		if (line) {
			if (*line) {
				add_history(line.get());
			}
			return std::string(line.get());
		}
		else {
			return std::string();
		}
	}

	const std::vector<std::string>& ReadlineWrapper::matches (const std::string& parPrefix) const {
		if (parPrefix != m_last_prefix) {
			m_last_prefix = parPrefix;
			m_last_matches = m_fetch_matches(parPrefix);
		}
		return m_last_matches;
	}
} //namespace dinlib
