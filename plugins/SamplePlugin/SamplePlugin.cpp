#include "PluginInterface.hpp"
#include <iostream>
#include <string_view>

namespace plugin_demo {

class SamplePlugin final : public PluginInterface {
public:
    SamplePlugin() {
        std::cout << "[SamplePlugin] Instance created (Constructor)\n";
    }

    ~SamplePlugin() override {
        std::cout << "[SamplePlugin] Instance destroyed (Destructor)\n";
    }

    [[nodiscard]] std::string_view name() const noexcept override {
        return "SamplePlugin";
    }

    [[nodiscard]] std::string_view version() const noexcept override {
        return "1.0.0";
    }

    void execute() override {
        std::cout << "[SamplePlugin] Running core task workload...\n";
    }

    [[nodiscard]] double calculate(double a, double b) const override {
        return a * b + 42.0;
    }
};

} // namespace plugin_demo

// Exported C factory and cleanup functions
extern "C" {

#if defined(_WIN32)
    __declspec(dllexport)
#else
    __attribute__((visibility("default")))
#endif
plugin_demo::PluginInterface* create_plugin() {
    return new plugin_demo::SamplePlugin();
}

#if defined(_WIN32)
    __declspec(dllexport)
#else
    __attribute__((visibility("default")))
#endif
void destroy_plugin(plugin_demo::PluginInterface* plugin) {
    delete plugin;
}

}
