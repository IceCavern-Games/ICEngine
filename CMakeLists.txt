cmake_minimum_required(VERSION 3.21)
project(IceCavernEngine)

# Use C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Library
add_library(${PROJECT_NAME} OBJECT
    src/ic_app.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Platform Variables
option(IC_RENDERER_VULKAN "Make Vulkan Renderer available" OFF)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")

# Tracks which libraries we need to link, depending on the options above.
set(LIBS, glfw ${GLFW_LIBRARIES} glm::glm)

if (IC_RENDERER_VULKAN)
    message("Vulkan Renderer Enabled")
    find_package(Vulkan REQUIRED)

    add_compile_definitions(IC_RENDERER_VULKAN)
    set(LIBS ${LIBS} Vulkan::Vulkan)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
