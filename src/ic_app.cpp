#include <ic_app.h>

#include "ic_renderer.h"
#include <ic_gameobject.h>
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
    // material setup
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

    std::shared_ptr<MaterialInstance> meshMaterialInstance = std::make_shared<MaterialInstance>(meshMaterial);
    meshMaterialInstance->SetBindingValue(0, &color, sizeof(glm::vec4));
    meshMaterialInstance->SetBindingValue(1, &diffusePath, sizeof(diffusePath));
    meshMaterialInstance->SetBindingValue(2, &specularPath, sizeof(specularPath));

    std::shared_ptr<MaterialInstance> unlitMaterialInstance = std::make_shared<MaterialInstance>(unlitMaterial);
    unlitMaterialInstance->SetBindingValue(0, &color, sizeof(glm::vec4));

    // mesh game object
    auto mesh = std::make_shared<GameObject>("test mesh");
    Mesh meshComponent{};
    mesh->GetTransform()->SetScale(glm::vec3(0.5f));
    meshComponent.SetMaterial(meshMaterialInstance);
    mesh->AddComponent<Mesh>(meshComponent);

    // test light
    auto pointLight = std::make_shared<GameObject>("point light");
    Mesh pointLightMesh{};
    PointLight pointLightComponent{};
    pointLight->GetTransform()->SetPosition(glm::vec3(1.7f, 1.0f, 1.0f));
    pointLight->GetTransform()->SetScale(glm::vec3(0.1f));
    pointLightMesh.SetMaterial(unlitMaterialInstance);
    pointLight->AddComponent<PointLight>(pointLightComponent);
    pointLight->AddComponent<Mesh>(pointLightMesh);

    // directional light
    auto dirLight = std::make_shared<GameObject>("directional light");
    DirectionalLight dirLightComponent{};
    dirLightComponent.SetDirection({0.0f, 0.0f, 1.0f});
    dirLightComponent.SetAmbient(glm::vec3(0.2f));
    dirLight->AddComponent<DirectionalLight>(dirLightComponent);

    appRendererApi->AddGameObject(mesh);
    appRendererApi->AddGameObject(pointLight);
    appRendererApi->AddGameObject(dirLight);
    appRendererApi->AddImguiFunction("game object", std::bind(&GameObject::Gui, mesh.get()));
    appRendererApi->AddImguiFunction("point light", std::bind(&GameObject::Gui, dirLight.get()));

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
