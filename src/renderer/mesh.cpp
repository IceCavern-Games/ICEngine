#include "mesh.hpp"

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace render {
Mesh::Mesh(ICDevice &device) : icDevice{device} {
    vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, -0.5f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {-0.5f, 0.5f}}};
    vertexCount = 3;
    indices = {0, 1, 2};
    indexCount = 3;
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
}

Mesh::~Mesh() {
    vkDestroyBuffer(icDevice.device(), vertexBuffer, nullptr);
    vkFreeMemory(icDevice.device(), vertexBufferMemory, nullptr);
    vkDestroyBuffer(icDevice.device(), indexBuffer, nullptr);
    vkFreeMemory(icDevice.device(), indexBufferMemory, nullptr);

    for (size_t i = 0; i < ICSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(icDevice.device(), uniformBuffers[i], nullptr);
        vkFreeMemory(icDevice.device(), uniformBuffersMemory[i], nullptr);
    }
}

void Mesh::bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout, size_t currentFrame) {
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::draw(VkCommandBuffer cmdBuffer) { vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0); }

void Mesh::updateUniformBuffer(UniformBufferObject uniformBuffer, uint32_t currentImage) {
    memcpy(uniformBuffersMapped[currentImage], &uniformBuffer, sizeof(uniformBuffer));
}

void Mesh::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    icDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer, stagingBufferMemory);
    void *data;
    vkMapMemory(icDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(icDevice.device(), stagingBufferMemory);

    icDevice.createBuffer(bufferSize,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    icDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(icDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(icDevice.device(), stagingBufferMemory, nullptr);
}

void Mesh::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    icDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer, stagingBufferMemory);
    void *data;
    vkMapMemory(icDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(icDevice.device(), stagingBufferMemory);

    icDevice.createBuffer(bufferSize,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    icDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(icDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(icDevice.device(), stagingBufferMemory, nullptr);
}

void Mesh::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(ICSwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(ICSwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(ICSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < ICSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        icDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(icDevice.device(), uniformBuffersMemory[i], 0, bufferSize, 0,
                    &uniformBuffersMapped[i]);
    }
}
} // namespace render