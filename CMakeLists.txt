cmake_minimum_required(VERSION 3.21)
project(IceCavernEngine)

include(GNUInstallDirs)

# Use C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Library
add_library(${PROJECT_NAME}
    src/ic_app.cpp
)

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

# Platform Variables
option(IC_RENDERER_VULKAN "Make Vulkan Renderer available" OFF)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Tracks which libraries we need to link, depending on the options above.
set(LIBS glfw glm::glm)

if (IC_RENDERER_VULKAN)
    message("Vulkan Renderer Enabled")
    find_package(Vulkan REQUIRED)

    add_compile_definitions(IC_RENDERER_VULKAN)
    set(LIBS ${LIBS} Vulkan::Vulkan)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# Install the library and its headers
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(
    FILES
        include/ic.h
        include/ic_app.h
    DESTINATION include/icengine
)

# Generate and install *-targets.cmake
install(EXPORT ${PROJECT_NAME}_targets
        FILE icengine-targets.cmake
        NAMESPACE IC::
        DESTINATION share/icengine)

# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/icengine-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(glfw3 CONFIG REQUIRED)\n"
"find_dependency(glm CONFIG REQUIRED)\n"
"find_dependency(Vulkan REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/icengine-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/icengine-config.cmake"
        DESTINATION share/icengine)
