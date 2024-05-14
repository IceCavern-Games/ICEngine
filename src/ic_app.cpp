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
    MaterialTemplate meshMaterial{};
    meshMaterial.AddBinding(0, "color", BindingType::Uniform, ShaderDataType::Vec4);
    meshMaterial.AddBinding(1, "diffuse", BindingType::Texture, ShaderDataType::String);
    meshMaterial.AddBinding(2, "specular", BindingType::Texture, ShaderDataType::String);
    meshMaterial.vertShaderData = "resources/shaders/default_lit_shader.vert.spv";
    meshMaterial.fragShaderData = "resources/shaders/default_lit_shader.frag.spv";
    meshMaterial.flags = MaterialFlags::Lit;

    MaterialTemplate unlitMaterial{};
    unlitMaterial.AddBinding(0, "color", BindingType::Uniform, ShaderDataType::Vec4);
    unlitMaterial.vertShaderData = "resources/shaders/default_unlit_shader.vert.spv";
    unlitMaterial.fragShaderData = "resources/shaders/default_unlit_shader.frag.spv";
    unlitMaterial.flags = MaterialFlags::None;

    glm::vec4 color = {0.8f, 0.8f, 0.8f, 1.0f};
    std::string diffusePath = "resources/textures/backpack_diffuse.jpg";
    std::string specularPath = "resources/textures/backpack_specular.jpg";

    MaterialInstance meshMaterialInstance{meshMaterial};
    meshMaterialInstance.SetBindingValue(0, &color, sizeof(glm::vec4));
    meshMaterialInstance.SetBindingValue(1, &diffusePath, sizeof(diffusePath));
    meshMaterialInstance.SetBindingValue(2, &specularPath, sizeof(specularPath));

    MaterialInstance unlitMaterialInstance{unlitMaterial};
    unlitMaterialInstance.SetBindingValue(0, &color, sizeof(glm::vec4));

    mesh.LoadFromFile("resources/models/backpack.obj");
    mesh.pos = glm::vec3(0.0f);
    mesh.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    mesh.scale = glm::vec3(0.5f);

    // test light
    std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
    light->color = {1.0f, 1.0f, 1.0f};
    light->ambient = glm::vec3(0.1f);
    light->specular = glm::vec3(1.0f);

    Mesh lightMesh;
    lightMesh.pos = glm::vec3(1.7f, 1.0f, 1.0f);
    lightMesh.rotation = glm::vec3(0.0f);
    lightMesh.scale = glm::vec3(0.1f);
    lightMesh.LoadFromFile("resources/models/sphere.obj");

    light->previewMesh = lightMesh;
    light->previewMaterial = &unlitMaterialInstance;

    // directional light
    std::shared_ptr<DirectionalLight> dirLight = std::make_shared<DirectionalLight>();
    dirLight->direction = {0.0f, 0.0f, 1.0f};
    dirLight->color = glm::vec3(1.0f);
    dirLight->ambient = glm::vec3(0.2f);
    dirLight->specular = glm::vec3(1.0f);

    appRendererApi->AddLight(light);
    appRendererApi->AddDirectionalLight(dirLight);
    appRendererApi->AddMesh(mesh, &meshMaterialInstance);
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
