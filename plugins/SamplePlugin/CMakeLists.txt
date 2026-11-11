add_library(sample_plugin MODULE
    SamplePlugin.cpp
)

target_include_directories(sample_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(sample_plugin PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PREFIX ""
)
