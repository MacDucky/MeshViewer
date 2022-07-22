#pragma once

#include <vector>
#include <map>
#include <memory>
#include <iostream>

#include "Camera.h"
#include "MeshModel.h"
#include "Lighting.h"
#include "Input.h"

#define ASSERT(x);  if(!(x)){ \
                     std::cout << "Assertion: '" << #x << "' failed at line: " << __LINE__ <<std::endl; \
                     exit(-1);\
                    }

class Scene {
public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;
	void PrintModelVerticesAndFaces(int idx) const;


	void AddCamera(const std::shared_ptr<Camera>& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;

    void SetModelInputFromGui(MInput &mInput, int active_model, bool toggle_vn, bool toggle_fn, bool toggle_bb);
    void SetCameraInputFromGui(CameraBars &cameraBars);

    void SetActiveCameraFromGui(const CameraBars& cameraBars);

    bool isVertexNormalToggle() const;
    bool isFaceNormalToggle() const;
    bool isBoundingBoxToggle() const;

    float GetNormalVLen() const;
    float GetNormalFLen() const;

	bool isDrawDepthToggle() const;
	void setDrawDepthToggle(bool drawDepthToggle);

	void SetModelColors(int model_idx, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular,
                        float shininess) const;

    ShadeMode& getShadeMode();

    const MeshModel& GetLightModel() const;
    Lights lights_;

    int& getToonColors();

private:
    std::vector<std::shared_ptr<MeshModel>> mesh_models_;
    std::vector<std::shared_ptr<Camera>> cameras_;
    //have a container that will contain the information that is received from DrawImguiMenus in the main.
    //this information will be passed down to the renderer later(because the scene is passed down to the renderer::render method.
    //MR,MS,MT - model rotate, scale and translate
    //WR,WS,WT - same for world
    std::map<std::string,glm::vec3> model_input_from_GUI;
    bool vertex_normal_toggle_; // if this is true, need to render the normals.
    bool face_normal_toggle_; // if this is true, need to render the normals.
    bool bounding_box_toggle_; // if this is true,need to render the bounding box.
    bool draw_depth_toggle_;
    float vnormal_len_;
    float fnormal_len_;

    int num_toon_color;

    ShadeMode shadeMode;

    int active_camera_index_;
    int active_model_index_;

    std::unique_ptr<MeshModel> light_model;

    void UpdateModelTransform(int idx) const;
    void UpdateWorldTransform(int idx) const;
};