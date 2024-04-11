#include <ic_app.h>
#include <ic_log.h>

#ifdef IC_RENDERER_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif

#include <vulkan_renderer.h>
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
    Config _appConfig;
    bool _appIsRunning = false;
    bool _appIsExiting = false;
}

bool App::Run(const Config *c)
{
    Log::Init();

    IC::Renderer::RendererConfig renderer_config{};
    IC::Renderer::ICRenderer *renderer;

    // Copy config over.
    _appConfig = *c;

    IC_CORE_INFO("Hello, {0}.", _appConfig.Name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(_appConfig.Width, _appConfig.Height, _appConfig.Name, nullptr, nullptr);

    renderer_config.pWindow = window;
    renderer_config.width = _appConfig.Width;
    renderer_config.height = _appConfig.Height;

#ifdef IC_RENDERER_VULKAN
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    IC_CORE_INFO("{0} Vulkan extensions supported.", extensionCount);

    renderer_config.renderer_type = IC::Renderer::RendererType::Vulkan;
#endif

    _appIsRunning = true;

    renderer = new IC::Renderer::VulkanRenderer(renderer_config);

    ICMesh mesh;
    ICMaterial material{};

    mesh.load_from_file("resources/models/cube.obj");
    material.fragShaderData = "resources/shaders/default_shader.frag.spv";
    material.vertShaderData = "resources/shaders/default_shader.vert.spv";
    material.constants.color = {1.0f, 0.0f, 0.0f, 1.0f};

    renderer->add_mesh(mesh, material);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer->draw_frame();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool App::IsRunning()
{
    return _appIsRunning;
}

void App::Exit()
{
    if (!_appIsExiting && _appIsRunning)
        _appIsExiting = true;
}

const Config &App::GetConfig()
{
    return _appConfig;
}
