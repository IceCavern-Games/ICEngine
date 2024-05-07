cmake_minimum_required(VERSION 3.21)

project(ICEngine)

##############################################
# Global Options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

##############################################
# Platform Variables
option(BUILD_IC_EDITOR "Build included editor tool" OFF)
option(IC_RENDERER_VULKAN "Make Vulkan Renderer available" OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_definitions(IC_PLATFORM_MACOS)
endif()

##############################################
# Declare Dependencies
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

# Tracks which libraries we need to link, depending on the options above.
set(LIBS glfw glm::glm)

if (IC_RENDERER_VULKAN)
    message("Vulkan Renderer Enabled")
    find_package(Vulkan REQUIRED)

    add_compile_definitions(IC_RENDERER_VULKAN)
    set(LIBS ${LIBS} Vulkan::Vulkan)

    set(SOURCE_FILES__RENDERER
        src/vulkan/descriptors.cpp
        src/vulkan/pipelines.cpp
        src/vulkan/swap_chain.cpp
        src/vulkan/vulkan_device.cpp
        src/vulkan/vulkan_initializers.cpp
        src/vulkan/vulkan_renderer.cpp
        src/vulkan/vulkan_util.cpp
    )

    set (INCLUDE_DIR__RENDERER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan
    )
endif()

##############################################
# Create target and set properties
add_library(${PROJECT_NAME}
    src/ic_app.cpp
    src/ic_camera.cpp
    src/ic_graphics.cpp
    src/ic_log.cpp
    src/ic_renderer.cpp
)

# Renderer sources
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES__RENDERER})

# This uses C++20 in its public interface, so CMake projects using ICEngine must support it too.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# This is necessary so that a static libICEngine.a can be used dynamically.
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ICENGINE_EXPORTS)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Renderer include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR__RENDERER})

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${LIBS}
    PUBLIC
        imgui::imgui
        spdlog::spdlog_header_only
        tinyobjloader::tinyobjloader
        ${Stb_INCLUDE_DIR}
)

##############################################
# Installation Instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR share/icengine)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icengine
)

# Export the targets to a script
install(
    EXPORT ${PROJECT_NAME}_targets
    FILE icengine-targets.cmake
    NAMESPACE IC::
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/icengine-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/icengine-config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/icengine-config.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT ${PROJECT_NAME}_targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/icengine-targets.cmake
    NAMESPACE IC::
)

##############################################
# Shaders

add_subdirectory(resources/shaders)

##############################################
# Editor

# Alias our library as IC::ICEngine so that examples
# and submodules can depend on it by its normal name.
add_library(IC::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (BUILD_IC_EDITOR)
    add_subdirectory(editor)
endif()
