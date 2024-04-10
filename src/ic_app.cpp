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
    Config app_config;
    bool app_is_running = false;
    bool app_is_exiting = false;
}

bool App::run(const Config *c)
{
    Log::Init();

    IC::Renderer::RendererConfig renderer_config{};
    IC::Renderer::ICRenderer *renderer;

    // Copy config over.
    app_config = *c;

    IC_CORE_INFO("Hello, {0}.", app_config.name);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(app_config.width, app_config.height, app_config.name, nullptr, nullptr);

    renderer_config.pWindow = window;
    renderer_config.width = app_config.width;
    renderer_config.height = app_config.height;

#ifdef IC_RENDERER_VULKAN
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    IC_CORE_INFO("{0} Vulkan extensions supported.", extensionCount);

    renderer_config.renderer_type = IC::Renderer::RendererType::Vulkan;
#endif

    app_is_running = true;

    renderer = new IC::Renderer::VulkanRenderer(renderer_config);

    ICMesh mesh;
    ICMaterial material{};

    mesh.load_from_file("resources/models/cube.obj");
    std::cout << "vert count: " << mesh.vertex_count << "\n";
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
