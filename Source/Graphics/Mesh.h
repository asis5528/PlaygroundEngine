#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }

};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    void draw(VkCommandBuffer commandBuffer);
};


struct Texture {
    uint32_t mipLevels;
    uint32_t width;
    uint32_t height;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler imageSampler;
    void destroy();
};



struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
struct ubo1 {
    float time1;
    float time2;
    float time3;
};

