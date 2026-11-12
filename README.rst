======================================
C++20 Dynamic Plugin Architecture Demo
======================================

.. image:: https://img.shields.io/badge/C%2B%2B-20-blue.svg
   :target: https://en.cppreference.com/w/cpp/20
   :alt: C++20
.. image:: https://img.shields.io/badge/CMake-3.25%2B-064F8C.svg?logo=cmake&logoColor=white
   :target: https://cmake.org/
   :alt: CMake 3.25+
.. image:: https://img.shields.io/badge/Platform-Linux%20%2F%20POSIX-lightgrey.svg
   :alt: Platform Linux / POSIX

A modern C++20 dynamic plugin architecture skeleton demonstrating runtime shared library loading using ``dlopen``, ``dlsym``, and ``dlclose`` in POSIX/Linux environments, managed via CMake presets, Boost.Program_options, and spdlog.

Project Layout
==============

::

    .
    ├── CMakeLists.txt                      # Root CMake configuration
    ├── CMakePresets.json                   # CMake Presets (debug, release, workflows)
    ├── .gitignore                          # Ignores build artifacts
    ├── README.rst                          # Project documentation and guide
    ├── cmake/
    │   └── toolchains/
    │       ├── linux-gcc.cmake             # GCC toolchain configuration
    │       └── linux-llvm.cmake            # LLVM/Clang toolchain configuration
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
        └── main.cpp                        # Host application demonstrating runtime loading, CLI parsing, and logging

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

4. **Modern CMake & Presets Integration** (``CMakeLists.txt``, ``CMakePresets.json``, ``cmake/toolchains/``):
   - Configures ``MODULE`` library targets for runtime plugins (``sample_plugin.so``).
   - Configures symbol visibility (``CXX_VISIBILITY_PRESET hidden``, ``VISIBILITY_INLINES_HIDDEN ON``) with explicit export annotations.
   - Organizes output artifacts cleanly into ``bin/`` (for executables) and ``plugins/`` (for loadable plugins) under each preset's build directory (e.g., ``build/debug/``, ``build/release/``).
   - Utilizes standard CMake Presets (v6) with Ninja generator and toolchain files (``linux-gcc.cmake``, ``linux-llvm.cmake``) for consistent configure, build, test (CTest), and workflow pipelines.
   - Links dynamic loader facilities (``${CMAKE_DL_LIBS}``), ``Boost::program_options``, and ``spdlog::spdlog``.

5. **Structured CLI & Logging** (``src/main.cpp``):
   - **Command-Line Interface**: Integrated ``boost::program_options`` to manage arguments, providing ``-h`` / ``--help`` documentation and ``-p`` / ``--plugin`` selection for shared library paths with default fallback.
   - **Console Logging**: Integrated ``spdlog`` with colored stdout sink for formatted runtime diagnostics, plugin lifecycle tracking, and error reporting.

Building and Running
====================

Prerequisites
-------------

- CMake 3.25 or newer (required for CMakePresets version 6)
- A C++20-compliant compiler (e.g. GCC 10+, Clang 12+)
- Ninja build system
- Boost.Program_options development files
- spdlog development files
- Linux / POSIX dynamic linker (``libdl``)

On Debian/Ubuntu systems, install dependencies using:

.. code-block:: bash

    sudo apt-get update
    sudo apt-get install --yes cmake ninja-build build-essential libboost-program-options-dev libspdlog-dev

Building
--------

The project uses CMake Presets configured with the Ninja generator. Two presets are defined: ``debug`` and ``release``.

Using Workflow Presets
^^^^^^^^^^^^^^^^^^^^^^

Workflow presets execute configure, build, and test steps in a single command:

.. code-block:: bash

    # Debug pipeline (builds under build/debug)
    cmake --workflow --preset debug

    # Release pipeline (builds under build/release)
    cmake --workflow --preset release

Using Individual Presets
^^^^^^^^^^^^^^^^^^^^^^^^

You can also run configure, build, and test steps individually:

