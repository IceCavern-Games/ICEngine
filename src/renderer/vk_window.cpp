#include "vk_window.hpp"

namespace render {
VkWindow::VkWindow(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
    initWindow();
}

VkWindow::~VkWindow() {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void VkWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    pWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void VkWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    VK_CHECK(glfwCreateWindowSurface(instance, pWindow, nullptr, surface));
}
} // namespace render