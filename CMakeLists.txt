# arm-none-eabi-cmake
cmake_minimum_required(VERSION 3.13)

# Declare project containing a single eponymous executable
project(citro3d_testing LANGUAGES C CXX ASM)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# declare an asset target for the executable's RomFS (optional)
dkp_add_asset_target(${PROJECT_NAME}_ctr_romfs romfs)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(APP_TITLE "citro3d_testing (${COMMIT_HASH})")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__=0)
endif()

ctr_add_shader_library(main_v_pica
    "shaders/main.v.pica"
)

# Generate a SMDH file for the executable
ctr_generate_smdh(${PROJECT_NAME}.smdh
    NAME        "${APP_TITLE}"

    # Other options available:
    DESCRIPTION "citro3d Testing"
    AUTHOR      "TurtleP"
)

dkp_install_assets (${PROJECT_NAME}_ctr_romfs
    DESTINATION "shaders"
    TARGETS
        main_v_pica
)

# Specify that the executable is a 3DSX file
ctr_create_3dsx(${PROJECT_NAME}
    SMDH  ${PROJECT_NAME}.smdh
    ROMFS ${PROJECT_NAME}_ctr_romfs
)

# Options for code generation
target_compile_options(${PROJECT_NAME} PRIVATE
    # Common C/C++ options
    -Wall -Wno-psabi

    # C++ specific options
    $<$<COMPILE_LANGUAGE:CXX>:-fexceptions -fno-rtti>
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# Source code files
# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
source/exception.cpp
source/framebuffer.cpp
source/main.cpp
source/matrix.cpp
source/renderer.cpp
source/shader.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE citro3d)