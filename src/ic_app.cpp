#include <ic_app.h>
#include <ic_log.h>
#include "ic_renderer.h"

#ifdef IC_RENDERER_VULKAN
#define GLFW_INCLUDE_VULKAN
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
    Config _appConfig;
    bool _appIsRunning = false;
    bool _appIsExiting = false;
    RendererConfig _rendererConfig{};
    Renderer *_appRendererApi;
}

bool App::Run(const Config *c)
{
    Log::Init();

    // Copy config over.
    _appConfig = *c;

    IC_CORE_INFO("Hello, {0}.", _appConfig.Name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(_appConfig.Width, _appConfig.Height, _appConfig.Name, nullptr, nullptr);

    _rendererConfig.RendererType = _appConfig.RendererType;
    _rendererConfig.Window = window;
    _rendererConfig.Width = _appConfig.Width;
    _rendererConfig.Height = _appConfig.Height;

    _appIsRunning = true;

    _appRendererApi = Renderer::MakeRenderer(_rendererConfig);

    if (_appRendererApi == nullptr)
    {
        IC_CORE_ERROR("Render module was not found.");

        glfwDestroyWindow(window);
        glfwTerminate();

        return false;
    }

    Mesh mesh;
    Material material{};

    mesh.LoadFromFile("resources/models/cube.obj");
    material.FragShaderData = "resources/shaders/default_shader.frag.spv";
    material.VertShaderData = "resources/shaders/default_shader.vert.spv";
    material.Constants.Color = {1.0f, 0.0f, 0.0f, 1.0f};

    _appRendererApi->AddMesh(mesh, material);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        _appRendererApi->DrawFrame();
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
