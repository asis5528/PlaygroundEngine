#include "Model.h"
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <stb_image.h>


Model::Model(const char* path, const Scene *enscene):engineScene(enscene){
    Assimp::Importer importer;
    scObjectSize = enscene->objects.size();
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    this->m_pScene = scene;
    objects;
    Object obj;
    //Node rootnode;
    aiMatrix4x4 m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
    aiVector3D trnas;
    aiVector3D rotate;
    aiVector3D sc; 
    m_GlobalInverseTransform.Decompose(sc, rotate,trnas);
    glm::mat4 dec = aiMatrix4x4ToGlm(&m_GlobalInverseTransform);
    glm::vec3 scale;
    glm::quat r;
    glm::vec3 trans;
    glm::vec3 skew;
    glm::vec4 pers;
    int upAxis = 0;
    int kt = scene->mNumTextures;
    if (scene->mMetaData)
    {
        int32_t UpAxis = 1, UpAxisSign = 1, FrontAxis = 2, FrontAxisSign = 1, CoordAxis = 0, CoordAxisSign = 1;
        double UnitScaleFactor = 1.0;
        for (unsigned MetadataIndex = 0; MetadataIndex < scene->mMetaData->mNumProperties; ++MetadataIndex)
        {
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxis") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, UpAxis);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxisSign") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, UpAxisSign);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxis") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, FrontAxis);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxisSign") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, FrontAxisSign);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxis") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, CoordAxis);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxisSign") == 0)
            {
                scene->mMetaData->Get<int32_t>(MetadataIndex, CoordAxisSign);
            }
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UnitScaleFactor") == 0)
            {
                scene->mMetaData->Get<double>(MetadataIndex, UnitScaleFactor);
            }
        }

        aiVector3D upVec, forwardVec, rightVec;

        upVec[UpAxis] = UpAxisSign * (float)UnitScaleFactor;
        forwardVec[FrontAxis] = FrontAxisSign * (float)UnitScaleFactor;
        rightVec[CoordAxis] = CoordAxisSign * (float)UnitScaleFactor;

        aiMatrix4x4 mat(rightVec.x, rightVec.y, rightVec.z, 0.0f,
            upVec.x, upVec.y, upVec.z, 0.0f,
            forwardVec.x, forwardVec.y, forwardVec.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
      //  scene->mRootNode->mTransformation = mat;
    }
  //  glm::mat4 r(1.0);
  //  r = glm::rotate(r, 3.1415f / 4.f, glm::vec3(1, 0, 0));
    aiMatrix4x4 rot;
    rot.Rotation(3.1415 / 2., aiVector3D(1, 0, 0), rot);

    int k = scene->mNumTextures;


    for (int i = 0; i < scene->mNumTextures; i++) {

        const aiTexture* tex = scene->mTextures[i];
        Texture texture = loadTexture(tex);
        textures.push_back(texture);

    }

    for (int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial *mat =  scene->mMaterials[i];
        
       

        ai_int illumModel;
        mat->Get(AI_MATKEY_SHADING_MODEL, illumModel);
        aiShadingMode shadingMode = (aiShadingMode)illumModel;



        aiColor4D color(0.f, 0.f, 0.f, 0.0f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);


        Material material;
            switch (shadingMode){

                case aiShadingMode_Phong:
                    if (mat->GetTextureCount(aiTextureType_DIFFUSE)>0) {
                       // Material material;
                        material.type = TexturedMaterial;
                        MaterialTexturedData *matData = new MaterialTexturedData();
                        matData->DiffuseColor = glm::vec4(color.r,color.g,color.b,color.a);
                        aiString texture_file;
                        mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
                        std::pair<const aiTexture*, int> texData = scene->GetEmbeddedTextureAndIndex(texture_file.C_Str());
                        matData->DiffuseTexture = (engineScene->ptextures.size()) + texData.second;
                        material.materialData = matData;
                    
                    }
                    else {
                       // Material material;
                        material.type = BasicMaterial;
                        MaterialBasicData* matData = new MaterialBasicData();
                        matData->DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                        material.materialData = matData;
                    }
                    break;
                case aiShadingMode_PBR_BRDF:
                    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                    
                        material.type = TexturedMaterial;
                        MaterialTexturedData* matData = new MaterialTexturedData();
                        matData->DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                        aiString texture_file;
                        mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
                        std::pair<const aiTexture*, int> texData = scene->GetEmbeddedTextureAndIndex(texture_file.C_Str());
                        matData->DiffuseTexture = (engineScene->ptextures.size()) + texData.second;
                        material.materialData = matData;

                    }
                    else {
                       // Material material;
                        material.type = BasicMaterial;
                        MaterialBasicData* matData = new MaterialBasicData();
                        matData->DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                        material.materialData = matData;
                    }
                    break;
                default:
                    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                        // Material material;
                        material.type = TexturedMaterial;
                        MaterialTexturedData* matData = new MaterialTexturedData();
                        matData->DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                        aiString texture_file;
                        mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
                        std::pair<const aiTexture*, int> texData = scene->GetEmbeddedTextureAndIndex(texture_file.C_Str());
                        matData->DiffuseTexture = (engineScene->ptextures.size()) + texData.second;
                        material.materialData = matData;

                    }
                    else {
                        // Material material;
                        material.type = BasicMaterial;
                        MaterialBasicData* matData = new MaterialBasicData();
                        matData->DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                        material.materialData = matData;
                    }
            }
            materials.push_back(material);
    }
    


    scene->mRootNode->mTransformation = scene->mRootNode->mTransformation*rot;
  //  glm::decompose(dec, scale, r, trans,skew,pers);
    processNode(scene->mRootNode, scene, this->rootNode);
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
   
}

