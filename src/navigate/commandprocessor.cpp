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

#define BOOST_SPIRIT_UNICODE

#include "commandprocessor.hpp"
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <ciso646>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <sstream>

namespace din {
	struct ParsedCommand {
		std::string cmd;
		std::vector<std::string> param;
	};
} //namespace din
BOOST_FUSION_ADAPT_STRUCT(
	din::ParsedCommand,
	(std::string, cmd)
	(std::vector<std::string>, param)
);

namespace din {
	using StringList = std::vector<std::string>;

	namespace {
		bool wrap_void_noparam (const std::function<void()>& parCallback, const StringList&) {
			parCallback();
			return false;
		}

		bool wrap_void_oneparam (const std::function<void(const std::string&)>& parCallback, const StringList& parArgs) {
			assert(1 == parArgs.size());
			parCallback(parArgs.front());
			return false;
		}

		bool wrap_bool_noparam (const std::function<bool()>& parCallback, const StringList& parArgs) {
			static_cast<void>(parArgs);
			assert(parArgs.empty());
			return parCallback();
		}
	} //unnamed namespace

	struct CustomCommand {
		std::string name;
		CommandProcessor::CmdCallback callback;
		uint32_t exp_arg_count;

		bool operator== ( const std::pair<std::string, uint32_t>& parOther ) const {
			return name == parOther.first and exp_arg_count == parOther.second;
		}
	};

	template <typename Iterator>
	struct CommandGrammar : boost::spirit::qi::grammar<Iterator, ParsedCommand(), boost::spirit::unicode::space_type> {
		CommandGrammar ( void );

		boost::spirit::qi::rule<Iterator, ParsedCommand(), boost::spirit::unicode::space_type> start;
		boost::spirit::qi::rule<Iterator, std::string(), boost::spirit::unicode::space_type> quoted_string;
		boost::spirit::qi::rule<Iterator, std::string(), boost::spirit::unicode::space_type> string_arg;
		boost::spirit::qi::rule<Iterator, std::string(), boost::spirit::unicode::space_type> argument;
	};

	struct CommandProcessor::LocalData {
		using IteratorType = std::string::const_iterator;

		CommandGrammar<IteratorType> gramm;
		std::vector<CustomCommand> commands;
	};

	template <typename Iterator>
	CommandGrammar<Iterator>::CommandGrammar() :
		CommandGrammar<Iterator>::base_type(start)
	{
		using boost::spirit::qi::lexeme;
		using boost::spirit::unicode::char_;
		using boost::spirit::unicode::space;
		using boost::spirit::qi::string;

		quoted_string %= lexeme['"' >> *(char_ - '"') >> '"'];
		argument %= lexeme[+(string("\\ ") | (char_ - space))];
		string_arg %= quoted_string | argument;
		start = argument >> *string_arg;
	}

	CommandProcessor::CommandProcessor() :
		m_local_data(new LocalData)
	{
	}

	CommandProcessor::~CommandProcessor() noexcept {
	}

	bool CommandProcessor::exec_command (const std::string& parCommand) {
		LocalData::IteratorType cmd_begin = parCommand.begin();
		ParsedCommand result;
		const bool parse_result = boost::spirit::qi::phrase_parse(
			cmd_begin,
			parCommand.end(),
			m_local_data->gramm,
			boost::spirit::unicode::space,
			result
		);

		if (not parse_result or parCommand.end() != cmd_begin) {
			//parse error
			std::ostringstream oss;
			oss << "Error parsing given command at position " << (cmd_begin - parCommand.begin());
			throw std::runtime_error(oss.str());
		}

		{
			const uint32_t arg_count = static_cast<uint32_t>(result.param.size());
			const auto it_found = std::find(m_local_data->commands.begin(), m_local_data->commands.end(), std::make_pair(result.cmd, arg_count));
			if (m_local_data->commands.end() != it_found) {
				return not it_found->callback(result.param);
			}
			else {
				std::ostringstream oss;
				oss << "Parsed command \"" << result.cmd << "\" with " << arg_count << " argument(s) not recognized";
				throw std::runtime_error(oss.str());
			}
		}
	}

	void CommandProcessor::add_command (std::string&& parName, CmdCallback parCallback, uint32_t parExpParams) {
		//Check that parName is not an empty string
		if (parName.size() == 0) {
			throw std::runtime_error("Can't register an empty command in CommandProcessor");
		}

		//Check that parName+parExpParams hasn't been added already
		{
			const auto it_found = std::find(m_local_data->commands.begin(), m_local_data->commands.end(), std::make_pair(parName, parExpParams));
			if (m_local_data->commands.end() != it_found) {
				std::ostringstream oss;
				oss << "Inserting duplicate command \"" << parName << "\" taking " << parExpParams << " parameter(s)";
				throw std::runtime_error(oss.str());
			}
		}

		//Add the new command
		m_local_data->commands.push_back(CustomCommand {std::move(parName), std::move(parCallback), parExpParams});
	}

	void CommandProcessor::add_command (std::string&& parName, std::function<void()> parCallback, uint32_t parExpParams) {
		add_command(std::move(parName), std::bind(&wrap_void_noparam, std::move(parCallback), std::placeholders::_1), parExpParams);
	}

	void CommandProcessor::add_command (std::string&& parName, std::function<void(const std::string&)> parCallback, uint32_t parExpParams) {
		add_command(std::move(parName), std::bind(&wrap_void_oneparam, std::move(parCallback), std::placeholders::_1), parExpParams);
	}

	void CommandProcessor::add_command (std::string&& parName, std::function<bool()> parCallback, uint32_t parExpParams) {
		add_command(std::move(parName), std::bind(&wrap_bool_noparam, std::move(parCallback), std::placeholders::_1), parExpParams);
	}

	void CommandProcessor::add_command (std::string&& parName, void(*parCallback)(), uint32_t parExpParams) {
		add_command(std::move(parName), std::function<void()>(parCallback), parExpParams);
	}

	void CommandProcessor::add_command (std::string&& parName, bool(*parCallback)(), uint32_t parExpParams) {
		add_command(std::move(parName), std::function<bool()>(parCallback), parExpParams);
	}
} //namespace din
