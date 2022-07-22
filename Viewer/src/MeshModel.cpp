#include <iostream>
#include "MeshModel.h"
MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces_(faces),vertices_(vertices),normals_(normals),
	model_rotate_(1.0f),model_scale_(1.0f),model_translate_(1.0f),
	world_rotate_(1.0f),world_scale_(1.0f),world_translate_(1.0f)
{

}

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals,
                     std::vector<glm::vec2> textureCoords, const std::string &model_name) : faces_(faces),
                                                                                            vertices_(vertices),
                                                                                            normals_(normals),
                                                                                            model_name_(model_name),
                                                                                            model_translate_(1.f),
                                                                                            model_rotate_(1.f),
                                                                                            model_scale_(1.f),
                                                                                            world_translate_(1.f),
                                                                                            world_rotate_(1.f),
                                                                                            world_scale_(1.f),
                                                                                            ambient_color_(0.f),
                                                                                            diffuse_color_(0.f),
                                                                                            specular_color_(1.f),
                                                                                            shininess_(1.f) {
    modelVertices.reserve(3 * faces.size());
    for (int i = 0; i < faces.size(); i++)
    {
        Face currentFace = faces.at(i);
        for (int j = 0; j < 3; j++)
        {
            int vertexIndex = currentFace.GetVertexIndex(j) - 1;

            Vertex vertex;
            vertex.position = vertices[vertexIndex];
            vertex.normal = normals[vertexIndex];

            if (!textureCoords.empty())
            {
                int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
                vertex.textureCoords = textureCoords[textureCoordsIndex];
            }

            modelVertices.push_back(vertex);
        }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Vertex Texture Coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // unbind to make sure other code does not change it somewhere else
    glBindVertexArray(0);
}

MeshModel::~MeshModel()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

const Face& MeshModel::GetFace(int index) const
{
	return faces_[index];
}

int MeshModel::GetFacesCount() const
{
	return faces_.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name_;
}

void MeshModel::PrintVertices() const {
    for(const auto& vertex: vertices_){
        std::cout << "v " << &glm::to_string(vertex)[4] << std::endl;
    }
}

void MeshModel::PrintFaces() const {
    for(const auto& face: faces_){
        std::cout << "f ";
        for (int i = 0; i < 3; ++i) {
            std::cout << face.GetVertexIndex(i) << "//" << face.GetNormalIndex(i) <<" ";
            if (i == 2)
                std::cout << std::endl;
        }
    }
}

const glm::vec3 &MeshModel::GetVertex(int idx) const {
    return vertices_[idx];
}

glm::mat4 MeshModel::GetModelTransform() const {
    return model_scale_*model_rotate_*model_translate_;
}

glm::mat4 MeshModel::GetWorldTransform() const {
    return world_scale_*world_rotate_*world_translate_;
}

glm::mat4 MeshModel::GetTransform() const {
    return GetWorldTransform()*GetModelTransform();
}

void MeshModel::UpdateMScale(const glm::vec3 &vec) {
    model_scale_ = glm::identity<glm::mat4>();
    model_scale_[0].x=vec[0];
    model_scale_[1].y=vec[1];
    model_scale_[2].z=vec[2];
}

void MeshModel::UpdateMRotate(const glm::vec3 &vec) {
    model_rotate_ = MeshModel::CreateRotationMat(vec);
}

void MeshModel::UpdateMTranslate(const glm::vec3 &vec) {
    model_translate_[3] = glm::vec4(vec,1.0f);
}

void MeshModel::UpdateWScale(const glm::vec3 &vec) {
    world_scale_ = glm::identity<glm::mat4>();
    world_scale_[0].x=vec[0];
    world_scale_[1].y=vec[1];
    world_scale_[2].z=vec[2];
}

void MeshModel::UpdateWRotate(const glm::vec3 &vec) {
    world_rotate_ = MeshModel::CreateRotationMat(vec);
}

void MeshModel::UpdateWTranslate(const glm::vec3 &vec) {
    world_translate_[3] = glm::vec4(vec,1.0f);
}

int MeshModel::GetVertexCount() const {
    return vertices_.size();
}

const glm::vec3 &MeshModel::GetNormal(int idx) const {
    return normals_[idx];
}

glm::mat4 MeshModel::CreateRotationMat(const glm::vec3& vec) {
    glm::mat4 x_rotate = glm::rotate(glm::mat4(1.0f),vec[0],glm::vec3(1,0,0));
    glm::mat4 y_rotate = glm::rotate(glm::mat4(1.0f),vec[1],glm::vec3(0,1,0));
    glm::mat4 z_rotate = glm::rotate(glm::mat4(1.0f),vec[2],glm::vec3(0,0,1));
    return x_rotate*y_rotate*z_rotate;
}

void MeshModel::ResetTransformations() {
    model_translate_ = model_rotate_ = model_scale_ = world_translate_ = world_rotate_ = world_scale_ = glm::mat4(1.0f);
}

const glm::vec3 &MeshModel::getAmbientColor() const {
    return ambient_color_;
}

void MeshModel::setAmbientColor(const glm::vec3 &ambientColor) {
    ambient_color_ = ambientColor;
}

const glm::vec3 &MeshModel::getDiffuseColor() const {
    return diffuse_color_;
}

void MeshModel::setDiffuseColor(const glm::vec3 &diffuseColor) {
    diffuse_color_ = diffuseColor;
}

const glm::vec3 &MeshModel::getSpecularColor() const {
    return specular_color_;
}

void MeshModel::setSpecularColor(const glm::vec3 &specularColor) {
    specular_color_ = specularColor;
}

float MeshModel::getShininess() const {
    return shininess_;
}

void MeshModel::setShininess(float shininess) {
    shininess_ = shininess;
}

GLuint MeshModel::GetVAO() const
{
    return vao;
}

const std::vector<Vertex>& MeshModel::GetModelVertices()
{
    return modelVertices;
}