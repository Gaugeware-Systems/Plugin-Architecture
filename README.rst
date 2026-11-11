======================================
C++20 Dynamic Plugin Architecture Demo
======================================

.. image:: https://img.shields.io/badge/C%2B%2B-20-blue.svg
   :target: https://en.cppreference.com/w/cpp/20
   :alt: C++20
.. image:: https://img.shields.io/badge/CMake-3.20%2B-064F8C.svg?logo=cmake&logoColor=white
   :target: https://cmake.org/
   :alt: CMake 3.20+
.. image:: https://img.shields.io/badge/Platform-Linux%20%2F%20POSIX-lightgrey.svg
   :alt: Platform Linux / POSIX

A modern C++20 dynamic plugin architecture skeleton demonstrating runtime shared library loading using ``dlopen``, ``dlsym``, and ``dlclose`` in POSIX/Linux environments, managed via CMake.

Project Layout
==============

::

    .
    ├── CMakeLists.txt                      # Root CMake configuration
    ├── .gitignore                          # Ignores build artifacts
    ├── README.rst                          # Project documentation and guide
    ├── include/
    │   ├── PluginInterface.hpp             # Pure virtual base class and C-linkage function signatures
    │   ├── SharedLibrary.hpp               # RAII wrapper for dlopen, dlsym, dlclose
    │   └── PluginLoader.hpp                # Factory creating std::unique_ptr with safe custom deleter
    ├── plugins/
    │   ├── CMakeLists.txt                  # Plugins directory
    │   └── SamplePlugin/
    │       ├── CMakeLists.txt              # Builds sample_plugin as a CMake MODULE library
    │       └── SamplePlugin.cpp            # Child class implementation and export symbols
    └── src/
        └── main.cpp                        # Host application demonstrating runtime loading

Key Architectural Highlights
============================

1. **Pure Virtual Base Contract** (``include/PluginInterface.hpp``):
   - Defines a standard C++20 interface with a virtual destructor and pure virtual methods (``name()``, ``version()``, ``execute()``, ``calculate()``).
   - Declares standard C-linkage export signatures (``create_plugin``, ``destroy_plugin``) for binary portability and avoiding C++ ABI name mangling issues across shared library boundaries.

2. **RAII-Safe Dynamic Loading** (``include/SharedLibrary.hpp``):
   - Encapsulates ``dlopen``, ``dlsym``, and ``dlclose`` within a move-only RAII container.
   - Provides type-safe symbol resolution via ``get_symbol<FuncPtr>()`` with detailed error reporting via ``dlerror()``.
   - Throws descriptive ``std::runtime_error`` exceptions with system error messages on failures.

3. **Safe Memory Deallocation & Lifetime Ordering** (``include/PluginLoader.hpp``):
   - Memory allocated inside the shared library (``new SamplePlugin()``) is deallocated inside the same library (``delete plugin``), avoiding cross-boundary allocator mismatch.
   - Returns a ``std::unique_ptr<PluginInterface, PluginDeleter>`` where the custom deleter holds a reference-counted handle (``std::shared_ptr<SharedLibrary>``) to the loaded library.
   - **Guaranteed Destruction Order**: When the ``unique_ptr`` goes out of scope, the plugin instance's destructor and deallocator run *before* ``dlclose()`` unmaps the code segment, preventing segmentation faults.

4. **CMake Integration** (``CMakeLists.txt``, ``plugins/SamplePlugin/CMakeLists.txt``):
   - Configures ``MODULE`` library targets for runtime plugins (``sample_plugin.so``).
   - Configures symbol visibility (``CXX_VISIBILITY_PRESET hidden``, ``VISIBILITY_INLINES_HIDDEN ON``) with explicit export annotations.
   - Links ``${CMAKE_DL_LIBS}`` for the host executable for POSIX dynamic loader support.

Building and Running
====================

Prerequisites
-------------

- CMake 3.20 or newer
- A C++20-compliant compiler (e.g. GCC 10+, Clang 12+)
- Linux / POSIX dynamic linker (``libdl``)

Build
-----

.. code-block:: bash

    cmake -B build -S .
    cmake --build build

Run
---

.. code-block:: bash

    # Run host app specifying the path to the plugin
    ./build/bin/host_app ./build/plugins/sample_plugin.so

Verification Results
====================

The project was configured and compiled with GCC and C++20 (``-Wall -Wextra -Wpedantic``):

::

    $ cmake -B build -S . && cmake --build build
    [ 50%] Built target host_app
    [100%] Built target sample_plugin

Execution output demonstrating safe instantiation, execution, and scoped RAII teardown:

::

    $ ./build/bin/host_app ./build/plugins/sample_plugin.so
    =========================================
     C++20 Dynamic Plugin Architecture Demo  
    =========================================

    [Host] Attempting to load plugin from: "./build/plugins/sample_plugin.so"
    [Host] Entering scoped block for plugin usage...
    [SamplePlugin] Instance created (Constructor)
    [Host] Successfully loaded plugin!
    [Host]   - Plugin Name:    SamplePlugin
    [Host]   - Plugin Version: 1.0.0
    [Host] Invoking plugin->execute():
    [SamplePlugin] Running core task workload...
    [Host] Invoking plugin->calculate(5, 10) = 92
    [Host] Exiting scoped block (plugin unique_ptr will now be destroyed)...
    [SamplePlugin] Instance destroyed (Destructor)
    [Host] Scoped block exited. Plugin object and shared library unloaded safely via RAII.
