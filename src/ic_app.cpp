#include <ic_app.h>

#include "ic_renderer.h"
#include <ic_log.h>

#include <GLFW/glfw3.h>
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
    LitMaterial meshMaterial{};

    mesh.LoadFromFile("resources/models/sphere.obj");
    mesh.pos = glm::vec3(0.0f);
    mesh.rotation = glm::vec3(0.0f);
    mesh.scale = glm::vec3(0.5f);
    meshMaterial.constants.color = {0.8f, 0.8f, 0.8f, 1.0f};
    meshMaterial.diffuseTexturePath = "resources/textures/Gravel026_1K_Color.png";

    // test light
    std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
    Mesh lightMesh;
    UnlitMaterial lightMaterial{};

    lightMesh.pos = glm::vec3(1.7f, 1.0f, 1.0f);
    lightMesh.rotation = glm::vec3(0.0f);
    lightMesh.scale = glm::vec3(0.1f);
    light->color = {1.0f, 1.0f, 1.0f};
    light->ambient = glm::vec3(0.1f);
    light->specular = glm::vec3(1.0f);

    lightMesh.LoadFromFile("resources/models/sphere.obj");
    lightMaterial.constants.color = {light->color.r, light->color.g, light->color.b, 1.0f};

    light->previewMesh = lightMesh;
    light->previewMaterial = &lightMaterial;

    // directional light
    std::shared_ptr<DirectionalLight> dirLight = std::make_shared<DirectionalLight>();
    dirLight->direction = {0.0f, 0.0f, 1.0f};
    dirLight->color = glm::vec3(1.0f);
    dirLight->ambient = glm::vec3(0.2f);
    dirLight->specular = glm::vec3(1.0f);

    appRendererApi->AddLight(light);
    // appRendererApi->AddLight(light2);
    appRendererApi->AddDirectionalLight(dirLight);
    appRendererApi->AddMesh(mesh, &meshMaterial);
    appRendererApi->AddImguiFunction("point light", std::bind(&PointLight::ParameterGui, light.get()));
    appRendererApi->AddImguiFunction("directional light", std::bind(&DirectionalLight::ParameterGui, dirLight.get()));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        appRendererApi->DrawFrame();
    }

    delete appRendererApi;
    appRendererApi = nullptr;
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
