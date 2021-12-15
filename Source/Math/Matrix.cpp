// Code copied almost directly from https://github.com/Hopson97/HopsonCraft/blob/master/Source/Maths/Matrix.cpp
// As of Nov. 1 I still can't write my own code in C++ yet so I'm coping and learning

#include "Matrix.h"
#include "../External/glm/gtc/matrix_transform.hpp"
#include "../External/glm/ext/matrix_projection.hpp"
#include "../External/glm/matrix.hpp"
#include "../Engine/Renderer.h"

glm::mat4x4 CreateViewMatrix(const Camera& camera) {
    glm::dmat4x4 matrix = glm::identity<glm::dmat4x4>();

    matrix = glm::rotate(matrix, (double)glm::radians(camera.rotation.x), {1, 0, 0});
    matrix = glm::rotate(matrix, (double)glm::radians(camera.rotation.y), {0, 1, 0});
    matrix = glm::rotate(matrix, (double)glm::radians(camera.rotation.z), {0, 0, 1});
    matrix = glm::translate(matrix, -camera.position);

    return matrix;
}

glm::mat4x4 CreateProjMatrix(CameraParams params, glm::ivec2 displaySize) {
    return glm::perspective(
        glm::radians(params.fov),
        (float)displaySize.x / (float)displaySize.y,
        params.nearClipPlane,
        params.farClipPlane);
}