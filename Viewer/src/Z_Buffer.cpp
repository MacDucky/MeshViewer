#include "Z_Buffer.h"

#include <cmath>

ZBuffer::ZBuffer(int width, int height) : width_(width), height_(height),draw_depth_(false),z_buffer_(new float[width * height]){
    const int tot_size = width_*height_;
    for (int i = 0; i < tot_size; ++i) {
        z_buffer_[i] = -std::numeric_limits<float>::infinity();
    }
}

ZBuffer::~ZBuffer() {
    delete[] z_buffer_;
    z_buffer_ = nullptr;
}

void ZBuffer::ClearZBuffer() {
    const int tot_size = width_*height_;
    const float inf = std::numeric_limits<float>::infinity();
    /*max_depth_ = -inf;
    min_depth_ = 0;*/
    for (int i = 0; i < tot_size; ++i) {
        z_buffer_[i] = -inf;
    }
}

void ZBuffer::Resize(int width, int height) {
    delete[] z_buffer_;
    width_ = width;
    height_ = height;
    const int tot_size = width*height;
    z_buffer_ = new float[tot_size];
    ClearZBuffer();
}

float &ZBuffer::operator()(int x_index, int y_index) {
    return *(z_buffer_ + width_ * y_index + x_index);
}

const float &ZBuffer::operator()(int x_index, int y_index) const {
    return const_cast<ZBuffer&>(*this).operator()(x_index,y_index);
}

bool ZBuffer::TestAndSetAt(int x, int y, float z_input) {
    float& curr_z = operator()(x,y);
    if (z_input > curr_z){ // closer to camera
        curr_z = z_input;
        return true;
    }
    return false;
}

/*float ZBuffer::NormalizeGreyScale(int x, int y) const {
    const float& depth = operator()(x,y);
    return 1-(1/(1+exp(depth)));
}*/

float ZBuffer::NormalizeGreyScale(int x, int y,float z_near , float z_far) const{
    const float& depth = operator()(x,y);
    float relative = abs(depth/z_far);
    return 1-relative;
}

void ZBuffer::SetDrawDepth(bool flag) {
    draw_depth_ = flag;
}

bool ZBuffer::IsDrawDepth() const {
    return draw_depth_;
}

