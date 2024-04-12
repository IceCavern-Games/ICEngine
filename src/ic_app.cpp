#include <ic_app.h>

#include <ic_log.h>

#include "ic_renderer.h"

#include <iostream>

using namespace IC;

namespace {
    // Global App State
    Config _appConfig;
    bool _appIsRunning = false;
    bool _appIsExiting = false;
    Renderer *_appRendererApi;
} // namespace

bool App::Run(const Config *c) {
    Log::Init();

    // Copy config over.
    _appConfig = *c;

    IC_CORE_INFO("Hello, {0}.", _appConfig.Name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window =
        glfwCreateWindow(_appConfig.Width, _appConfig.Height, _appConfig.Name, nullptr, nullptr);

    RendererConfig rendererConfig{};
    rendererConfig.RendererType = _appConfig.RendererType;
    rendererConfig.Window = window;
    rendererConfig.Width = _appConfig.Width;
    rendererConfig.Height = _appConfig.Height;

    _appIsRunning = true;

    _appRendererApi = Renderer::MakeRenderer(rendererConfig);

    if (_appRendererApi == nullptr) {
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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        _appRendererApi->DrawFrame();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool App::IsRunning() { return _appIsRunning; }

void App::Exit() {
    if (!_appIsExiting && _appIsRunning)
        _appIsExiting = true;
}

const Config &App::GetConfig() { return _appConfig; }
