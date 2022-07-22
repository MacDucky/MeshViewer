#include "Input.h"

ModelInput::ModelInput(const float *const *info, int active_model,float vnormal_len=.1f,float fnormal_len=.1f,bool VN,bool FN,bool BB): model_idx(active_model),toggleFN(FN),toggleVN(VN),toggleBB(BB) {
    std::get<std::string>(transforms[0]) = "MT";
    std::get<std::string>(transforms[1]) = "MR";
    std::get<std::string>(transforms[2]) = "MS";
    std::get<std::string>(transforms[3]) = "WT";
    std::get<std::string>(transforms[4]) = "WR";
    std::get<std::string>(transforms[5]) = "WS";
    for (int i = 0; i < Input::NUM_OF_MODEL_TRANSFORMS; ++i) {
        glm::vec3 &v = this->operator[](i);
        const float *const input = info[i];
        for (int j = 0; j < 3; ++j) {
            v[j] = input[j];
        }
    }
    vnormal_len_=vnormal_len;
    fnormal_len_=fnormal_len;
}

glm::vec3 &ModelInput::operator[](int idx) {
    return std::get<glm::vec3>(transforms[idx]);

}

const glm::vec3 &ModelInput::operator[](int idx) const {
    return std::get<glm::vec3>(transforms[idx]);
}


std::string ModelInput::GetTransformName(int idx) const {
    return std::get<std::string>(transforms[idx]);
}

glm::vec3 ModelInput::GetTransformVec(int idx) const {
    return std::get<glm::vec3>(transforms[idx]);
}

int ModelInput::GetSelectedModel() const {
    return model_idx;
}

bool ModelInput::isToggleVn() const {
    return toggleVN;
}

bool ModelInput::isToggleFn() const {
    return toggleFN;
}

bool ModelInput::isToggleBb() const {
    return toggleBB;
}

float ModelInput::GetNormalVLen() const {
    return vnormal_len_;
}
float ModelInput::GetNormalFLen() const {
    return fnormal_len_;
}
//********************************************************************************************

void TransformModelBars::AddBar() {
    Input::F3Array zero = {0, 0, 0};
    Input::F3Array one = {1, 1, 1};
    Input::F63Array InitArray = {zero, zero, one, zero, zero, one};
    _transforms.push_back(InitArray);
    v_normal_len.push_back(1.f);
    f_normal_len.push_back(1.f);
}

Input::F3ArrayIt TransformModelBars::GetMT(int active_model) {
    return _transforms[active_model][0].begin();
}

Input::F3ArrayIt TransformModelBars::GetMR(int active_model) {
    return _transforms[active_model][1].begin();
}

Input::F3ArrayIt TransformModelBars::GetMS(int active_model) {
    return _transforms[active_model][2].begin();
}

Input::F3ArrayIt TransformModelBars::GetWT(int active_model) {
    return _transforms[active_model][3].begin();
}

Input::F3ArrayIt TransformModelBars::GetWR(int active_model) {
    return _transforms[active_model][4].begin();
}

Input::F3ArrayIt TransformModelBars::GetWS(int active_model) {
    return _transforms[active_model][5].begin();
}

float* TransformModelBars::GetVNormalLen(int active_model) {
    return &v_normal_len[active_model];
}
float* TransformModelBars::GetFNormalLen(int active_model) {
    return &f_normal_len[active_model];
}

TransformCameraBars::TransformCameraBars():_active_cam(-1) {}

void TransformCameraBars::AddBar() {
    Input::F3Array pos = {0, 0, 0};
    Input::F3Array at = {0,0,0};
    Input::F3Array up = {0, 1, 0};
    Input::F33Array bars = {pos, at, up};
    _camera_base.push_back(bars);
    {
        using namespace Input;
        F4Array ortho_init ={ORTHO_LEFT_INIT, ORTHO_RIGHT_INIT, ORTHO_BOT_INIT, ORTHO_TOP_INIT};
        _ortho_proj.push_back(ortho_init);
        F3Array perspect_init={PERSPECT_FOVY_INIT, PERSPECT_WIDTH_INIT, PERSPECT_HEIGHT_INIT};
        _perspect_proj.push_back(perspect_init);
    }
    _camera_mode.push_back(false);
}

void TransformCameraBars::SetActiveCam(int idx) {
    if(_camera_base.size() <= idx || idx < 0)
        throw Outofbounds();
    _active_cam=idx;
}

int TransformCameraBars::GetActiveCam() const {
    return _active_cam;
}

Input::F3ArrayIt TransformCameraBars::GetPos(int active_cam) {
    return _camera_base[active_cam][0].begin();
}

