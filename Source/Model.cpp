#include "Model.h"
#include <iostream>


Model::Model(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    this->m_pScene = scene;
    
    processNode(scene->mRootNode, scene);
  //  animation = Animation(animation);
    if (scene->HasAnimations()) {
        AnimationData animData;
        float duration = this->m_pScene->mAnimations[0]->mDuration;
        
        readAnimation(scene->mRootNode, &animData);
       // this->animation = new Animation(animData, this->m_pScene->mAnimations[0]->mDuration);
        Animation animation;
        animation.animationData = animData;
        animation.duration = this->m_pScene->mAnimations[0]->mDuration;
        animation.tps = this->m_pScene->mAnimations[0]->mTicksPerSecond;
        aiMatrix4x4 m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        
        animation.m_GlobalInverseTransform = aiMatrix4x4ToGlm(&m_GlobalInverseTransform);
    
       

        animations.push_back(animation);
     
    }
    float k = 1.;
}

void Model::processNode(aiNode* node, const aiScene* scene)
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
        meshes.push_back(meshbuffer);
        const aiMatrix4x4 mat = node->mTransformation;
        glm::mat4 transformation = aiMatrix4x4ToGlm(&mat);
        transformations.push_back(transformation);
       
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        if (std::string(node->mChildren[i]->mName.C_Str()) != std::string("defaultobject")) {
            processNode(node->mChildren[i], scene);
        }
    }

}
void Model::processMesh(aiMesh* mesh, const aiScene* scene, Mesh& meshbuffer)
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

    std::vector< VertexBoneData> Bones;


    if (mesh->HasBones()) {
        meshbuffer.skinnedvertices.resize(meshbuffer.vertices.size());
        for (int i = 0; i < meshbuffer.vertices.size(); i++) {
            meshbuffer.skinnedvertices[i].pos = meshbuffer.vertices[i].pos;
            meshbuffer.skinnedvertices[i].texCoord = meshbuffer.vertices[i].texCoord;
            meshbuffer.skinnedvertices[i].normal = meshbuffer.vertices[i].normal;
       }
        Bones.resize(mesh->mNumVertices);
        
    }
    for (int i = 0; i < mesh->mNumBones; i++) {

        int BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);
        std::cout << BoneName << "\n";
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);

            m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(&mesh->mBones[i]->mOffsetMatrix);
            aiVector3D position;
            aiQuaternion rotation;
            aiVector3D scale;
            mesh->mBones[i]->mOffsetMatrix.Decompose(scale, rotation, position);
            m_BoneInfo[BoneIndex].name = BoneName;

            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }

        for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
            meshbuffer.skinnedvertices[VertexID].IDs[0] = Bones[VertexID].IDs[0];
            meshbuffer.skinnedvertices[VertexID].IDs[1] = Bones[VertexID].IDs[1]; 
            meshbuffer.skinnedvertices[VertexID].IDs[2] = Bones[VertexID].IDs[2]; 
            meshbuffer.skinnedvertices[VertexID].IDs[3] = Bones[VertexID].IDs[3]; 
            meshbuffer.skinnedvertices[VertexID].Weights[0] = Bones[VertexID].Weights[0];
            meshbuffer.skinnedvertices[VertexID].Weights[1] = Bones[VertexID].Weights[1];
            meshbuffer.skinnedvertices[VertexID].Weights[2] = Bones[VertexID].Weights[2];
            meshbuffer.skinnedvertices[VertexID].Weights[3] = Bones[VertexID].Weights[3];
           // meshbuffer.skinnedvertices[j].Weights = Bones[VertexID].Weights;
            //meshbuffer.skinnedvertices[VertexID].IDs = Bones[VertexID].IDs;
        }
    }
    meshbuffer.bones = Bones;
    meshbuffer.boneInfo = m_BoneInfo;
    // return meshbuffer;

}
inline glm::mat4 Model::aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
    glm::mat4 to;

    to[0][0] = (float)from->a1; to[0][1] = (float)from->b1;  to[0][2] = (float)from->c1; to[0][3] = (float)from->d1;
    to[1][0] = (float)from->a2; to[1][1] = (float)from->b2;  to[1][2] = (float)from->c2; to[1][3] = (float)from->d2;
    to[2][0] = (float)from->a3; to[2][1] = (float)from->b3;  to[2][2] = (float)from->c3; to[2][3] = (float)from->d3;
    to[3][0] = (float)from->a4; to[3][1] = (float)from->b4;  to[3][2] = (float)from->c4; to[3][3] = (float)from->d4;

    return to;
}
void Model::readAnimation(const aiNode* pNode, AnimationData* aData) {
    std::string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    glm::mat4 RotationMatrix(1.0);
    unsigned int animationDataIndex = 1000;
    glm::mat4 globalTransform(1.0);
    if (pNodeAnim) {
    
        AnimationData data;
        for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys; i++) {
            AnimationTransformation trans;
            aiVector3D  po = pNodeAnim->mPositionKeys[i].mValue;
            aiVector3D  sc = pNodeAnim->mScalingKeys[i].mValue;
            aiQuaternion q = pNodeAnim->mRotationKeys[i].mValue;
            trans.position = glm::vec3(po.x, po.y, po.z);
            trans.scale = glm::vec3(sc.x, sc.y, sc.z);
            glm::quat qu;
            qu.w = q.w;
            qu.x = q.x;
            qu.y = q.y;
            qu.z = q.z;
            trans.rotation = qu;
            trans.time = pNodeAnim->mRotationKeys[i].mTime;
            data.animationTransformation.push_back(trans);
            data.name = NodeName;
            // data.ticksperSec = pAnimation->mTicksPerSecond;
            // data.duration =  pAnimation->mDuration;


        }
        aData->childAnimationData.push_back(data);
    
    }
    for (int i = 0; i < pNode->mNumChildren; i++) {
        std::cout << "parent : " << pNode->mName.data << " --- child : " << pNode->mChildren[i]->mName.data << "\n";

        if (pNodeAnim) {
            readAnimation(pNode->mChildren[i], &aData->childAnimationData[aData->childAnimationData.size() - 1]);

        }
        else {
            readAnimation(pNode->mChildren[i], aData);
        }




    }
 


}
const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
        else {
            float k = 1.;
        }
    }
    return NULL;
}