void Model::processNode(aiNode* node, const aiScene* scene,Node & nodechild)
{
    // process each mesh located at the current node
   // object.meshID.resize(node->mNumMeshes);

    Object childObj;
    Node childNode;
    if (node->mNumMeshes > 1) {
        float k = 1.;
    }
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      
       
        Mesh meshbuffer;
        meshbuffer.name = mesh->mName.C_Str();
       // meshbuffer.color = glm::vec4(color.r,color.g,color.b,color.a);
        meshbuffer.matID =engineScene->materials.size()+ mesh->mMaterialIndex;
        processMesh(mesh, scene, meshbuffer);
        meshes.push_back(meshbuffer);
        childObj.meshID.push_back(meshes.size()-1);
        childObj.hasMesh = true;
    }
    const aiMatrix4x4 mat = node->mTransformation; 
    glm::mat4 transformation = aiMatrix4x4ToGlm(&mat);
   //transformation = glm::rotate(transformation, 3.1315f / 2.f, glm::vec3(0, -1, 1));
  
    childObj.ModelMatrix = transformation;
    childObj.name = node->mName.C_Str();
    //object.child.push_back(childObj);
    objects.push_back(childObj);
    childNode.ObjectID = (objects.size() - 1)+(scObjectSize);
    nodechild.children.push_back(childNode);
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        if (std::string(node->mChildren[i]->mName.C_Str()) != std::string("defaultobject")) {
            processNode(node->mChildren[i], scene, nodechild.children[nodechild.children.size()-1]);
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
Texture Model::loadTexture(const aiTexture* tex)
{
    if (tex) {
        int h = -1;
        h = tex->mHeight;
        if (h == 0) {
            int width;
            int height;
            int comp;
            stbi_uc* pixels = stbi_load_from_memory((unsigned char*)tex->pcData, tex->mWidth, &width, &height, &comp, 4);
            Texture texture;
            texture.width = width;
            texture.height = height;
            texture.data = pixels;
            texture.name = tex->mFilename.C_Str();
            return texture;
            //textures.push_back(texture);
           // meshbuffer.texID = (engineScene->textures.size() - 1) + (textures.size());
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
