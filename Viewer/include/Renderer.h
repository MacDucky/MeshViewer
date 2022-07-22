#pragma once
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Scene.h"
#include "Z_Buffer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <vector>
#include <array>
#include <ctime>


enum TypeOfLine{BASIC,SWAP_ROLES,REFLECT,SWAP_N_REFLECT};
typedef std::pair<glm::vec3, glm::vec3> Edge;

Edge operator*(const glm::mat4& mat, const Edge& start_end);
glm::vec3 operator*(const glm::mat4& mat,const glm::vec3& vec);
constexpr int NUM_CUBE_EDGES = 12;
#define PROJECTION(x) ((x).GetProjectionType() == ORTHOGRAPHIC) ? (x).GetOrthographicProjection() : (x).GetPerspectiveProjection()


class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
    void SetViewportSizes(int w, int h);
    void ResizeZBuffer(int w, int h);

	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
/******************************************************************************************/
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawBasic(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
    void DrawSwap(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
    void DrawReflect(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
    void DrawSNR(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
/******************************************************************************************/
    void CreateBuffers(int w, int h);
    void CreateOpenGLBuffer();
    void InitOpenGLRendering();
/************************************   Usage of DrawLine   *******************************/
    void DrawLineInEveryDirection(const glm::ivec2 &p1, GLuint radius, GLuint deg_step, const glm::vec3 &color);
    void WriteLove();
    void DrawHome();
    void Draw2D(const MeshModel &model);
    void DrawNormals(const Scene &scene);
    void DrawBB(const Scene &scene);

    void Draw3D(const MeshModel &model, const glm::mat4 &mvp, float width, float height);
    void DrawModels(Scene& scene);
//    void DrawCameras(const Scene &scene, float width, float height);
    void DrawVNormal(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view);
    void DrawFNormal(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view);
    void DrawBoundingBox(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view, std::array<Edge,NUM_CUBE_EDGES>bounding_box);
    /***********************************    Load Transforms   *********************************/
    // Creates a load transformation to place a model in the center of the screen.
    glm::mat4 CreateLoadTransform(const MeshModel &model, float model_screen_ratio = 2.0f/3) const;
    /***********************************    Transformed Vectors   *****************************/
    // Returns the bounding box edges to draw for a given model.
    static std::array<Edge, NUM_CUBE_EDGES> GetBoundingBox(const MeshModel &model);
public:

    static glm::vec3
    GetTransformedNormal(const glm::vec3 &normal, const glm::mat4 &model_view, const glm::mat4 &projection = glm::mat4(1.f));
    static glm::vec3 TransformAndDivideW(const glm::mat4& mvp, const glm::vec3& vertex);
    void LoadShaders();
    void LoadTextures();
private:
    glm::mat4 GetViewTransform(const Camera& camera) const;
    static glm::mat4 GetProjectionTransform(const Camera& camera) ;
    glm::mat4 GetViewPortTransform() const;
    /************************************   Z Buffer   ****************************************/
    /*float ComputeScalars(const glm::vec2& pa,const glm::vec2& pb,int x, int y) const;*/
    float DecideHalfPlane(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2) const;
    bool IsInTriangle(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p) const;
    float ComputeTriangleArea(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2) const;
    void ComputeBB(const glm::vec2 *p, float &min_x, float &max_x, float &min_y, float &max_y) const;
    void DrawTriangle(const glm::vec2& p0,const glm::vec2& p1,const glm::vec2& p2 , const glm::vec3& color);
    /*************************************   Lighting   ***************************************/
//    void DrawLightsModels(Scene& scene);
    /***********************************  Interpolations  *************************************/
    float ZLinearInterpolation(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec2 &p) const;
    glm::vec3 ColorIntensityInterpolation(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
                                          const glm::vec3 &c0, const glm::vec3 &c1, const glm::vec3 &c2,
                                          const glm::vec2 &p) const;
    /*************************************    Shading    **************************************/

    void DrawFlatShading(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &color);

    static glm::vec3 CalculateFlatShading(const Lights &lights, const MeshModel &meshModel, const glm::vec3 &cam_pos,
                                          const glm::mat4 &model_view, const glm::vec3 &p0, const glm::vec3 &p1,
                                          const glm::vec3 &p2);

    void DrawGouraudShading(const Lights &lights, const MeshModel &meshModel, const glm::vec3 &cam_pos,
                            const glm::mat4 &view, const glm::mat4 &projection);


    /***********************************    Fields   ******************************************/
    float* color_buffer_;
    int viewport_width_;
    int viewport_height_;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
	ZBuffer zb;
    ShaderProgram colorShader;
    Texture2D texture1;
};

