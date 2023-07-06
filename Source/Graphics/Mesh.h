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
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 normal;
    alignas(16) glm::vec2 texCoord;

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



struct Texture {
    const char* name;
    uint32_t width;
    uint32_t height;
    unsigned char* data;

};



#endif