#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include "Face.h"

enum Axis{X,Y,Z};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
    MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals,std::vector<glm::vec2> textureCoords, const std::string& model_name);
	virtual ~MeshModel();

	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	const glm::vec3& GetVertex(int idx) const;
	int GetVertexCount() const;
	const glm::vec3& GetNormal(int idx) const;
/*******************   Prints   ************************/
    void PrintVertices() const;
    void PrintFaces() const;
/*******************   Transformations   ***************/

    // Model
    void UpdateMScale(const glm::vec3& vec);
    void UpdateMRotate(const glm::vec3& vec);   // receives rotation angles around each axis correspondingly.
    void UpdateMTranslate(const glm::vec3& vec);
    // World
    void UpdateWScale(const glm::vec3& vec);
    void UpdateWRotate(const glm::vec3& vec);
    void UpdateWTranslate(const glm::vec3& vec);

    void ResetTransformations();

    glm::mat4 GetModelTransform() const;
    glm::mat4 GetWorldTransform() const;
    glm::mat4 GetTransform() const;

    const glm::vec3 &getAmbientColor() const;
    void setAmbientColor(const glm::vec3 &ambientColor);

    const glm::vec3 &getDiffuseColor() const;
    void setDiffuseColor(const glm::vec3 &diffuseColor);

    const glm::vec3 &getSpecularColor() const;
    void setSpecularColor(const glm::vec3 &specularColor);

    float getShininess() const;
    void setShininess(float shininess);

    GLuint GetVAO() const;
    const std::vector<Vertex> &GetModelVertices();

private:
    static glm::mat4 CreateRotationMat(const glm::vec3& vec) ;
    std::vector<Face> faces_;
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;

//    std::vector<glm::vec3> textureCoords;

	std::string model_name_;
    std::vector<Vertex> modelVertices;

    GLuint vbo;
    GLuint vao;
    glm::mat4 model_translate_;
    glm::mat4 model_rotate_;
    glm::mat4 model_scale_;
    glm::mat4 world_translate_;

    glm::mat4 world_rotate_;
    glm::mat4 world_scale_;
    glm::vec3 ambient_color_;
    glm::vec3 diffuse_color_;

    glm::vec3 specular_color_;

    float shininess_;
};
