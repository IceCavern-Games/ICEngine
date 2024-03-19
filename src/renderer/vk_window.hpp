#pragma once

#include "vk_types.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace render {
class VkWindow {
private:
    int width, height;
    std::string windowName;
    GLFWwindow *pWindow;

    void initWindow();

public:
    VkWindow(int w, int h, std::string name);
    ~VkWindow();

    VkWindow(const VkWindow &) = delete;
    VkWindow &operator=(const VkWindow &) = delete;

    bool shouldClose() { return glfwWindowShouldClose(pWindow); }
    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }
    GLFWwindow *getWindow() { return pWindow; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
};
} // namespace render