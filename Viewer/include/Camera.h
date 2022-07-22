#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "MeshModel.h"
#include "Utils.h"
#include <iostream>

enum CameraProjection{ORTHOGRAPHIC,PERSPECTIVE};
//TODO: if I want to implement free camera translation and rotation, it should be applied after camera lookat.
class Camera {
public:
    Camera() = default;
    Camera(const glm::vec3 &pos, const glm::vec3 &at, const glm::vec3 &up);
    virtual ~Camera() = default;
    void SetCameraLookAt(const glm::vec3 &eye, const glm::vec3 &at, const glm::vec3 &up);
    void SetOrthoProjectionTransform(float left, float right, float bottom, float top, float near = 0.1f, float far = 500.0f);
    void SetPerspectProjectionTransform(float fovY,float width,float height,float near=0.1f,float far=500.0f);

    const glm::mat4x4 &GetViewTransformation() const;
    const glm::mat4& GetCameraLoadTransform() const;
    const glm::mat4x4 &GetOrthographicProjection() const;
    const glm::mat4x4 &GetPerspectiveProjection() const;
//    const MeshModel& GetCameraMesh() const;
    CameraProjection GetProjectionType() const;
    const glm::vec3& GetCameraPosition() const;

private:
//    std::shared_ptr<MeshModel> camera_mesh;
    glm::mat4x4 ortho_projection_transform_;
    glm::mat4x4 perspect_projection_transform_;
    glm::mat4x4 view_transformation_;
    glm::mat4 camera_transformation_;
    glm::vec3 camera_position_;
    CameraProjection mode;
};
