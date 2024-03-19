#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <stdexcept>

#define VK_CHECK(x)                                                                    \
    do {                                                                               \
        VkResult err = x;                                                              \
        if (err) {                                                                     \
            throw std::runtime_error("Detected Vulkan error: " + err);                 \
            abort();                                                                   \
        }                                                                              \
    } while (0)