Input::F3ArrayIt TransformCameraBars::GetAt(int active_cam) {
    return _camera_base[active_cam][1].begin();
}

Input::F3ArrayIt TransformCameraBars::GetUp(int active_cam) {
    return _camera_base[active_cam][2].begin();
}

Input::F4ArrayIt TransformCameraBars::GetOrtho(int active_cam) {
    return _ortho_proj[active_cam].begin();
}

Input::F3ArrayIt TransformCameraBars::GetPerspect(int active_cam) {
    return _perspect_proj[active_cam].begin();
}

bool TransformCameraBars::GetCameraMode(int idx) const {
    return _camera_mode[idx];
}

/*bool TransformCameraBars::isChanged() const {
    return _is_changed;
}

void TransformCameraBars::SetIsChanged(bool is_changed) {
    _is_changed = is_changed;
}*/

int TransformCameraBars::GetCamCount() const {
    return _camera_base.size();
}

void TransformCameraBars::SetCameraMode(int idx,bool mode) {
    _camera_mode[idx]=mode;
}

/*******************************************************************/
const char *Outofbounds::what() const noexcept {
    return "TransformCameraBars out of bounds";
}

ZBufferInput::ZBufferInput():draw_depth_(false) {}

bool &ZBufferInput::DrawGetRef() {
    return draw_depth_;
}

const bool &ZBufferInput::DrawGetRef() const {
    return const_cast<bool&>(draw_depth_);
}

void ModelColorsInput::AddBars() {
    ambient_.emplace_back(default_color);
    diffuse_.emplace_back(default_color);
    specular_.emplace_back(default_color);
    shininess_.emplace_back(1.f);
}



PointLight &Lights::getPointLight(int idx) {
    return point_input_[idx];
}

ParallelLight &Lights::getParallelLight(int idx) {
    return parallel_input_[idx];
}

void Lights::AddPointBars() {
    point_input_.emplace_back();
}

void Lights::AddParallelBars() {
    parallel_input_.emplace_back();
}

bool Lights::empty() const {
    return point_input_.empty() && parallel_input_.empty();
}

glm::vec3 &MInput::GetMT(int active_model) {
    return mt[active_model];
}

glm::vec3 &MInput::GetMR(int active_model) {
    return mr[active_model];
}

glm::vec3 &MInput::GetMS(int active_model) {
    return ms[active_model];
}

glm::vec3 &MInput::GetWT(int active_model) {
    return wt[active_model];
}

glm::vec3 &MInput::GetWR(int active_model) {
    return wr[active_model];
}

glm::vec3 &MInput::GetWS(int active_model) {
    return ws[active_model];
}

float &MInput::GetVNLen(int active_model) {
    return vn_len[active_model];
}

float &MInput::GetFNLen(int active_model) {
    return fn_len[active_model];
}

void MInput::AddBars() {
    mt.emplace_back(0,0,0);
    mr.emplace_back(0,0,0);
    ms.emplace_back(1.f);
    wt.emplace_back(0,0,0);
    wr.emplace_back(0,0,0);
    ws.emplace_back(1.f);
    vn_len.emplace_back(1.f);
    fn_len.emplace_back(1.f);
}

glm::vec3 &CameraBars::GetPosition(int active_cam) {
    return position_[active_cam];
}

glm::vec3 &CameraBars::GetAt(int active_cam) {
    return at_[active_cam];
}

glm::vec3 &CameraBars::GetUp(int active_cam) {
    return up_[active_cam];
}

glm::vec4 &CameraBars::GetOrthographic(int active_cam) {
    return ortho_proj_[active_cam];
}

glm::vec3 &CameraBars::GetPerspective(int active_cam) {
    return perspective_proj_[active_cam];
}

bool CameraBars::GetCamMode(int active_cam) const {
    return camera_mode_[active_cam];
}

void CameraBars::SetCamMode(bool mode, int active_cam) {
    camera_mode_[active_cam] = mode;
}

void CameraBars::AddBars() {
    using namespace Input;
    position_.emplace_back(0,0,0);
    at_.emplace_back(0,0,0);
    up_.emplace_back(0,1,0);
    camera_mode_.emplace_back(0);
    ortho_proj_.emplace_back(ORTHO_LEFT_INIT, ORTHO_RIGHT_INIT, ORTHO_BOT_INIT, ORTHO_TOP_INIT);
    perspective_proj_.emplace_back(PERSPECT_FOVY_INIT, PERSPECT_WIDTH_INIT, PERSPECT_HEIGHT_INIT);
}

int CameraBars::GetActiveCam() const {
    return active_cam_;
}

void CameraBars::SetActiveCam(int active_cam) {
    active_cam_ = active_cam;
}
