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

//See:
// http://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux

#include "backends/backend_loader.hpp"
#include "backends/exposed_functions.hpp"
#include "backends/backend_version.hpp"
#include <dlfcn.h>
#include <cassert>
#include <functional>

namespace dindb {
	namespace {
		BackendPtr load_backend (void* parSOHandle, const YAML::Node* parConfig) {
			typedef decltype(&dindexer_create_backend) CreateBackendFun;
			typedef decltype(&dindexer_destroy_backend) DeleteBackendFun;

			assert(parSOHandle);
			assert(parConfig);

			auto create = reinterpret_cast<CreateBackendFun>(dlsym(parSOHandle, "dindexer_create_backend"));
			auto destroy = reinterpret_cast<DeleteBackendFun>(dlsym(parSOHandle, "dindexer_destroy_backend"));

			return BackendPtr(create(parConfig), destroy);
		}

		const char* backend_name (void* parSOHandle) {
			typedef decltype(&dindexer_backend_name) GetNameFun;

			assert(parSOHandle);

			auto get_name = reinterpret_cast<GetNameFun>(dlsym(parSOHandle, "dindexer_backend_name"));
			return get_name();
		}

		void nop_destroy (Backend*) {
		}

		int backend_iface_version (void* parSOHandle) {
			typedef decltype(&dindexer_backend_iface_version) GetVersionFun;

			auto get_version = reinterpret_cast<GetVersionFun>(dlsym(parSOHandle, "dindexer_backend_iface_version"));
			return get_version();
		}
	} //unnamed namespace

	std::string backend_name (const std::string& parSOPath) {
		assert(not parSOPath.empty());
		using SoHandle = std::unique_ptr<void, int(*)(void*)>;

		auto handle = SoHandle(dlopen(parSOPath.c_str(), RTLD_LAZY), &dlclose);
		return backend_name(handle.get());
	}

	BackendPlugin::BackendPlugin() :
		m_lib(nullptr, &dlclose),
		m_backend(nullptr, &nop_destroy),
		m_name()
	{
	}

	BackendPlugin::BackendPlugin (const std::string& parSOPath, const YAML::Node* parConfig) :
		m_lib(dlopen(parSOPath.c_str(), RTLD_LAZY), &dlclose),
		m_backend(load_backend(m_lib.get(), parConfig)),
		m_name(backend_name(m_lib.get())),
		m_iface_ver(backend_iface_version(m_lib.get()))
	{
		if (g_current_iface_version != m_iface_ver) {
			m_backend.reset();
			m_lib.reset();
		}
	}

	BackendPlugin::~BackendPlugin() noexcept {
	}

	const boost::string_ref& BackendPlugin::name() const {
		return m_name;
	}

	Backend& BackendPlugin::backend() {
		assert(m_lib);
		assert(m_backend);
		if (not m_backend) {
			throw std::bad_function_call();
		}

		return *m_backend;
	}

	const Backend& BackendPlugin::backend() const {
		assert(m_lib);
		assert(m_backend);
		if (not m_backend) {
			throw std::bad_function_call();
		}

		return *m_backend;
	}

	bool BackendPlugin::is_loaded() const {
		return static_cast<bool>(m_backend);
	}

	int BackendPlugin::backend_interface_version() const {
		return m_iface_ver;
	}

	int BackendPlugin::max_supported_interface_version() const {
		return g_current_iface_version;
	}
} //namespace dindb
