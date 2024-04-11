#include <ic_app.h>
#include <ic_log.h>

#ifdef IC_RENDERER_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif

#include "vulkan/vulkan_renderer.h"
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

    IC::Renderer::RendererConfig rendererConfig{};
    IC::Renderer::Renderer *renderer;

    // Copy config over.
    _appConfig = *c;

    IC_CORE_INFO("Hello, {0}.", _appConfig.Name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(_appConfig.Width, _appConfig.Height, _appConfig.Name, nullptr, nullptr);

    rendererConfig.Window = window;
    rendererConfig.Width = _appConfig.Width;
    rendererConfig.Height = _appConfig.Height;

#ifdef IC_RENDERER_VULKAN
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    IC_CORE_INFO("{0} Vulkan extensions supported.", extensionCount);

    rendererConfig.RendererType = IC::Renderer::RendererType::Vulkan;
#endif

    _appIsRunning = true;

    renderer = new IC::Renderer::VulkanRenderer(rendererConfig);

    Mesh mesh;
    Material material{};

    mesh.LoadFromFile("resources/models/cube.obj");
    material.FragShaderData = "resources/shaders/default_shader.frag.spv";
    material.VertShaderData = "resources/shaders/default_shader.vert.spv";
    material.Constants.Color = {1.0f, 0.0f, 0.0f, 1.0f};

    renderer->AddMesh(mesh, material);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer->DrawFrame();
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