.. code-block:: bash

    # Configure
    cmake --preset debug
    # or: cmake --preset release

    # Build
    cmake --build --preset debug
    # or: cmake --build --preset release

    # Run tests via CTest
    ctest --preset debug
    # or: ctest --preset release

Build Artifacts Layout
^^^^^^^^^^^^^^^^^^^^^^

Artifacts are placed in preset-specific build directories:

- Host executable: ``build/<preset>/bin/host_app``
- Plugin shared library: ``build/<preset>/plugins/sample_plugin.so``

Running the Host Application
----------------------------

The host application uses ``boost::program_options`` to provide a command-line interface.

CLI Options
^^^^^^^^^^^

::

    Allowed options:
      -h [ --help ]                         Show help message
      -p [ --plugin ] arg (=sample_plugin.so)
                                            Path to the plugin shared library

CLI Usage Examples
^^^^^^^^^^^^^^^^^^

Display help message:

.. code-block:: bash

    ./build/debug/bin/host_app --help

Run specifying a plugin path with ``-p`` or ``--plugin``:

.. code-block:: bash

    # Using debug build
    ./build/debug/bin/host_app -p ./build/debug/plugins/sample_plugin.so

    # Using release build
    ./build/release/bin/host_app --plugin ./build/release/plugins/sample_plugin.so

Verification Results
====================

The project was configured and compiled with GCC and C++20 (``-Wall -Wextra -Wpedantic``) using CMake presets:

::

    $ cmake --preset debug && cmake --build --preset debug
    -- The CXX compiler identification is GNU 15.2.0
    -- Detecting CXX compiler ABI info
    -- Detecting CXX compiler ABI info - done
    -- Check for working CXX compiler: /usr/bin/g++ - skipped
    -- Detecting CXX compile features
    -- Detecting CXX compile features - done
    -- Performing Test CMAKE_HAVE_LIBC_PTHREAD
    -- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
    -- Found Threads: TRUE
    -- Configuring done (1.4s)
    -- Generating done (0.0s)
    -- Build files have been written to: /home/rpdaly/Projects/Plugin-Architecture/build/debug
    [4/4] Linking CXX executable bin/host_app

Execution output demonstrating CLI parsing, spdlog logging, safe instantiation, execution, and scoped RAII teardown:

::

    $ ./build/debug/bin/host_app -p ./build/debug/plugins/sample_plugin.so
    [2026-11-11 21:08:11.757] [Host] [info] =========================================
    [2026-11-11 21:08:11.757] [Host] [info]  C++20 Dynamic Plugin Architecture Demo  
    [2026-11-11 21:08:11.757] [Host] [info] =========================================
    [2026-11-11 21:08:11.757] [Host] [info] 
    [2026-11-11 21:08:11.757] [Host] [info] Using plugin path: ./build/debug/plugins/sample_plugin.so
    [Host] Attempting to load plugin from: "./build/debug/plugins/sample_plugin.so"
    [Host] Entering scoped block for plugin usage...
    [SamplePlugin] Instance created (Constructor)
    [2026-11-11 21:08:11.758] [Host] [info] [Host] Successfully loaded plugin!
    [2026-11-11 21:08:11.758] [Host] [info] [Host]   - Plugin Name:    SamplePlugin
    [2026-11-11 21:08:11.758] [Host] [info] [Host]   - Plugin Version: 1.0.0
    [2026-11-11 21:08:11.758] [Host] [info] [Host] Invoking plugin->execute()...
    [SamplePlugin] Running core task workload...
    [2026-11-11 21:08:11.758] [Host] [info] [Host] Invoking plugin->calculate(5, 10) = 92
    [2026-11-11 21:08:11.758] [Host] [info] [Host] Exiting scoped block (plugin unique_ptr will now be destroyed)...
    [SamplePlugin] Instance destroyed (Destructor)
    [2026-11-11 21:08:11.758] [Host] [info] [Host] Scoped block exited. Plugin object and shared library unloaded safely via RAII.
