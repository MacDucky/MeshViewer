#ifndef MESHVIEWER_Z_BUFFER_H
#define MESHVIEWER_Z_BUFFER_H

#include <cassert>
#include <limits>

class ZBuffer{
    float* z_buffer_;
    int width_;
    int height_;
    bool draw_depth_;
    /*float min_depth_;
    float max_depth_;*/

    // Auxiliary methods:


public:
    ZBuffer(int width,int height);
    ~ZBuffer();

    void ClearZBuffer();
    void Resize(int width,int height);

    // Convenient access.
    float& operator()(int width,int height);
    const float& operator()(int width,int height) const;

    // If the current element is closer then the item drawn, then return true.
    bool TestAndSetAt(int x, int y, float z_input);
    // Normalize depth values in to [0,1]
    float NormalizeGreyScale(int x, int y,float z_near = 0.1f, float z_far = 100.f) const;

    void SetDrawDepth(bool flag);
    bool IsDrawDepth() const;


};

#endif
