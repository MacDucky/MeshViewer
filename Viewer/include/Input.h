#ifndef INPUTFROMIMGUI_H
#define INPUTFROMIMGUI_H

#include <exception>
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include "glm/glm.hpp"
#include "Lighting.h"

namespace Input {
    typedef std::tuple<std::string,glm::vec3> Vec3;
    typedef std::array<float,3> F3Array;
    typedef std::array<float,4> F4Array;
    typedef std::array<F3Array ,6> F63Array;
    typedef std::array<F3Array ,3> F33Array;
    typedef F3Array::iterator F3ArrayIt;
    typedef F3Array::const_iterator F3ArrayCIt;
    typedef F4Array::iterator  F4ArrayIt;
    typedef F4Array::const_iterator  F4ArrayCIt;
    typedef std::vector<float>::iterator FVec;
    constexpr int NUM_OF_MODEL_TRANSFORMS = 6;
    constexpr float ORTHO_LEFT_INIT = -8;
    constexpr float ORTHO_RIGHT_INIT = 8;
    constexpr float ORTHO_BOT_INIT = -4.5;
    constexpr float ORTHO_TOP_INIT = 4.5;
    constexpr float PERSPECT_FOVY_INIT = 60;
    constexpr float PERSPECT_WIDTH_INIT = 1280;
    constexpr float PERSPECT_HEIGHT_INIT = 720;
}


enum ShadeMode{FLAT,GOURAUD,PHONG,TOON};

class MInput{
    std::vector<glm::vec3> mt;
    std::vector<glm::vec3> mr;
    std::vector<glm::vec3> ms;
    std::vector<glm::vec3> wt;
    std::vector<glm::vec3> wr;
    std::vector<glm::vec3> ws;
    std::vector<float> vn_len;
    std::vector<float> fn_len;
public:
    glm::vec3& GetMT(int active_model);
    glm::vec3& GetMR(int active_model);
    glm::vec3& GetMS(int active_model);
    glm::vec3& GetWT(int active_model);
    glm::vec3& GetWR(int active_model);
    glm::vec3& GetWS(int active_model);
    float& GetVNLen(int active_model);
    float& GetFNLen(int active_model);
    void AddBars();
};

class ModelInput {
    // MT ->MR ->MS ->WT ->WR ->WS
    Input::Vec3 transforms[Input::NUM_OF_MODEL_TRANSFORMS];
    int model_idx;
    float vnormal_len_;
    float fnormal_len_;
    bool toggleVN;
    bool toggleFN;
    bool toggleBB;

    // operator[] is used for internal purposes
    glm::vec3& operator[](int idx);
    const glm::vec3& operator[](int idx) const;

public:
    ModelInput(const float *const *info, int active_model,float vnormal_len,float fnormal_len, bool VN = false, bool FN = false , bool BB = false);

    // MT ->MR ->MS ->WT ->WR ->WS
    std::string GetTransformName(int idx) const;
    glm::vec3 GetTransformVec(int idx) const;
    int GetSelectedModel() const;

    float GetNormalVLen() const;
    float GetNormalFLen() const;

    bool isToggleVn() const;
    bool isToggleFn() const;
    bool isToggleBb() const;
};

class TransformModelBars{
    std::vector<Input::F63Array> _transforms;
    std::vector<float> v_normal_len;
    std::vector<float> f_normal_len;
public:
    void AddBar(); // each time a model is loaded,a new set of bars is formed.
    Input::F3ArrayIt GetMT(int active_model);
    Input::F3ArrayIt GetMR(int active_model);
    Input::F3ArrayIt GetMS(int active_model);
    Input::F3ArrayIt GetWT(int active_model);
    Input::F3ArrayIt GetWR(int active_model);
    Input::F3ArrayIt GetWS(int active_model);
    float* GetVNormalLen(int active_model);
    float* GetFNormalLen(int active_model);

};

class Outofbounds: public std::exception{
public:
    const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override;
};


class CameraBars{
    std::vector<glm::vec3> position_;
    std::vector<glm::vec3> at_;
    std::vector<glm::vec3> up_;
    std::vector<bool> camera_mode_;
    std::vector<glm::vec4> ortho_proj_;
    std::vector<glm::vec3> perspective_proj_;
    int active_cam_;
public:
    glm::vec3& GetPosition(int active_cam);
    glm::vec3& GetAt(int active_cam);
    glm::vec3& GetUp(int active_cam);
    glm::vec4& GetOrthographic(int active_cam);
    glm::vec3& GetPerspective(int active_cam);
    bool GetCamMode(int active_cam) const;
    void SetCamMode(bool mode,int active_cam);
    int GetActiveCam() const;
    void SetActiveCam(int active_cam);

    void AddBars();

};

class TransformCameraBars{
    std::vector<Input::F33Array> _camera_base;
    std::vector<Input::F4Array> _ortho_proj;
    std::vector<Input::F3Array> _perspect_proj;
    std::vector<bool> _camera_mode; //for each camera orthographic/perspective
    std::vector<float> _normal_len;
    int _active_cam;
public:
    TransformCameraBars();
    void AddBar();
    void SetActiveCam(int idx);
    int GetActiveCam() const;
    Input::F3ArrayIt GetPos(int active_cam);
    Input::F3ArrayIt GetAt(int active_cam);
    Input::F3ArrayIt GetUp(int active_cam);
    Input::F4ArrayIt GetOrtho(int active_cam);
    Input::F3ArrayIt GetPerspect(int active_cam);
    void SetCameraMode(int idx,bool mode);
    bool GetCameraMode(int idx) const;
    int GetCamCount() const;

};

class ZBufferInput{
    bool draw_depth_;
public:
    ZBufferInput();
    bool& DrawGetRef();
    const bool& DrawGetRef() const;
};


struct ModelColorsInput{
    std::vector<glm::vec3> ambient_;
    std::vector<glm::vec3> diffuse_;
    std::vector<glm::vec3> specular_;
    std::vector<float> shininess_;
    void AddBars();
};

struct Lights{
    std::vector<PointLight> point_input_;
    std::vector<ParallelLight> parallel_input_;

    void AddPointBars();
    void AddParallelBars();

    PointLight& getPointLight(int idx);
    ParallelLight& getParallelLight(int idx);

    bool empty() const;
};

#endif
