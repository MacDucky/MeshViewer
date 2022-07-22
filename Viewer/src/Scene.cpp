#include "Scene.h"
#include "MeshModel.h"
#include "Input.h"
#include <string>

Scene::Scene() :
        active_camera_index_(0),
        active_model_index_(0),
        face_normal_toggle_(false),
        vertex_normal_toggle_(false),
        bounding_box_toggle_(false),num_toon_color(2){
    light_model = Utils::LoadMeshModel();
}

void Scene::AddModel(const std::shared_ptr<MeshModel> &mesh_model) {
    mesh_models_.push_back(mesh_model);
}

int Scene::GetModelCount() const {
    return mesh_models_.size();
}

MeshModel &Scene::GetModel(int index) const {
    return *mesh_models_[index];
}

MeshModel &Scene::GetActiveModel() const {
    return *mesh_models_[active_model_index_];
}

void Scene::AddCamera(const std::shared_ptr<Camera> &camera) {
    cameras_.push_back(camera);
}

int Scene::GetCameraCount() const {
    return cameras_.size();
}

Camera &Scene::GetCamera(int index) {
    return *cameras_[index];
}

Camera &Scene::GetActiveCamera() {
    return *cameras_[active_camera_index_];
}

void Scene::SetActiveCameraIndex(int index) {
    active_camera_index_ = index;
}

int Scene::GetActiveCameraIndex() const {
    return active_camera_index_;
}

void Scene::SetActiveModelIndex(int index) {
    active_model_index_ = index;
}

int Scene::GetActiveModelIndex() const {
    return active_model_index_;
}

void Scene::PrintModelVerticesAndFaces(int idx) const {
    MeshModel model = GetModel(idx);
    std::cout << "Printing vertices:" << std::endl;
    model.PrintVertices();
    std::cout << std::endl << "Printing faces:" << std::endl;
    model.PrintFaces();
}

void Scene::UpdateModelTransform(int idx) const {
    MeshModel& model = GetModel(idx);
    auto iter = model_input_from_GUI.find("MT");
    if(iter != model_input_from_GUI.end()){
        auto iter2 = model_input_from_GUI.find("MR");
        if (iter != model_input_from_GUI.end()){
            auto iter3 = model_input_from_GUI.find("MS");
            if(iter3 != model_input_from_GUI.end()){
                model.UpdateMTranslate(iter->second);
                model.UpdateMRotate(iter2->second);
                model.UpdateMScale(iter3->second);
            }
        }
    }
}

void Scene::UpdateWorldTransform(int idx) const {
    MeshModel& model = GetModel(idx);
    auto iter = model_input_from_GUI.find("WT");
    if(iter != model_input_from_GUI.end()){
        auto iter2 = model_input_from_GUI.find("WR");
        if (iter != model_input_from_GUI.end()){
            auto iter3 = model_input_from_GUI.find("WS");
            if(iter3 != model_input_from_GUI.end()){
                model.UpdateWTranslate(iter->second);
                model.UpdateWRotate(iter2->second);
                model.UpdateWScale(iter3->second);
            }
        }
    }
}


void Scene::SetModelInputFromGui(MInput &mInput, int active_model, bool toggle_vn, bool toggle_fn, bool toggle_bb) {
    MeshModel& meshModel = *mesh_models_[active_model];
    meshModel.UpdateMTranslate(mInput.GetMT(active_model));
    meshModel.UpdateMRotate(mInput.GetMR(active_model));
    meshModel.UpdateMScale(mInput.GetMS(active_model));
    meshModel.UpdateWTranslate(mInput.GetWT(active_model));
    meshModel.UpdateWRotate(mInput.GetWR(active_model));
    meshModel.UpdateWScale(mInput.GetWS(active_model));
    active_model_index_ = active_model;
    vertex_normal_toggle_ = toggle_vn;
    face_normal_toggle_ = toggle_fn;
    vnormal_len_ = mInput.GetVNLen(active_model);
    fnormal_len_ = mInput.GetFNLen(active_model);
    bounding_box_toggle_ = toggle_bb;
}


bool Scene::isVertexNormalToggle() const {
    return vertex_normal_toggle_;
}

bool Scene::isFaceNormalToggle() const {
    return face_normal_toggle_;
}

bool Scene::isBoundingBoxToggle() const {
    return bounding_box_toggle_;
}


void Scene::SetCameraInputFromGui(CameraBars &cameraBars) {
    int active_cam = cameraBars.GetActiveCam();
    Camera& camera = *cameras_[active_cam];
    glm::vec3 pos,at,up;
    pos = cameraBars.GetPosition(active_cam);
    at = cameraBars.GetAt(active_cam);
    up = cameraBars.GetUp(active_cam);
    camera.SetCameraLookAt(pos,at,up);
    if(!cameraBars.GetCamMode(cameraBars.GetActiveCam())){
        const glm::vec4& ortho = cameraBars.GetOrthographic(active_cam);
        camera.SetOrthoProjectionTransform(ortho[0],ortho[1],ortho[2],ortho[3]);
    }
    else{ // perspective
        glm::vec3& perspective = cameraBars.GetPerspective(active_cam);
        float angle_in_rads = (perspective[0] / 180.f) * M_PI;
        camera.SetPerspectProjectionTransform(angle_in_rads,perspective[1],perspective[2]);
    }
}


float Scene::GetNormalVLen() const {
    return vnormal_len_;
}

float Scene::GetNormalFLen() const {
    return fnormal_len_;
}



bool Scene::isDrawDepthToggle() const {
    return draw_depth_toggle_;
}

void Scene::setDrawDepthToggle(bool drawDepthToggle) {
    draw_depth_toggle_ = drawDepthToggle;
}

void
Scene::SetModelColors(int model_idx, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular,
                      float shininess) const {
    MeshModel& meshModel = GetModel(model_idx);
    meshModel.setAmbientColor(ambient);
    meshModel.setDiffuseColor(diffuse);
    meshModel.setSpecularColor(specular);
    meshModel.setShininess(shininess);
}

ShadeMode& Scene::getShadeMode(){
    return shadeMode;
}

void Scene::SetActiveCameraFromGui(const CameraBars &cameraBars) {
    active_camera_index_ = cameraBars.GetActiveCam();
}

const MeshModel &Scene::GetLightModel() const {
    return *light_model;
}

int &Scene::getToonColors() {
    return num_toon_color;
}









