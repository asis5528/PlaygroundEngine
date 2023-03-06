#include "Loader.h"
#include "Model.h"
#include <stb_image.h>
#include <glm/gtx/matrix_decompose.hpp>

Loader::Loader(VulkanBase* base, Scene* scene) {
	this->base = base;
	this->scene = scene;
}

void Loader::load(int path_count, const char* paths[])
{
	vkDeviceWaitIdle(base->device);
	for (int i = 0; i < path_count; i++) {
		const char* path = paths[i];
        Model modelData = Model(path, scene );
        int modelCount = 0;
        for (int i = 0; i < scene->objects.size(); i++) {
            if (scene->objects[i].hasMesh) {
                modelCount++;
            }
        }
        for (Node& node : modelData.rootNode.children) {
            scene->rootNode.children.push_back(node);
        }
        for (Animation& animation : modelData.animations) {
            this->scene->animations.push_back(animation);
        }

        for (Material& material : modelData.materials) {
            switch (material.type) {
            case BasicMaterial:
                material.pipeline = scene->pipelines[2];
                material.skinnedPipeline = scene->pipelines[3];
                break;
            case TexturedMaterial:
                material.pipeline = scene->pipelines[0];
                material.skinnedPipeline = scene->pipelines[1];
                break;
            }

            scene->materials.push_back(material);
        }

        for (Texture& rawtex : modelData.textures) {
            VulkanTexture texture;
            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
            texture.width = rawtex.width;
            texture.height = rawtex.height;
            texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture.width, texture.height)))) + 1;
            if (!rawtex.data) {
                throw std::runtime_error("failed to load texture image!");
            }
            base->vbuffer->createTexturefromBuffer(texture, rawtex.data);
            stbi_image_free(rawtex.data);
            scene->textures.push_back(texture);
        }
        
        for (Object &object:modelData.objects) {
            object.textureID = 0;
         //   object.ModelMatrix = glm::translate(object.ModelMatrix, glm::vec3(modelCount * 10.-10., -100, 0));
          //  glm::vec3 scale, position,skew;
         //   glm::quat rot;
         //   glm::vec4 pers;
          //  glm::decompose(object.ModelMatrix,scale,rot,position,skew,pers);

            if (object.hasMesh) {
                for (int i = 0; i < object.meshID.size(); i++) {
                    Mesh mesh = modelData.meshes[object.meshID[i]];
                    if (mesh.bones.size()) base->vbuffer->createSkinnedVertexBuffer(mesh);
                    else base->vbuffer->createVertexBuffer(mesh);
                    base->vbuffer->createIndexBuffer(mesh);
                    scene->meshes.push_back(mesh);
                    object.meshID[i] = scene->meshes.size() - 1;
                }

                if (modelData.animations.size() > 0) {
                    object.animationID = this->scene->animations.size() - 1;
                    object.hasAnimation = true;
                }
                else {
                    object.hasAnimation = false;
                }
                //std::vector<VulkanShader> shader;


                for (int i = 0; i < object.meshID.size(); i++) {
                    Mesh mesh = scene->meshes[object.meshID[i]];
                    VulkanShader shader;
                    if (mesh.bones.size()) {
                        shader.graphicsPipeline = scene->materials[mesh.matID].skinnedPipeline;
                    }
                    else {
                        shader.graphicsPipeline = scene->materials[mesh.matID].pipeline;

                    }
                    /*
                    if (mesh.texID >= 0)
                    {
                        if (mesh.bones.size()) {
                            shader.graphicsPipeline = this->scene->pipelines[1];
                        }
                        else {
                            shader.graphicsPipeline = this->scene->pipelines[0];
                        }
                    }
                    else {
                        if (mesh.bones.size()) {
                            shader.graphicsPipeline = this->scene->pipelines[3];
                        }
                        else {
                            shader.graphicsPipeline = this->scene->pipelines[0];
                        }
                    }*/
                    shader.ubos = base->vbuffer->createUniformBuffers(shader.graphicsPipeline->bufferSize);
                    object.shader.push_back(shader);
                }
              //Come back if any errors or issue
               // object.shader = shader;
               // object.shader.ubos = base->vbuffer->createUniformBuffers(object.shader.graphicsPipeline->bufferSize);
            }
            this->scene->objects.push_back(object);
        }
        /*
        for (int i = 0; i < modelData.meshes.size(); i++) {
            if (modelData.meshes[i].bones.size()) base->vbuffer->createSkinnedVertexBuffer(modelData.meshes[i]);
            else base->vbuffer->createVertexBuffer(modelData.meshes[i]);
            
            base->vbuffer->createIndexBuffer(modelData.meshes[i]);
           
            this->scene->meshes.push_back(modelData.meshes[i]);
            Object object;
            object.meshID.push_back(  this->scene->meshes.size() - 1);
            object.textureID = 0;
            object.ModelMatrix = modelData.transformations[i];
            object.ModelMatrix =  glm::translate(object.ModelMatrix, glm::vec3(scene->objects.size()*2.,0,0));
            if (modelData.animations.size()>0) {
                object.animationID = this->scene->animations.size() - 1;
                object.hasAnimation = true;
            }
            else {
                object.hasAnimation = false;
            }
            
            Shader shader;
            if (modelData.meshes[i].bones.size()) {
                shader.graphicsPipeline = this->scene->pipelines[1];
            }
            else {
                shader.graphicsPipeline = this->scene->pipelines[0];
            }
            
            object.shader = shader;
            object.shader.ubos = base->vbuffer->createUniformBuffers(object.shader.graphicsPipeline->bufferSize);

            this->scene->objects.push_back(object);
        }*/
	//	app->loadModel(path);
	}
}
/*
void processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        const char* stt = node->mName.C_Str();
        const char* strr = "defaultobject";
        const char* strk = "defaultobject";
        if (std::string(node->mName.C_Str()) == std::string(strr)) {
            float k = 0.0;
        }
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        Mesh meshbuffer;
        processMesh(mesh, scene, meshbuffer);
        base->vbuffer->createVertexBuffer(meshbuffer);
        base->vbuffer->createIndexBuffer(meshbuffer);

        this->scene->meshes.push_back(meshbuffer);
        Object object;
        object.meshID = this->scene->meshes.size() - 1;
        object.textureID = 0;
        const aiMatrix4x4 mat = node->mTransformation;
        object.ModelMatrix = aiMatrix4x4ToGlm(&mat);
        int sizee = (this->scene->objects.size()) + ((this->scene->objects.size() + 1) % 2);
        glm::mat4 transl = glm::translate(glm::mat4(1.0), glm::vec3(sizee / 3., 0.0, 0.0));
        object.ModelMatrix = transl * object.ModelMatrix;
        Shader shader;
        shader.graphicsPipeline = this->scene->pipelines[0];
        object.shader = shader;
        object.shader.ubos = base->vbuffer->createUniformBuffers(object.shader.graphicsPipeline->bufferSize);

        this->scene->objects.push_back(object);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        if (std::string(node->mChildren[i]->mName.C_Str()) != std::string("defaultobject")) {
            processNode(node->mChildren[i], scene);
        }
    }

}
void processMesh(aiMesh* mesh, const aiScene* scene, Mesh& meshbuffer)
{
    //data to fill
    //vector<Vertex> vertices;
    //vector<unsigned int> indices;
    //vector<Texture> textures;
    //Mesh meshbuffer;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.pos = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            // vertex.Tangent = vector;
             // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            // vertex.Bitangent = vector;
        }
        else
            vertex.texCoord = glm::vec2(0.0f, 0.0f);

        meshbuffer.vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            meshbuffer.indices.push_back(face.mIndices[j]);
    }
    // return meshbuffer;

}



void loadModel(const char* MODEL_PATH) {


    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(MODEL_PATH, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);



}*/