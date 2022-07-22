#include <Lighting.h>

/*std::shared_ptr<MeshModel> Light::light_mesh_ = Utils::LoadMeshModel(
        R"(C:\Users\Dany\CLionProjects\computergraphics2021-dp\Data\light_sphere.obj)");*/

Light::Light() : ambient_intensity_(default_color), diffuse_intensity_(default_color),
                 specular_intensity_(default_color) {}

glm::vec3 &Light::getAmbientIntensity() {
    return ambient_intensity_;
}

glm::vec3 &Light::getDiffuseIntensity() {
    return diffuse_intensity_;
}

glm::vec3 &Light::getSpecularIntensity() {
    return specular_intensity_;
}

/*const MeshModel &Light::getLightMesh() {
    return *light_mesh_;
}*/

glm::vec3 Light::CalculateAmbient(const glm::vec3 &ambient_color) const {
    return ambient_color * ambient_intensity_;
}

PointLight::PointLight(glm::vec3 pos) : position_(pos) {}



glm::vec3
PointLight::CalculateDiffuse(const glm::vec3 &diffuse_color, const glm::vec3 &point, const glm::vec3 &normal,
                             const glm::mat4 &model_view) const {
    glm::vec3 transformed_pos = CalculateTransformedPosition(model_view);
    glm::vec3 light_direction = glm::normalize(transformed_pos - point);
    glm::vec3 normalized_n = glm::normalize(normal);
    float resf = glm::dot(light_direction, normalized_n);
    float angle = glm::acos(resf);
    if(angle >= M_PI/2.f || angle <= -M_PI/2.f)
        return {0,0,0};
    return resf > 0 ? resf * diffuse_color * diffuse_intensity_ : glm::vec3{0,0,0};
}

glm::vec3 PointLight::CalculateSpecular(const glm::vec3 &specular_color, float shininess, const glm::vec3 &point,
                                        const glm::vec3 &normal, const glm::vec3 &cam_pos,
                                        const glm::mat4 &model_view) const {
    glm::vec3 transformed_pos = CalculateTransformedPosition(model_view);
    glm::vec3 light_direction = glm::normalize(transformed_pos - point);
    glm::vec3 relative_cam = glm::normalize(cam_pos - point);
    glm::vec3 normalized_n = glm::normalize(normal);
    glm::vec3 reflection = glm::normalize(glm::reflect(-light_direction,normalized_n));
    float dot = glm::dot(reflection, relative_cam);
    float angle = glm::acos(dot);
    if(angle >= M_PI/2.f || angle <= -M_PI/2.f)
        return {0,0,0};
    float base = dot > 0.0f ? dot : 0.0f;
    float scalar = glm::pow(base,shininess);
    return scalar * specular_color * specular_intensity_;
}

glm::vec3
PointLight::CalcColorAtPoint(const glm::vec3 &ambient_c, const glm::vec3 &diffuse_c, const glm::vec3 &specular_c,
                             float shininess,
                             const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &cam_pos,
                             const glm::mat4 &model_view) const {
    glm::vec3 final_color;
    return CalculateAmbient(ambient_c) + CalculateDiffuse(diffuse_c, p, n, model_view) + CalculateSpecular(specular_c, shininess, p,
                                                                                                           n, cam_pos, model_view);
    /*for (int i = 0; i < 3; ++i) {
        final_color[i] = final_color[i] > 1.f ? 1.f : final_color[i];     // cut down if bigger then maximum color.
        assert(final_color[i] >= 0);
    }*/
}

glm::vec3 &PointLight::getPosition() {
    return position_;
}

glm::mat4 PointLight::getLightTransform() const {
    return glm::translate(glm::mat4(1.f),position_) * glm::scale(glm::mat4(1.f),glm::vec3{light_scale}) ;
}

glm::vec3 PointLight::CalculateTransformedPosition(const glm::mat4 &transform) const {
    glm::vec4 v{position_,1};
    v = transform * v;
    if(v.w == 0)
        return {v.x,v.y,v.z};
    else
        return glm::vec3{v.x,v.y,v.z}/v.w;
}

ParallelLight::ParallelLight(glm::vec3 light_direction):light_direction_(light_direction) {}

glm::vec3 &ParallelLight::getLightDirection() {
    return light_direction_;
}

glm::vec3 ParallelLight::CalculateDiffuse(const glm::vec3 &diffuse_color, const glm::vec3 &normal) const {
    // given a TRANSFORMED normal, the calculation will be accurate.
    glm::vec3 normalized_light = glm::normalize(light_direction_);
    glm::vec3 normalized_normal = glm::normalize(normal);
    float resf = glm::dot(light_direction_, normalized_normal);
    return resf > 0 ? resf * diffuse_color * diffuse_intensity_ : glm::vec3{0, 0, 0};
}

glm::vec3 ParallelLight::CalculateSpecular(const glm::vec3 &specular_color, float shininess, const glm::vec3 &point,
                                           const glm::vec3 &normal, const glm::vec3 &cam_pos) const {
    glm::vec3 normalized_light = glm::normalize(light_direction_);
    glm::vec3 relative_cam = glm::normalize(cam_pos - point);
    glm::vec3 normalized_n = glm::normalize(normal);
    glm::vec3 reflection = -glm::reflect(normalized_light, normalized_n);
    float dot = glm::dot(reflection, relative_cam);
    float base = dot > 0.f ? dot : 0.f;
    float scalar = glm::pow(base, shininess);
    return scalar * specular_color * specular_intensity_;
}

glm::vec3
ParallelLight::CalcColorAtPoint(const glm::vec3 &ambient_c, const glm::vec3 &diffuse_c, const glm::vec3 &specular_c,
                                float shininess, const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &cam_pos) const {
    glm::vec3 final_color;
    return CalculateAmbient(ambient_c) + CalculateDiffuse(diffuse_c,n) + CalculateSpecular(specular_c,shininess,p,n,cam_pos);
    /*for (int i = 0; i < 3; ++i) {
        final_color[i] = final_color[i] > 1.f ? 1.f : final_color[i];     // cut down if bigger then maximum color.
        assert(final_color[i] >= 0);
    }*/
}
