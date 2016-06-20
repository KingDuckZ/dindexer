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

#include "batch.hpp"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <cassert>
#include <future>
#include <ciso646>
#include <boost/iterator/transform_iterator.hpp>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace redis {
	namespace {
		const std::size_t g_max_redis_unanswered_commands = 1000;

		struct HiredisCallbackData {
			HiredisCallbackData ( std::atomic_size_t& parPendingFutures, std::condition_variable& parSendCmdCond ) :
				promise(),
				pending_futures(parPendingFutures),
				send_command_condition(parSendCmdCond)
			{
			}

			std::promise<Reply> promise;
			std::atomic_size_t& pending_futures;
			std::condition_variable& send_command_condition;
		};

		Reply make_redis_reply_type (redisReply* parReply) {
			using boost::transform_iterator;
			using PtrToReplyIterator = transform_iterator<Reply(*)(redisReply*), redisReply**>;

			switch (parReply->type) {
			case REDIS_REPLY_INTEGER:
				return parReply->integer;
			case REDIS_REPLY_STRING:
				return std::string(parReply->str, parReply->len);
			case REDIS_REPLY_ARRAY:
				return std::vector<Reply>(
					PtrToReplyIterator(parReply->element, &make_redis_reply_type),
					PtrToReplyIterator(parReply->element + parReply->elements, &make_redis_reply_type)
				);
			case REDIS_REPLY_ERROR:
				return ErrorString(parReply->str, parReply->len);
			default:
				return Reply();
			};
		}

		void hiredis_run_callback (redisAsyncContext*, void* parReply, void* parPrivData) {
			assert(parPrivData);
			auto* data = static_cast<HiredisCallbackData*>(parPrivData);
			const auto old_count = data->pending_futures.fetch_add(-1);
			assert(old_count > 0);
			if (old_count == g_max_redis_unanswered_commands)
				data->send_command_condition.notify_one();

			if (parReply) {
				auto reply = make_redis_reply_type(static_cast<redisReply*>(parReply));
				data->promise.set_value(std::move(reply));
			}

			delete data;
		}
	} //unnamed namespace

	struct Batch::LocalData {
		LocalData ( void ) :
			free_cmd_slot(),
			futures_mutex(),
			pending_futures(0)
		{
		}

		std::condition_variable free_cmd_slot;
		std::mutex futures_mutex;
		std::atomic_size_t pending_futures;
	};

	Batch::Batch (Batch&&) = default;

	Batch::Batch (redisAsyncContext* parContext) :
		m_futures(),
		m_replies(),
		m_local_data(new LocalData),
		m_context(parContext)
	{
		assert(m_context);
	}

	Batch::~Batch() noexcept {
	}

	void Batch::run_pvt (int parArgc, const char** parArgv, std::size_t* parLengths) {
		assert(not replies_requested());
		assert(parArgc >= 1);
		assert(parArgv);
		assert(parLengths); //This /could/ be null, but I don't see why it should

		const auto pending_futures = m_local_data->pending_futures.fetch_add(1);
		auto* data = new HiredisCallbackData(m_local_data->pending_futures, m_local_data->free_cmd_slot);

		std::cout << "run_pvt(), " << pending_futures << " items pending... ";
		if (pending_futures >= g_max_redis_unanswered_commands) {
			std::cout << " waiting... ";
			std::unique_lock<std::mutex> u_lock(m_local_data->futures_mutex);
			m_local_data->free_cmd_slot.wait(u_lock, [this]() { return m_local_data->pending_futures < g_max_redis_unanswered_commands; });
		}
		std::cout << "command sent to hiredis" << std::endl;

		m_futures.push_back(data->promise.get_future());
		const int command_added = redisAsyncCommandArgv(m_context, &hiredis_run_callback, data, parArgc, parArgv, parLengths);
		assert(REDIS_OK == command_added); // REDIS_ERR if error
	}

	bool Batch::replies_requested() const {
		return not m_replies.empty();
	}

	const std::vector<Reply>& Batch::replies() {
		if (not replies_requested()) {
			m_replies.reserve(m_futures.size());
			for (auto& fut : m_futures) {
				m_replies.push_back(fut.get());
			}

			auto empty_vec = std::move(m_futures);
		}
		return m_replies;
	}

	void Batch::throw_if_failed() {
		const auto& rep = replies();
		assert(false); //not implemented
	}

	RedisError::RedisError (const char* parMessage, std::size_t parLength) :
		std::runtime_error(std::string(parMessage, parLength))
	{
	}
} //namespace redis
