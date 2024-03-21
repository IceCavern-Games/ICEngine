#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <vulkan/vk_enum_string_helper.h>

#include <stdexcept>
#include <vector>

#define VK_CHECK(x)                                                                    \
    do {                                                                               \
        VkResult err = x;                                                              \
        if (err) {                                                                     \
            throw std::runtime_error(string_VkResult(err));                            \
            abort();                                                                   \
        }                                                                              \
    } while (0)

struct Pipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    std::vector<VkShaderModule> shaderModules;
    VkDescriptorSetLayout descriptorSetLayout;
    bool transparent = false;

    bool operator<(const Pipeline &other) const {
        return pipeline < other.pipeline && transparent <= other.transparent;
    }
};