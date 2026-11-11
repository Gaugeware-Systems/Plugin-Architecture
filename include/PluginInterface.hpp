#pragma once

#include <string_view>

namespace plugin_demo {

/**
 * @brief Base interface for dynamic plugins.
 *
 * Defines the contract that all dynamically loaded plugins must satisfy.
 */
class PluginInterface {
public:
    virtual ~PluginInterface() = default;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    [[nodiscard]] virtual std::string_view version() const noexcept = 0;
    virtual void execute() = 0;
    [[nodiscard]] virtual double calculate(double a, double b) const = 0;
};

// Function pointer signatures for C-linkage factory and deallocation hooks
using CreatePluginFn = PluginInterface* (*)();
using DestroyPluginFn = void (*)(PluginInterface*);

} // namespace plugin_demo
