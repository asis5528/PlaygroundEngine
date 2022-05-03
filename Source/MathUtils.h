#pragma once
/*
#include <assimp/matrix4x4.h>
*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>*/

namespace utils {

    inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
}
