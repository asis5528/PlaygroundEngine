#pragma once
#ifndef MESH_H
#define MESH_H



#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
struct BoneInfo
{
    std::string name;
    glm::mat4 BoneOffset;
};
struct VertexBoneData
{
   unsigned int IDs[4] = { 0,0,0,0 };
    float Weights[4] = { 0.0,0.0,0.0,0.0 };

    VertexBoneData()
    {
        Reset();
    };

    void Reset()
    {
        //ZERO_MEM(IDs);
        //ZERO_MEM(Weights);
    }

    void AddBoneData(unsigned int BoneID, float Weight) {
        for (unsigned int i = 0; i < sizeof(IDs) / sizeof(IDs[0]); i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
               // std::cout << IDs[i] << "\n";
                Weights[i] = Weight;
                return;
            }
        }
    };
};
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }

};
struct SkinnedVertex {
   // Vertex vertex;
   // VertexBoneData bonedata;
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    unsigned int IDs[4] = { 0,0,0,0 };
    float Weights[4] = { 0.0,0.0,0.0,0.0 };

    static VkVertexInputBindingDescription getBindingDescription();

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();


};


struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<SkinnedVertex> skinnedvertices;
    std::vector<uint32_t> indices;
    std::vector<VertexBoneData> bones;
    std::vector< BoneInfo> boneInfo;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    glm::vec4 color;
    int texID = -1;
    int matID = 0;
    void draw(VkCommandBuffer commandBuffer);
};


struct VulkanTexture {
    uint32_t mipLevels;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler imageSampler;
    VkImageType imageType = VK_IMAGE_TYPE_2D;
    VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    void destroy();
};
struct VulkanTexture3D : VulkanTexture {
    
  //  VkImageType imageType = VK_IMAGE_TYPE_3D;
  //  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_3D;
};

struct Texture {
    const char* name;
    uint32_t width;
    uint32_t height;
    unsigned char* data;

};



#endif