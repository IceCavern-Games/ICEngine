cmake_minimum_required(VERSION 3.21)
project(IceCavernGameEngine)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DMACOS)
endif()

# Use C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# vcpkg packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")

# Executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm)

add_subdirectory("src")
add_subdirectory("src/shaders")