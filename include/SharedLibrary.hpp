#pragma once

#include <dlfcn.h>
#include <string>
#include <stdexcept>
#include <utility>

namespace plugin_demo {

/**
 * @brief RAII wrapper for dynamic shared library handles loaded via dlopen.
 */
class SharedLibrary {
public:
    explicit SharedLibrary(const std::string& path, int flags = RTLD_NOW | RTLD_LOCAL) {
        handle_ = ::dlopen(path.c_str(), flags);
        if (!handle_) {
            const char* err = ::dlerror();
            throw std::runtime_error("Failed to load shared library '" + path + "': " +
                                     (err ? err : "unknown error"));
        }
    }

    ~SharedLibrary() {
        if (handle_) {
            ::dlclose(handle_);
        }
    }

    SharedLibrary(const SharedLibrary&) = delete;
    SharedLibrary& operator=(const SharedLibrary&) = delete;

    SharedLibrary(SharedLibrary&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

    SharedLibrary& operator=(SharedLibrary&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                ::dlclose(handle_);
            }
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    template <typename FuncPtr>
    [[nodiscard]] FuncPtr get_symbol(const std::string& symbol_name) const {
        // Clear any pre-existing dlerror condition
        ::dlerror();

        void* symbol = ::dlsym(handle_, symbol_name.c_str());
        const char* err = ::dlerror();
        if (err != nullptr || symbol == nullptr) {
            throw std::runtime_error("Failed to resolve symbol '" + symbol_name + "': " +
                                     (err ? err : "symbol is null"));
        }

        return reinterpret_cast<FuncPtr>(symbol);
    }

    [[nodiscard]] void* native_handle() const noexcept {
        return handle_;
    }

private:
    void* handle_{nullptr};
};

} // namespace plugin_demo
