#include "mimetype.hpp"
#include <magic.h>
#include <ciso646>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace din {
	using MagicCookie = std::unique_ptr<magic_set, void(*)(magic_t)>;

	struct MimeType::LocalData {
		LocalData ( void ) :
			magic_cookie(nullptr, &magic_close)
		{
		}

		MagicCookie magic_cookie;
	};

	MimeType::MimeType() :
		m_local_data(new LocalData)
	{
		auto raw_cookie = magic_open(MAGIC_SYMLINK | MAGIC_MIME | MAGIC_PRESERVE_ATIME | MAGIC_ERROR);
		MagicCookie cookie(raw_cookie, &magic_close);

		if (raw_cookie) {
			if (0 != magic_load(raw_cookie, nullptr)) {
				std::ostringstream oss;
				oss << "MimeType cannot load magic database: \"" <<
					magic_error(raw_cookie) <<
					"\"";

				throw std::runtime_error(oss.str());
			}
		}

		std::swap(m_local_data->magic_cookie, cookie);
	}

	MimeType::~MimeType() noexcept {
	}

	bool MimeType::initialized() const {
		return not not m_local_data->magic_cookie;
	}

	const char* MimeType::analyze (const std::string& parPath) {
		if (initialized()) {
			const auto cookie = m_local_data->magic_cookie.get();
			const auto retval = magic_file(cookie, parPath.c_str());
			if (not retval) {
				std::ostringstream oss;
				oss << "MimeType failed to analyze \"" <<
					parPath << "\": \"" << magic_error(cookie) << "\"";

				throw std::runtime_error(oss.str());
			}
			return retval;
		}
		else {
			return nullptr;
		}
	}
} //namespace din
