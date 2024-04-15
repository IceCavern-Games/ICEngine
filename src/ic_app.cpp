#include <ic_app.h>

#include <ic_log.h>

#include "ic_renderer.h"

#include <imgui.h>

#include <functional>
#include <iostream>

using namespace IC;

namespace {
    // Global App State
    Config appConfig;
    bool appIsRunning = false;
    bool appIsExiting = false;
    Renderer *appRendererApi;
} // namespace

bool App::Run(const Config *c) {
    Log::Init();

    // Copy config over.
    appConfig = *c;

    IC_CORE_INFO("Hello, {0}.", appConfig.name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(appConfig.width, appConfig.height, appConfig.name, nullptr, nullptr);

    RendererConfig rendererConfig{};
    rendererConfig.rendererType = appConfig.rendererType;
    rendererConfig.window = window;
    rendererConfig.width = appConfig.width;
    rendererConfig.height = appConfig.height;

    appIsRunning = true;

    appRendererApi = Renderer::MakeRenderer(rendererConfig);

    if (appRendererApi == nullptr) {
        IC_CORE_ERROR("Render module was not found.");

        glfwDestroyWindow(window);
        glfwTerminate();

        return false;
    }

    Mesh mesh;
    Material material{};

    mesh.LoadFromFile("resources/models/cube.obj");
    mesh.pos = glm::vec3(0.0f);
    mesh.rotation = glm::vec3(0.0f);
    mesh.scale = glm::vec3(0.5f);
    material.fragShaderData = "resources/shaders/default_unlit_shader.frag.spv";
    material.vertShaderData = "resources/shaders/default_unlit_shader.vert.spv";
    material.constants.color = {1.0f, 0.0f, 0.0f, 1.0f};

    // test light
    PointLight light{};
    Mesh lightMesh;
    Material lightMaterial{};

    lightMesh.pos = glm::vec3(1.0f);
    lightMesh.rotation = glm::vec3(0.0f);
    lightMesh.scale = glm::vec3(0.2f);
    light.color = {1.0f, 1.0f, 1.0f};

    lightMesh.LoadFromFile("resources/models/sphere.obj");
    lightMaterial.fragShaderData = "resources/shaders/default_unlit_shader.frag.spv";
    lightMaterial.vertShaderData = "resources/shaders/default_unlit_shader.vert.spv";
    lightMaterial.constants.color = {light.color.r, light.color.g, light.color.b, 1.0f};

    light.lightPreviewMesh = lightMesh;
    light.lightPreviewMaterial = lightMaterial;

    appRendererApi->AddMesh(mesh, material);
    appRendererApi->AddImguiFunction(std::bind(&PointLight::ParameterGui, &light));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        appRendererApi->DrawFrame();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool App::IsRunning() {
    return appIsRunning;
}

void App::Exit() {
    if (!appIsExiting && appIsRunning)
        appIsExiting = true;
}

const Config &App::GetConfig() {
    return appConfig;
}
