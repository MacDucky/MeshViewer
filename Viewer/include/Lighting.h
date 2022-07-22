#ifndef MESHVIEWER_LIGHTING_H
#define MESHVIEWER_LIGHTING_H


#include <memory>
#include "glm/glm.hpp"
#include "MeshModel.h"
#include "Utils.h"

constexpr glm::vec3 default_color{0,0,0};
constexpr float light_scale = 0.5f;

class Light{
protected:
    glm::vec3 ambient_intensity_;
    glm::vec3 diffuse_intensity_;
    glm::vec3 specular_intensity_;
//    static std::shared_ptr<MeshModel> light_mesh_;

    glm::vec3 CalculateAmbient(const glm::vec3& ambient_color) const;

public:
    glm::vec3& getAmbientIntensity();
    glm::vec3& getDiffuseIntensity();
    glm::vec3& getSpecularIntensity();
    /*static const MeshModel& getLightMesh() ;*/

    Light();
    virtual ~Light() = default;
    Light& operator=(const Light& rhs) = delete;
    /*Light(const Light& other) = delete;*/
};

class PointLight: public Light{
    glm::vec3 position_;

    glm::vec3 CalculateTransformedPosition(const glm::mat4& transform) const;

    glm::vec3 CalculateDiffuse(const glm::vec3 &diffuse_color, const glm::vec3 &point, const glm::vec3 &normal,
                               const glm::mat4 &model_view) const;
    glm::vec3
    CalculateSpecular(const glm::vec3 &specular_color, float shininess, const glm::vec3 &point,
                      const glm::vec3 &normal, const glm::vec3 &cam_pos, const glm::mat4 &model_view) const;
public:
    explicit PointLight(glm::vec3 pos = {0,0,0});

    glm::vec3 CalcColorAtPoint(const glm::vec3 &ambient_c, const glm::vec3 &diffuse_c, const glm::vec3 &specular_c,
                               float shininess,
                               const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &cam_pos,
                               const glm::mat4 &model_view) const;
    glm::vec3& getPosition();
    glm::mat4 getLightTransform() const;

};

class ParallelLight: public Light{
    glm::vec3 light_direction_;

    glm::vec3 CalculateDiffuse(const glm::vec3& diffuse_color,const glm::vec3& normal) const;
    glm::vec3 CalculateSpecular(const glm::vec3 &specular_color, float shininess, const glm::vec3 &point, const glm::vec3& normal,
                      const glm::vec3& cam_pos) const;
public:
    explicit ParallelLight(glm::vec3 light_direction = {0,0,0});

    glm::vec3 CalcColorAtPoint(const glm::vec3 &ambient_c, const glm::vec3 &diffuse_c, const glm::vec3 &specular_c,
                               float shininess, const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &cam_pos) const;
    glm::vec3& getLightDirection();
};


#endif
