#include <ic_app.h>
#include <ic_log.h>

#ifdef IC_RENDERER_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif

#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

using namespace IC;

namespace
{
    // Global App State
    Config app_config;
    bool app_is_running = false;
    bool app_is_exiting = false;
}

bool App::run(const Config *c)
{
    Log::Init();
    IC_CORE_INFO("Hello, world.");

    // Copy config over.
    app_config = *c;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(app_config.width, app_config.height, app_config.name, nullptr, nullptr);

#ifdef IC_RENDERER_VULKAN
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    IC_CORE_INFO("{0} Vulkan extensions supported.", extensionCount;
#endif

    app_is_running = true;

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool App::is_running()
{
    return app_is_running;
}

void App::exit()
{
    if (!app_is_exiting && app_is_running)
        app_is_exiting = true;
}

const Config &App::config()
{
    return app_config;
}
