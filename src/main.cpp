#include "PluginLoader.hpp"

#include <iostream>
#include <filesystem>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=========================================\n";
    std::cout << " C++20 Dynamic Plugin Architecture Demo  \n";
    std::cout << "=========================================\n\n";

    // Determine plugin path from CLI argument or default build location
    std::filesystem::path plugin_path = "plugins/sample_plugin.so";
    if (argc > 1) {
        plugin_path = argv[1];
    }

    try {
        std::cout << "[Host] Attempting to load plugin from: " << plugin_path << "\n";

        {
            // Scope block to verify RAII destruction and library unload
            std::cout << "[Host] Entering scoped block for plugin usage...\n";
            plugin_demo::PluginPtr plugin = plugin_demo::PluginLoader::load(plugin_path);

            std::cout << "[Host] Successfully loaded plugin!\n";
            std::cout << "[Host]   - Plugin Name:    " << plugin->name() << "\n";
            std::cout << "[Host]   - Plugin Version: " << plugin->version() << "\n";

            std::cout << "[Host] Invoking plugin->execute():\n";
            plugin->execute();

            double a = 5.0;
            double b = 10.0;
            double result = plugin->calculate(a, b);
            std::cout << "[Host] Invoking plugin->calculate(" << a << ", " << b << ") = " << result << "\n";

            std::cout << "[Host] Exiting scoped block (plugin unique_ptr will now be destroyed)...\n";
        }

        std::cout << "[Host] Scoped block exited. Plugin object and shared library unloaded safely via RAII.\n";

    } catch (const std::exception& ex) {
        std::cerr << "[Host Error] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
