#include "Camera.h"

Camera::Camera(const glm::vec3 &pos, const glm::vec3 &at, const glm::vec3 &up) : ortho_projection_transform_(1.0f),
                                                                                 perspect_projection_transform_(1.0f) {
    SetCameraLookAt(pos, at, up);
//    camera_mesh = Utils::LoadMeshModel("C:\\Users\\Dany\\CLionProjects\\computergraphics2021-d-v\\Data\\camera.obj");
}


const glm::mat4x4 &Camera::GetViewTransformation() const {
    return view_transformation_;
}

const glm::mat4& Camera::GetCameraLoadTransform() const {
    return camera_transformation_;
}

const glm::mat4x4 &Camera::GetOrthographicProjection() const {
    return ortho_projection_transform_;
}

const glm::mat4x4 &Camera::GetPerspectiveProjection() const {
    return perspect_projection_transform_;
}

void Camera::SetCameraLookAt(const glm::vec3 &eye, const glm::vec3 &at, const glm::vec3 &up) {
    camera_position_ = eye;
    view_transformation_ = glm::lookAt(eye, at, up);
    camera_transformation_ = glm::inverse(camera_transformation_);
}

void Camera::SetOrthoProjectionTransform(float left,float right , float bottom,float top,float z_near,float z_far) {
    ortho_projection_transform_ = glm::ortho(left, right, bottom, top, z_near, z_far);
    mode = ORTHOGRAPHIC;
}

void Camera::SetPerspectProjectionTransform(float fovY,float width,float height,float z_near,float z_far) {
    perspect_projection_transform_ = glm::perspectiveFov(fovY, width, height, z_near, z_far);
    mode = PERSPECTIVE;
}

/*const MeshModel &Camera::GetCameraMesh() const{
    return *camera_mesh;
}*/

CameraProjection Camera::GetProjectionType() const {
    return mode;
}

const glm::vec3 &Camera::GetCameraPosition() const {
    return camera_position_;
}

