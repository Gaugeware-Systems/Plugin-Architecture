#pragma once

#include "PluginInterface.hpp"
#include "SharedLibrary.hpp"

#include <memory>
#include <string>
#include <filesystem>
#include <stdexcept>

namespace plugin_demo {

/**
 * @brief Custom deleter for PluginInterface instances.
 *
 * Holds a shared reference to the SharedLibrary instance. This guarantees that
 * dlclose() is not invoked until after the plugin's C++ destructor and custom
 * deallocator have finished running.
 */
class PluginDeleter {
public:
    PluginDeleter() = default;

    PluginDeleter(std::shared_ptr<SharedLibrary> lib, DestroyPluginFn destroy_fn)
        : library_(std::move(lib)), destroy_fn_(destroy_fn) {}

    void operator()(PluginInterface* ptr) const noexcept {
        if (ptr && destroy_fn_) {
            destroy_fn_(ptr);
        }
        // library_ will be destructed after this lambda/functor completes,
        // decrementing the reference count and calling dlclose() safely if count reaches 0.
    }

private:
    std::shared_ptr<SharedLibrary> library_{nullptr};
    DestroyPluginFn destroy_fn_{nullptr};
};

using PluginPtr = std::unique_ptr<PluginInterface, PluginDeleter>;

/**
 * @brief Factory class to load dynamic plugins from disk.
 */
class PluginLoader {
public:
    /**
     * @brief Loads a plugin shared library and instantiates the plugin object.
     * @param path Path to the shared library file (.so / .dylib / .dll).
     * @return PluginPtr A unique_ptr managing the plugin instance with RAII lifetime.
     */
    [[nodiscard]] static PluginPtr load(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Plugin file does not exist: " + path.string());
        }

        auto lib = std::make_shared<SharedLibrary>(path.string());
        auto create_fn = lib->get_symbol<CreatePluginFn>("create_plugin");
        auto destroy_fn = lib->get_symbol<DestroyPluginFn>("destroy_plugin");

        PluginInterface* raw_instance = create_fn();
        if (!raw_instance) {
            throw std::runtime_error("create_plugin() returned nullptr in: " + path.string());
        }

        return PluginPtr(raw_instance, PluginDeleter(std::move(lib), destroy_fn));
    }
};

} // namespace plugin_demo
