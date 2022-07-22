#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
    zb(viewport_width,viewport_height),
	viewport_width_(viewport_width),
	viewport_height_(viewport_height)
{
	/*InitOpenGLRendering();
	CreateBuffers(viewport_width, viewport_height);*/
}

Renderer::~Renderer()
{
	delete[] color_buffer_;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width_) return;
	if (j < 0) return; if (j >= viewport_height_) return;

	color_buffer_[INDEX(viewport_width_, i, j, 0)] = color.x;
	color_buffer_[INDEX(viewport_width_, i, j, 1)] = color.y;
	color_buffer_[INDEX(viewport_width_, i, j, 2)] = color.z;
}


// Draw a line in every direction.
void Renderer::DrawLineInEveryDirection(const glm::ivec2 &p1, GLuint radius, GLuint deg_step, const glm::vec3 &color) {
    glm::ivec2 p2;
    unsigned int denominator = 180 / deg_step;
    unsigned int total_steps = denominator * 2;
    for (int i = 0; i < total_steps; i++) {
        p2[0] = p1[0] + radius * cos((M_PI * i) / denominator);
        p2[1] = p1[1] + radius * sin((M_PI * i) / denominator);
        DrawLine(p1, p2, color);
    }
}
// Draw a line using the bresenham's line drawing algorithm algorithm
void Renderer::DrawLine(const glm::ivec2 &p1, const glm::ivec2 &p2, const glm::vec3 &color) {
    TypeOfLine type;
    glm::ivec2 temp1(p1),temp2(p2);
    if(temp1[0]>temp2[0] || (temp1[0]==temp2[0] && temp1[1]>temp2[1]))  //swap so that we would always draw from left to right.
        std::swap(temp1,temp2);
    GLint dx=temp2[0]-temp1[0],dy=temp2[1]-temp1[1]+0.5; //     0.5 to fix rounding of negative numbers.
    if(dy>=0){                  //drawing from down to up.
        if (dy<=dx)             //x-step is bigger then y-step
            type=BASIC;         //basic taught function
        else
            type=SWAP_ROLES;    //swap x and y roles
    }
    else{                       // drawing from up to down
        if(abs(dy)<=abs(dx))     // x-step is bigger then y-step
            type=REFLECT;       // then we just reflect on x axis.
        else        // both y step is bigger then x step and we draw from up to down,so we swap roles and reflect.
            type=SWAP_N_REFLECT;
    }
    switch(type){
        case BASIC:
            DrawBasic(temp1, temp2, color); break;
        case SWAP_ROLES:
            DrawSwap(temp1, temp2, color); break;
        case REFLECT:
            DrawReflect(temp1, temp2, color); break;
        case SWAP_N_REFLECT:
            DrawSNR(temp1, temp2, color); break;
    }
}
void Renderer::DrawBasic(const glm::ivec2 &p1, const glm::ivec2 &p2, const glm::vec3 &color) {
    GLint d_y = p2[1] - p1[1];
    GLint d_x = p2[0] - p1[0];
    GLint x = p1[0], y = p1[1];     // starting point
    GLint error = -d_x;
    while (x <= p2[0]) {
        if (error > 0) {
            y++;
            error -= 2 * d_x;
        }
        PutPixel(x, y, color);
        x++;
        error += 2 * d_y;
    }
}
void Renderer::DrawSwap(const glm::ivec2 &p1, const glm::ivec2 &p2, const glm::vec3 &color) {
    GLint d_y = p2[1] - p1[1];
    GLint d_x = p2[0] - p1[0];
    GLint x = p1[0], y = p1[1];     // starting point
    GLint error = -d_y;
    while (y <= p2[1]) {
        if (error > 0) {
            x++;
            error -= 2 * d_y;
        }
        PutPixel(x, y, color);
        y++;
        error += 2 * d_x;
    }
}
void Renderer::DrawReflect(const glm::ivec2 &p1, const glm::ivec2 &p2, const glm::vec3 &color) {
    GLint d_y = p2[1] - p1[1];
    GLint d_x = p2[0] - p1[0];
    GLint x = p1[0], y = p1[1];     // starting point
    GLint error = -d_x;
    while (x <= p2[0]) {
        if (error > 0) {
            y--;
            error -= 2 * d_x;
        }
        PutPixel(x, y, color);
        x++;
        error -= 2 * d_y;
    }
}
void Renderer::DrawSNR(const glm::ivec2 &p1, const glm::ivec2 &p2, const glm::vec3 &color) {
    GLint d_y = p2[1] - p1[1];
    GLint d_x = p2[0] - p1[0];
    GLint x = p1[0], y = p1[1];     // starting point
    GLint error = d_y;
    while (y >= p2[1]) {
        if (error > 0) {
            x++;
            error += 2 * d_y;
        }
        PutPixel(x, y, color);
        y--;
        error += 2 * d_x;
    }
}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenGLBuffer(); //Do not remove this line.
	color_buffer_ = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex_);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc_);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc_);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The texture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width_, viewport_height_, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width_, viewport_height_);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width_, viewport_height_, GL_RGB, GL_FLOAT, color_buffer_);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc_);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width_; i++)
	{
		for (int j = 0; j < viewport_height_; j++)
		{
			PutPixel(i, j, color);
		}
	}
	zb.ClearZBuffer();
}


void Renderer::DrawHome() {
	glm::vec3 yellow(255,255, 0);
	glm::vec3 red(255, 0, 0);
	glm::vec3 blue(0, 0,255);
	glm::vec3 green(0, 250, 0);
	glm::vec3 purple(128, 0, 128);

	for (int i = 0; i <=viewport_height_ ; i++)
	{
		for (int j = 0; j <= viewport_width_; j++)
		{
			if (i< 1.5*(viewport_height_ / 5))
				PutPixel(j, i, green);
			else
				PutPixel(j, i,blue);
		}
	}

	GLuint r3 = 2.5 * (viewport_height_ / 20);
	GLuint step3 =2;
	glm::ivec2 sun((viewport_width_) / 10, 8 * (viewport_height_ / 10));
	DrawLineInEveryDirection(sun, r3, step3, yellow);

	for (int i = (viewport_height_ / 5); i <= 3* (viewport_height_ / 5); i++)
	{
		for (int j =  (viewport_width_) / 5; j <= 4* (viewport_width_) /5; j++)
		{
			if(i >= (viewport_height_ / 5) && i <= 2 * (viewport_height_ / 5) && j >= 2 * (viewport_width_) / 5 && j <= 2.5 * (viewport_width_) / 5)
				PutPixel(j, i,purple);
			else if (i >= 1.9*(viewport_height_ / 5) && i <= 2.3 * (viewport_height_ / 5) && j >= 3 * (viewport_width_) / 5 && j <=3.4 * (viewport_width_) / 5)
				PutPixel(j, i,blue);
			else
				PutPixel(j, i,yellow);
		}
	}

	int start = (viewport_width_) / 10;
	int end = 9 * (viewport_width_) / 10;

	for (int i = 3*(viewport_height_ / 5) ;  i <= 4*(viewport_height_/5 ) ;  i++)
	{
		for (int j = start; j <= end; j++)
		{
			PutPixel(j, i, red);
		}
		start++;
		end--;
	}

	GLuint r = 0.3*(viewport_height_ / 10);
	GLuint step = 5;

	glm::ivec2 flower1(0.5 * (viewport_width_) / 5, 1.2* (viewport_height_ / 5));
	glm::ivec2 flower2(4.5 * (viewport_width_) / 5, 1.2 * (viewport_height_ / 5));
	glm::ivec2 flower3(1 * (viewport_width_) / 5, 0.5 * (viewport_height_ / 5));
	glm::ivec2 flower4(2.5 * (viewport_width_) / 5, 0.5 * (viewport_height_ / 5));
	glm::ivec2 flower5(4 * (viewport_width_) / 5, 0.5 * (viewport_height_ / 5));
	glm::ivec2 flower6(1.75 * (viewport_width_) / 5, 0.85 * (viewport_height_ / 5));
	glm::ivec2 flower7(3.25 * (viewport_width_) / 5, 0.85 * (viewport_height_ / 5));
	DrawLineInEveryDirection(flower1, r, step, purple);
	DrawLineInEveryDirection(flower2, r, step, purple);
	DrawLineInEveryDirection(flower3, r, step, purple);
	DrawLineInEveryDirection(flower4, r, step, purple);
	DrawLineInEveryDirection(flower5, r, step, purple);
	DrawLineInEveryDirection(flower6, r, step, purple);
	DrawLineInEveryDirection(flower7, r, step, purple);

	GLuint r2 = 0.3 * (viewport_height_ / 20);
	GLuint step2 = 1;
	glm::ivec2 knok(2.1 * (viewport_width_) / 5, 1.5* (viewport_height_ / 5));
	DrawLineInEveryDirection(knok, r2, step2, green);


}

void Renderer::WriteLove() {
	int w = viewport_width_ / 10;
	int h = viewport_height_ / 3;
	int thick = 15;

	GLuint r = (viewport_height_ / 6);
	GLuint step = 3;

	glm::vec3 fuchsia(255, 0, 255);
	glm::vec3 Aqua(0, 255, 255);
	glm::vec3 bluey(0, 0, 128);
	glm::vec3 greeny(0, 250, 154);
	glm::vec3 purple(128, 0, 128);


	glm::ivec2 pointL1(1.5 * (viewport_width_) / 10, 2 * (viewport_height_ / 3));
	glm::ivec2 pointL2(1.5 * (viewport_width_) / 10, (viewport_height_ / 3));
	glm::ivec2 pointL3(2.5 * (viewport_width_) / 10, (viewport_height_ / 3));

	glm::ivec2 pointO1(4 * (viewport_width_) / 10, 1.5 * (viewport_height_ / 3));

	glm::ivec2 pointV1(5.5 * (viewport_width_) / 10, 2 * (viewport_height_ / 3));
	glm::ivec2 pointV2(6 * (viewport_width_) / 10, (viewport_height_ / 3));
	glm::ivec2 pointV3(6.5 * (viewport_width_) / 10, 2 * (viewport_height_ / 3));

	glm::ivec2 pointE1(7.5 * (viewport_width_) / 10, 2 * (viewport_height_ / 3));
	glm::ivec2 pointE2(7.5 * (viewport_width_) / 10, 1.5 * (viewport_height_ / 3));
	glm::ivec2 pointE3(7.5 * (viewport_width_) / 10, (viewport_height_ / 3));
	glm::ivec2 pointE4(8.5 * (viewport_width_) / 10, 2 * (viewport_height_ / 3));
	glm::ivec2 pointE5(8.5 * (viewport_width_) / 10, 1.5 * (viewport_height_ / 3));
	glm::ivec2 pointE6(8.5 * (viewport_width_) / 10, (viewport_height_ / 3));

	for (int i = 2 * (viewport_height_ / 3); i > (viewport_height_ / 3); i--)
	{
		for (int j = 1.5 * (viewport_width_) / 10; j <= 1.5 * (viewport_width_) / 10 + thick; j++)
		{
			PutPixel(j, i, purple);
		}
	}

	//DrawLine(pointL1, pointL2, purple);
	DrawLine(pointL2, pointL3, purple);
	DrawLineInEveryDirection(pointO1, r, step, bluey);
	DrawLine(pointV1, pointV2, greeny);
	DrawLine(pointV2, pointV3, greeny);
	DrawLine(pointE1, pointE4, fuchsia);
	DrawLine(pointE2, pointE5, fuchsia);
	DrawLine(pointE3, pointE6, fuchsia);
	for (int i = 2 * (viewport_height_ / 3); i > (viewport_height_ / 3); i--)
	{
		for (int j = (7.5 * (viewport_width_) / 10); j <= (7.5 * (viewport_width_) / 10 + thick); j++)
		{
			PutPixel(j, i, purple);
		}
	}
	//DrawLine(pointE1, pointE3, fuchsia);
}

void Renderer::Draw2D(const MeshModel& model) {
    glm::vec3 black(0,0,0);
    glm::mat4 load = CreateLoadTransform(model);
    glm::mat4 transform = model.GetTransform()*load;
    for (int i = 0; i < model.GetFacesCount(); ++i) {
        const Face& face = model.GetFace(i);
        glm::vec3 p1 = model.GetVertex(face.GetVertexIndex(0)-1);
        glm::vec3 p2 = model.GetVertex(face.GetVertexIndex(1)-1);
        glm::vec3 p3 = model.GetVertex(face.GetVertexIndex(2)-1);
        glm::vec4 temp = {p1,1};
        temp = transform*temp;
        glm::ivec2 _p1 = {temp.x,temp.y};
        temp = {p2,1};
        temp = transform * temp;
        glm::ivec2 _p2 = {temp.x,temp.y};
        temp = transform*temp;
        temp = {p3,1};
        temp = transform * temp;
        glm::ivec2 _p3 = {temp.x,temp.y};
        DrawLine(_p1,_p2,black);
        DrawLine(_p2,_p3,black);
        DrawLine(_p1,_p3,black);
    }
}

void Renderer::Draw3D(const MeshModel &model, const glm::mat4 &mvp, float width, float height) {
    glm::vec3 black(0,0,0);
    for (int i = 0; i < model.GetFacesCount(); ++i) {
        const Face& face = model.GetFace(i);
        const glm::vec3& p1 = model.GetVertex(face.GetVertexIndex(0) - 1);
        const glm::vec3& p2 = model.GetVertex(face.GetVertexIndex(1) - 1);
        const glm::vec3& p3 = model.GetVertex(face.GetVertexIndex(2) - 1);
        // sadly can't use the overloaded operator* here, since i need to divide by w.

        glm::vec4 temp = {p1,1};
        temp = mvp * temp;
        temp /= temp.w;
        glm::vec2 fp1 = {temp.x,temp.y};

        temp = {p2,1};
        temp = mvp * temp;
        temp /= temp.w;
        glm::vec2 fp2 = {temp.x,temp.y};

        temp = {p3,1};
        temp = mvp * temp;
        temp /= temp.w;
        glm::vec2 fp3 = {temp.x,temp.y};

        /*fp1-fp3 are in normalized device coordinates*/

        glm::vec2 window_size(width,height) , offset = window_size/2.f;
        glm::ivec2 v1_window_pos = ((fp1 + 1.f) / 2.f) * window_size; /*+ offset;*/
        glm::ivec2 v2_window_pos = ((fp2 + 1.f) / 2.f) * window_size; /*+ offset;*/
        glm::ivec2 v3_window_pos = ((fp3 + 1.f) / 2.f) * window_size; /*+ offset;*/
        DrawLine(v1_window_pos,v2_window_pos,black);
        DrawLine(v2_window_pos,v3_window_pos,black);
        DrawLine(v1_window_pos,v3_window_pos,black);
    }
}

void Renderer::DrawNormals(const Scene &scene) {
    const MeshModel& model = scene.GetActiveModel();
    glm::vec3 start,direction;
    glm::mat4 transform = model.GetTransform()* CreateLoadTransform(model);
    if(scene.isVertexNormalToggle()){
        for (int i = 0; i < model.GetFacesCount(); ++i) {
            const Face& face = model.GetFace(i);
            for (int j = 0; j < 3; ++j) {
                glm::vec3 vertex = model.GetVertex(face.GetVertexIndex(j) - 1);
                glm::vec3 vn = model.GetNormal(face.GetNormalIndex(j) - 1);
                glm::vec4 temp = {vertex, 1};
                temp = transform * temp;
                vertex = {temp.x, temp.y, temp.z};
                temp = {vn, 1};
                temp = transform * temp;
                vn = {temp.x, temp.y, temp.z};
                vn = glm::normalize(vn);
                vn *= 50;
                glm::vec3 end = vertex + vn;
                glm::ivec2 istart(vertex.x,vertex.y),iend(end.x,end.y);
                glm::vec3 green(0,1,0);
                DrawLine(istart,iend,green);
            }
        }
    }
    if(scene.isFaceNormalToggle()){
        for (int i = 0; i < model.GetFacesCount(); ++i) {
            const Face& face = model.GetFace(i);
            glm::vec3 p1 = model.GetVertex(face.GetVertexIndex(0)-1);
            glm::vec3 p2 = model.GetVertex(face.GetVertexIndex(1)-1);
            glm::vec3 p3 = model.GetVertex(face.GetVertexIndex(2)-1);
            glm::vec4 temp = {p1,1};
            temp = transform * temp;
            p1 = {temp.x,temp.y,temp.z};
            temp = {p2,1};
            temp = transform * temp;
            p2 = {temp.x,temp.y,temp.z};
            temp = {p3,1};
            temp = transform * temp;
            p3 = {temp.x,temp.y,temp.z};
            // transformed triangle.
            start = (p1+p2+p3);
            for (int j = 0; j < 3; ++j) {
                start[j]/=3;
            } // now it's the real starting point.
            direction = glm::triangleNormal(p1,p2,p3);
            direction = glm::normalize(direction);
            direction *= 50; // scalar is currently set to a hardcoded size
            glm::vec3 end_point = start + direction;
            glm::vec3 red(1,0,0);
            glm::ivec2 istart = {start.x,start.y},iend={end_point.x,end_point.y};
            DrawLine(istart,iend,red);
        }
    }
}

void Renderer::DrawBB(const Scene &scene) {
    if(scene.isBoundingBoxToggle()){
        MeshModel& model = scene.GetActiveModel();
        glm::mat4 model_transform = model.GetTransform()* CreateLoadTransform(model);
        float min_x,max_x,min_y,max_y,min_z,max_z;
        for (int i = 0; i < model.GetVertexCount(); ++i) {
            glm::vec4 vertex4 = model_transform*glm::vec4(model.GetVertex(i),1);
            glm::vec3 vertex(vertex4.x,vertex4.y,vertex4.z);
            if(i==0){
                min_x = max_x = vertex.x;
                min_y = max_y = vertex.y;
                min_z = max_z = vertex.z;
            }
            min_x = std::min(min_x,vertex.x);
            max_x = std::max(max_x,vertex.x);
            min_y = std::min(min_y,vertex.y);
            max_y = std::max(max_y,vertex.y);
            min_z = std::min(min_z,vertex.z);
            max_z = std::max(max_z,vertex.z);
        }
        glm::vec3 mxmymz(min_x,min_y,min_z),Mxmymz(max_x,min_y,min_z),mxMymz(min_x,max_y,min_z),MxMymz(max_x,max_y,min_z);
        glm::vec3 mxmyMz(min_x,min_y,max_z),MxmyMz(max_x,min_y,max_z),mxMyMz(min_x,max_y,max_z),MxMyMz(max_x,max_y,max_z);
        glm::vec3 blue(0,0,1);
        DrawLine(mxmymz,Mxmymz,blue);
        DrawLine(mxmymz,mxMymz,blue);
        DrawLine(mxmymz,mxmyMz,blue);
        DrawLine(MxMymz,mxMymz,blue);
        DrawLine(MxMymz,MxMyMz,blue);
        DrawLine(MxMymz,Mxmymz,blue);
        DrawLine(MxmyMz,mxmyMz,blue);
        DrawLine(MxmyMz,MxMyMz,blue);
        DrawLine(MxmyMz,Mxmymz,blue);
        DrawLine(mxMyMz,MxMyMz,blue);
        DrawLine(mxMyMz,mxmyMz,blue);
        DrawLine(mxMyMz,mxMymz,blue);
    }
}

int Renderer::GetViewportWidth() const
{
	return viewport_width_;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height_;
}

glm::mat4 Renderer::CreateLoadTransform(const MeshModel &model, float model_screen_ratio) const {
    float max_left, max_right, max_up, max_down,x_translate,y_translate;
    for (int i = 0; i < model.GetVertexCount(); ++i) {
        glm::vec3 vertex = model.GetVertex(i);
        if (i == 0) {
            max_left = max_right = vertex[0];
            max_down = max_up = vertex[1];
        }
        //finding the most extreme points
        if (max_left > vertex[0])
            max_left = vertex[0];
        if (max_right < vertex[0])
            max_right = vertex[0];
        if (max_up < vertex[1])
            max_up = vertex[1];
        if (max_down > vertex[1])
            max_down = vertex[1];
    }
    float d_x = max_right - max_left, d_y = max_up - max_down;
    assert(d_x >= 0 && d_y >= 0);
    float max_delta = (d_x > d_y) ? d_x : d_y;
    float scale = (((viewport_height_ > viewport_width_ ? viewport_width_ : viewport_height_) * model_screen_ratio) / max_delta);
    glm::mat4 scale_transform(1.0f), translate_transform(1.0f); // initialized to identity matrix
    scale_transform[0].x = scale;
    scale_transform[1].y = scale;
    scale_transform[2].z = scale; // scale is set
    float average_up_down = ((max_up+max_down)/2)*scale;
    if(average_up_down < viewport_height_/2)
        y_translate = abs(viewport_height_/2 - average_up_down);
    else
        y_translate = - abs(viewport_height_/2 - average_up_down);
    float average_left_right = ((max_right + max_left)/2)*scale;
    if(average_left_right < viewport_width_/2){
        x_translate = abs(viewport_width_/2 - average_left_right);
    }
    else{
        x_translate = -abs(viewport_width_/2 - average_left_right);
    }
    translate_transform[3] = glm::vec4(x_translate,y_translate,0,1); // translate to center of the screen.
    return translate_transform * scale_transform;
}


void Renderer::Render(const Scene &scene) {
    int cam_count = scene.GetCameraCount();
    Scene& scene_ = const_cast<Scene&>(scene);
    if(cam_count > 0){
        int model_count = scene.GetModelCount();
        const Camera& camera = scene_.GetActiveCamera();
        glm::mat4 view = GetViewTransform(camera);
        glm::mat4 projection = GetProjectionTransform(camera);
        for (int i = 0; i < model_count; ++i) {
            MeshModel& meshModel = scene_.GetModel(i);
            glm::mat4 model = meshModel.GetTransform();

            // Activate the 'colorShader' program (vertex and fragment shaders)
            colorShader.use();

            // Set the uniform variables
            colorShader.setUniform("model", model);
            colorShader.setUniform("view", view);
            colorShader.setUniform("projection", projection);

            colorShader.setUniform("draw_toon",scene_.getShadeMode() == ShadeMode::TOON);
            colorShader.setUniform("num_colors",scene_.getToonColors());
            int& colors = scene_.getToonColors();
            colors = colors;
            bool draw_toon = scene_.getShadeMode() == ShadeMode::TOON;
            draw_toon = draw_toon;
            colorShader.setUniform("material.textureMap", 0);
            colorShader.setUniform("material.model_ambient_color", meshModel.getAmbientColor());
            colorShader.setUniform("material.model_diffuse_color",meshModel.getDiffuseColor());
            colorShader.setUniform("material.model_specular_color",meshModel.getSpecularColor());
            colorShader.setUniform("material.shininess" , meshModel.getShininess());

            colorShader.setUniform("camera_position" , camera.GetCameraPosition());
            if (!scene_.lights_.empty()){
                const MeshModel& light_mesh = scene.GetLightModel();

                if (!scene.lights_.point_input_.empty()){
                    std::vector<PointLight>& point_lights = scene_.lights_.point_input_;
                    int point_light_count = scene.lights_.point_input_.size();
                    std::unique_ptr<glm::vec3*> point_light_positions = std::make_unique<glm::vec3*>(new glm::vec3[point_light_count]);
                    std::unique_ptr<glm::vec3*> point_ambient_intensity = std::make_unique<glm::vec3*>(new glm::vec3[point_light_count]);
                    std::unique_ptr<glm::vec3*> point_diffuse_intensity = std::make_unique<glm::vec3*>(new glm::vec3[point_light_count]);
                    std::unique_ptr<glm::vec3*> point_specular_intensity = std::make_unique<glm::vec3*>(new glm::vec3[point_light_count]);
                    for (int j = 0; j < point_light_count; ++j) {
                        PointLight& light = point_lights[j];
                        const glm::vec3& light_pos = light.getPosition();
                        glm::mat4 light_transform =
                                glm::scale(glm::mat4(1.f), glm::vec3(0.1f)) * glm::translate(glm::mat4(1.f), light_pos);

                        //Drawing the point lights...
                        colorShader.setUniform("draw_light", true);  // Set draw a light flag.
                        colorShader.setUniform("light_transform",light_transform);

                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        glBindVertexArray(light_mesh.GetVAO());
                        glDrawArrays(GL_TRIANGLES, 0, const_cast<MeshModel&>(light_mesh).GetModelVertices().size());
                        glBindVertexArray(0);

                        colorShader.setUniform("draw_light", false); // Disable.

                        (*point_light_positions)[j] = light_pos;
                        (*point_ambient_intensity)[j] = light.getAmbientIntensity();
                        (*point_diffuse_intensity)[j] = light.getDiffuseIntensity();
                        (*point_specular_intensity)[j] = light.getSpecularIntensity();
                    }
                    colorShader.setUniform("point_light_count",point_light_count);
                    colorShader.setUniform("point_light_position",*point_light_positions,point_light_count);
                    colorShader.setUniform("point_ambient_intensity",*point_ambient_intensity,point_light_count);
                    colorShader.setUniform("point_diffuse_intensity",*point_diffuse_intensity,point_light_count);
                    colorShader.setUniform("point_specular_intensity",*point_specular_intensity,point_light_count);
                }

                if (!scene.lights_.parallel_input_.empty()){
                    std::vector<ParallelLight>& parallel_lights = scene_.lights_.parallel_input_;
                    int parallel_light_count = scene.lights_.parallel_input_.size();
                    std::unique_ptr<glm::vec3*> parallel_light_positions = std::make_unique<glm::vec3*>(new glm::vec3[parallel_light_count]);
                    std::unique_ptr<glm::vec3*> parallel_ambient_intensity = std::make_unique<glm::vec3*>(new glm::vec3[parallel_light_count]);
                    std::unique_ptr<glm::vec3*> parallel_diffuse_intensity = std::make_unique<glm::vec3*>(new glm::vec3[parallel_light_count]);
                    std::unique_ptr<glm::vec3*> parallel_specular_intensity = std::make_unique<glm::vec3*>(new glm::vec3[parallel_light_count]);
                    for (int j = 0; j < parallel_light_count; ++j) {
                        ParallelLight& light = parallel_lights[j];
                        const glm::vec3& light_dir = light.getLightDirection();

                        (*parallel_light_positions)[j] = light_dir;
                        (*parallel_ambient_intensity)[j] = light.getAmbientIntensity();
                        (*parallel_diffuse_intensity)[j] = light.getDiffuseIntensity();
                        (*parallel_specular_intensity)[j] = light.getSpecularIntensity();
                    }
                    colorShader.setUniform("parallel_light_count",parallel_light_count);
                    colorShader.setUniform("parallel_light_direction",*parallel_light_positions,parallel_light_count);
                    colorShader.setUniform("parallel_ambient_intensity",*parallel_ambient_intensity,parallel_light_count);
                    colorShader.setUniform("parallel_diffuse_intensity",*parallel_diffuse_intensity,parallel_light_count);
                    colorShader.setUniform("parallel_specular_intensity",*parallel_specular_intensity,parallel_light_count);
                }
            }


            texture1.bind();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBindVertexArray(meshModel.GetVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshModel.GetModelVertices().size());
            glBindVertexArray(0);

            texture1.unbind();

            /*// Drag our model's faces (triangles) in line mode (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindVertexArray(meshModel.GetVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshModel.GetModelVertices().size());
            glBindVertexArray(0);*/
        }
    }
}


std::array<Edge, NUM_CUBE_EDGES> Renderer::GetBoundingBox(const MeshModel &model) {
    const glm::mat4& transform = model.GetTransform();
    float min_x,max_x,min_y,max_y,min_z,max_z;
    for (int i = 0; i < model.GetVertexCount(); ++i) {
        glm::vec3 vertex = transform * model.GetVertex(i);
        if(i==0){
            min_x = max_x = vertex.x;
            min_y = max_y = vertex.y;
            min_z = max_z = vertex.z;
        }
        min_x = std::min(min_x,vertex.x);
        max_x = std::max(max_x,vertex.x);
        min_y = std::min(min_y,vertex.y);
        max_y = std::max(max_y,vertex.y);
        min_z = std::min(min_z,vertex.z);
        max_z = std::max(max_z,vertex.z);
    }
    std::array<Edge,NUM_CUBE_EDGES> cube;
    glm::vec3 mxmymz(min_x,min_y,min_z),Mxmymz(max_x,min_y,min_z),mxMymz(min_x,max_y,min_z),MxMymz(max_x,max_y,min_z);
    glm::vec3 mxmyMz(min_x,min_y,max_z),MxmyMz(max_x,min_y,max_z),mxMyMz(min_x,max_y,max_z),MxMyMz(max_x,max_y,max_z);
    cube = {std::make_pair(mxmymz, Mxmymz),
            std::make_pair(mxmymz, mxMymz),
            std::make_pair(mxmymz, mxmyMz),
            std::make_pair(MxMymz, mxMymz),
            std::make_pair(MxMymz, MxMyMz),
            std::make_pair(MxMymz, Mxmymz),
            std::make_pair(MxmyMz, mxmyMz),
            std::make_pair(MxmyMz, MxMyMz),
            std::make_pair(MxmyMz, Mxmymz),
            std::make_pair(mxMyMz, MxMyMz),
            std::make_pair(mxMyMz, mxmyMz),
            std::make_pair(mxMyMz, mxMymz)};
    return cube;
}

void Renderer::DrawModels(Scene &scene) {
    std::vector<std::array<Edge, 3>> modelVN; // model vertex normals.
    std::vector<Edge> modelFN;  //model face normals
    std::array<Edge, NUM_CUBE_EDGES> bounding_box;  //model bounding box
    int model_count = scene.GetModelCount();
    Camera &camera = scene.GetActiveCamera();
    zb.SetDrawDepth(scene.isDrawDepthToggle());
    ShadeMode shade = scene.getShadeMode();
    for (int i = 0; i < model_count; ++i) {
        const MeshModel &model = scene.GetModel(i);
        glm::mat4 model_transform = model.GetTransform();
        const glm::mat4 view = GetViewTransform(camera);
        const glm::mat4 projection = GetProjectionTransform(camera);
        const glm::mat4 viewport_transform = GetViewPortTransform();
        const glm::mat4 mv = view*model_transform;
        glm::mat4 mvp = projection * view * model_transform;
        for (int j = 0; j < model.GetFacesCount() && shade == FLAT; ++j) {
            const Face& face = model.GetFace(j);
            glm::vec3 p0,p1,p2;
            p0 = model.GetVertex(face.GetVertexIndex(0)-1);
            p1 = model.GetVertex(face.GetVertexIndex(1)-1);
            p2 = model.GetVertex(face.GetVertexIndex(2)-1);
            //Apply mvp on the vertices.
            glm::vec4 temp{p0,1};
            temp = mvp*temp;
            float w0 = temp.w;
            glm::vec3 fp0(temp);
            temp={p1,1};
            temp = mvp*temp;
            float w1 = temp.w;
            glm::vec3 fp1(temp);
            temp={p2,1};
            temp = mvp*temp;
            float w2 = temp.w;
            glm::vec3 fp2(temp);
            glm::vec3 color = CalculateFlatShading(scene.lights_, model, camera.GetCameraPosition(), mv, p0, p1, p2);
            DrawFlatShading(fp0 / w0, fp1 / w1, fp2 / w2, color);
        }
        if (shade == GOURAUD){
            DrawGouraudShading(scene.lights_, model, camera.GetCameraPosition(), view, projection);
        }
        if (scene.isVertexNormalToggle()) {
            DrawVNormal(scene, projection, view);
        }
        if (scene.isFaceNormalToggle()) {
            DrawFNormal(scene, projection, view);
        }
        if (scene.isBoundingBoxToggle()) {
            DrawBoundingBox(scene, projection, view, bounding_box);
        }
    }
}


/*void Renderer::DrawCameras(const Scene &scene, float width, float height) {
    if(scene.GetCameraCount()>1){
        glm::vec3 orange(1,0.5,0);
        int activeCameraIndex = scene.GetActiveCameraIndex();
        const Camera& camera = const_cast<Scene&>(scene).GetCamera(activeCameraIndex);
        glm::mat4 view_projection = PROJECTION(camera) * camera.GetViewTransformation();
        const MeshModel& camera_model = camera.GetCameraMesh();
        for (int i = 0; i < scene.GetCameraCount(); ++i) {
            if(i == activeCameraIndex)
                continue;
            const glm::mat4& camera_transform = const_cast<Scene&>(scene).GetCamera(i).GetCameraLoadTransform();
            const glm::mat4& view_projection_camera = view_projection * camera_transform;
            for (int j = 0; j < camera_model.GetFacesCount(); ++j) {
                const Face& face = camera_model.GetFace(i);
                const glm::vec3& p1 = camera_model.GetVertex(face.GetVertexIndex(0) - 1);
                const glm::vec3& p2 = camera_model.GetVertex(face.GetVertexIndex(1) - 1);
                const glm::vec3& p3 = camera_model.GetVertex(face.GetVertexIndex(2) - 1);
                // sadly can't use the overloaded operator* here, since i need to divide by w.

                glm::vec4 temp = {p1,1};
                temp = view_projection_camera * temp;
                temp /= temp.w;
                glm::vec2 fp1 = {temp.x,temp.y};

                temp = {p2,1};
                temp = view_projection_camera * temp;
                temp /= temp.w;
                glm::vec2 fp2 = {temp.x,temp.y};

                temp = {p3,1};
                temp = view_projection_camera * temp;
                temp /= temp.w;
                glm::vec2 fp3 = {temp.x,temp.y};

                *//*fp1-fp3 are in normalized device coordinates*//*

                glm::vec2 window_size(width,height) , offset = window_size/2.f;
                glm::ivec2 v1_window_pos = ((fp1 + 1.f) / 2.f) * window_size; *//*+ offset;*//*
                glm::ivec2 v2_window_pos = ((fp2 + 1.f) / 2.f) * window_size; *//*+ offset;*//*
                glm::ivec2 v3_window_pos = ((fp3 + 1.f) / 2.f) * window_size; *//*+ offset;*//*
                DrawLine(v1_window_pos,v2_window_pos,orange);
                DrawLine(v2_window_pos,v3_window_pos,orange);
                DrawLine(v1_window_pos,v3_window_pos,orange);
            }
        }
    }
}*/

void Renderer::DrawVNormal(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view){
    glm::vec3 green(0,1,0);
    const MeshModel& model = scene.GetActiveModel();
    const glm::mat4 mvp = projection*view*model.GetTransform();
    for (int j = 0; j < model.GetFacesCount(); ++j) {
        const Face &face = model.GetFace(j);
        for (int l = 0; l < 3; ++l) {
            glm::vec3 vertex = model.GetVertex(face.GetVertexIndex(l) - 1);
            glm::vec3 vertex_normal = model.GetNormal(face.GetNormalIndex(l) - 1);
            glm::vec4 temp = {vertex, 1};
            temp = model.GetTransform() * temp;
            vertex = {temp.x, temp.y, temp.z};
            temp = {vertex_normal, 1};
            temp = model.GetTransform() * temp;
            vertex_normal = {temp.x, temp.y, temp.z};
            vertex_normal = glm::normalize(vertex_normal) * scene.GetNormalVLen();
            glm::vec3 start = vertex;
            temp = {start, 1};
            temp = projection * view * temp;
            glm::vec2 fstart = {temp.x / temp.w, temp.y / temp.w};
            glm::vec3 end_point = start + vertex_normal;
            temp = {end_point, 1};
            temp = projection * view * temp;
            glm::vec2 fend = {temp.x / temp.w, temp.y / temp.w};
            glm::vec2 window_size(viewport_width_, viewport_height_);
            glm::ivec2 istart = ((fstart + 1.f) / 2.f) * window_size;
            glm::ivec2 iend = ((fend + 1.f) / 2.f) * window_size;
            DrawLine(istart, iend, green);
        }
    }
}

void Renderer::DrawFNormal(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view){
        const MeshModel& model = scene.GetActiveModel();
        glm::vec3 red(1,0,0);
        for (int j = 0; j < model.GetFacesCount(); ++j) {
            const Face& face = model.GetFace(j);
            glm::vec3 p1 = model.GetVertex(face.GetVertexIndex(0)-1);
            glm::vec3 p2 = model.GetVertex(face.GetVertexIndex(1)-1);
            glm::vec3 p3 = model.GetVertex(face.GetVertexIndex(2)-1);
            glm::vec4 temp = {p1,1};
            temp = model.GetTransform() * temp;
            p1 = {temp.x,temp.y,temp.z};
            temp = {p2,1};
            temp = model.GetTransform() * temp;
            p2 = {temp.x,temp.y,temp.z};
            temp = {p3,1};
            temp = model.GetTransform() * temp;
            p3 = {temp.x,temp.y,temp.z};
            // transformed triangle.
            glm::vec3 start = (p1+p2+p3)/3.f;
            // now it's the real starting point.
            glm::vec3 direction = glm::triangleNormal(p1,p2,p3)*scene.GetNormalFLen();
            glm::vec3 end_point = start+direction;
            temp = {start,1};
            temp = projection * view * temp;
            glm::vec2 fstart = {temp.x/temp.w,temp.y/temp.w};
            temp = {end_point,1};
            temp = projection * view * temp;
            glm::vec2 fend = {temp.x/temp.w,temp.y/temp.w};
            glm::vec2 window_size(viewport_width_,viewport_height_);
            glm::ivec2 istart = ((fstart+1.f)/2.f) * window_size;
            glm::ivec2 iend = ((fend+1.f)/2.f) * window_size;
            DrawLine(istart,iend,red);
        }


    }
void Renderer::DrawBoundingBox(const Scene &scene,const glm::mat4 &projection,const glm::mat4 &view, std::array<Edge,NUM_CUBE_EDGES>bounding_box){
    const MeshModel& model = scene.GetActiveModel();
    bounding_box = GetBoundingBox(model);
    glm::vec3 blue(0,0,1);
    for(const auto& edge : bounding_box){
        glm::vec3 start = edge.first;
        glm::vec3 end = edge.second;
        glm::vec4 temp = {start,1};
        temp = projection * view * temp;
        glm::vec2 fstart = {temp.x/temp.w,temp.y/temp.w};
        temp = {end,1};
        temp = projection * view * temp;
        glm::vec2 fend = {temp.x/temp.w,temp.y/temp.w};
        glm::vec2 window_size(viewport_width_,viewport_height_);
        glm::ivec2 istart = ((fstart+1.f)/2.f) * window_size;
        glm::ivec2 iend = ((fend+1.f)/2.f) * window_size;
        DrawLine(istart,iend,blue);
    }
}

glm::mat4 Renderer::GetViewTransform(const Camera &camera) const {
    return camera.GetViewTransformation();
}

glm::mat4 Renderer::GetProjectionTransform(const Camera &camera) {
    return camera.GetProjectionType() == ORTHOGRAPHIC ? camera.GetOrthographicProjection() : camera.GetPerspectiveProjection();
}

glm::mat4 Renderer::GetViewPortTransform() const {
    float vl(0),vb(0),vr(viewport_width_),vt(viewport_height_);
    glm::vec4 c1,c2,c3,c4;
    c1 = {(vr-vl)/2,0,0,0};
    c2 = {0,(vt-vb)/2,0,0};
    c3 = {0,0,0.5,0};
    c4 = {(vr+vl)/2,(vt+vb)/2,0.5,1};
    return {c1,c2,c3,c4};
}

void Renderer::DrawTriangle(const glm::vec2& p0,const glm::vec2& p1,const glm::vec2& p2 , const glm::vec3& color) {
    const glm::vec2 ptr[3]={p0,p1,p2};
    float min_x,max_x,min_y,max_y;
    ComputeBB(ptr,min_x,max_x,min_y,max_y);
    for (int i = min_x; i < max_x; ++i) {
        for (int j = min_y; j < max_y; ++j) {
            if (IsInTriangle(p0,p1,p2,glm::vec2(i,j)))
                PutPixel(i,j,color);
        }
    }
}

void Renderer::ComputeBB(const glm::vec2 *p, float &min_x, float &max_x, float &min_y, float &max_y) const {
    min_x = max_x = p[0].x;
    min_y = max_y = p[0].y;
    for (int i = 1; i < 3; ++i) {
        if (min_x > p[i].x)
            min_x = p[i].x;
        if (max_x < p[i].x)
            max_x = p[i].x;
        if (min_y > p[i].y)
            min_y = p[i].y;
        if (max_y < p[i].y)
            max_y = p[i].y;
    }
    if (min_x < 0)
        min_x = 0;
    if (min_y < 0)
        min_y = 0;
    if (max_x > viewport_width_)
        max_x = viewport_width_-1;
    if (max_y > viewport_height_)
        max_y = viewport_height_-1;
}

float Renderer::DecideHalfPlane(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2) const {
    return (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
}

bool Renderer::IsInTriangle(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p) const {
    float side1, side2, side3;
    bool neg, pos;
    side1 = DecideHalfPlane(p, p0, p1);
    side2 = DecideHalfPlane(p, p1, p2);
    side3 = DecideHalfPlane(p, p2, p0);
    neg = side1 < 0 || side2 < 0 || side3 < 0;
    pos = side1 > 0 || side2 > 0 || side3 > 0;
    return !(neg && pos);       //even if it's outside of one of the planes and on the inner side of the other,return false.
}

void Renderer::SetViewportSizes(int w, int h) {
    delete[] color_buffer_;
    viewport_height_ = h;
    viewport_width_ = w;
    color_buffer_ = new float[3 * w * h];
}

void Renderer::ResizeZBuffer(int w, int h) {
    zb.Resize(w,h);
}


// Assumes the points are pre-viewport transformation and after projection.
void Renderer::DrawFlatShading(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &color) {
    float min_x,max_x,min_y,max_y;
    glm::vec2 p0after_viewport,p1after_viewport,p2after_viewport;
    glm::mat4 viewport_transform = GetViewPortTransform();
    glm::vec4 temp = {p0,1};
    temp = viewport_transform * temp;
    p0after_viewport = temp / temp.w;
    temp = {p1, 1};
    temp = viewport_transform * temp;
    p1after_viewport = temp / temp.w;
    temp = {p2, 1};
    temp = viewport_transform * temp;
    p2after_viewport = temp / temp.w;
    glm::vec2 ptr[3]={p0after_viewport,p1after_viewport,p2after_viewport};
    ComputeBB(ptr,min_x,max_x,min_y,max_y);
    for (int i = min_x; i < max_x; ++i) {
        for (int j = min_y; j < max_y; ++j) {
            if (IsInTriangle(p0after_viewport,p1after_viewport,p2after_viewport,{i,j})){
                float z = ZLinearInterpolation(p0,p1,p2,{i,j});
                if(zb.TestAndSetAt(i,j,z)){
                    if (zb.IsDrawDepth())
                        PutPixel(i,j,glm::vec3(zb.NormalizeGreyScale(i, j)));
                    else{
                        PutPixel(i,j,color);
                    }
                }
            }
        }
    }
}

float Renderer::ComputeTriangleArea(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2) const {
    glm::vec3 x(p0.x,p1.x,p2.x),ydiff(p1.y-p2.y,p2.y-p0.y,p0.y-p1.y);
    return abs(glm::dot(x,ydiff)/2);
}

float Renderer::ZLinearInterpolation(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec2 &p) const {
    float area1,area2,area3,tot_area;
    area1 = ComputeTriangleArea(p1,p2,p);
    area2 = ComputeTriangleArea(p0,p2,p);
    area3 = ComputeTriangleArea(p0,p2,p);
    tot_area = area1+area2+area3;
    if (tot_area == 0)
        return 0;
    glm::vec3 z(p0.z,p1.z,p2.z),rel_area(area1/tot_area,area2/tot_area,area3/tot_area);
    return -glm::dot(rel_area,z);
}

/*
void Renderer::DrawLightsModels(Scene &scene) {
    const std::vector<PointLight>& point_lights = scene.lights_.point_input_;
    Camera &camera = scene.GetActiveCamera();
    zb.SetDrawDepth(false);
    int point_count = point_lights.size();
    const glm::mat4 view = GetViewTransform(camera);
    const glm::mat4 projection = GetProjectionTransform(camera);
    const glm::mat4 viewport_transform = GetViewPortTransform();
    for (int i = 0; i < point_count; ++i) {
        const PointLight& pointLight = point_lights[i];
        const MeshModel& light_model = PointLight::getLightMesh();
        glm::mat4 mvp = projection * view * pointLight.getLightTransform();
        int face_count = light_model.GetFacesCount();
        for (int j = 0; j < face_count; ++j) {
            const Face& face = light_model.GetFace(j);
            const glm::vec3& p0 = light_model.GetVertex(face.GetVertexIndex(0)-1);
            const glm::vec3& p1 = light_model.GetVertex(face.GetVertexIndex(1)-1);
            const glm::vec3& p2 = light_model.GetVertex(face.GetVertexIndex(2)-1);
            glm::vec4 temp{p0,1};
            temp = mvp*temp;
            glm::vec3 fp0(temp/temp.w);
            temp={p1,1};
            temp = mvp*temp;
            glm::vec3 fp1(temp/temp.w);
            temp={p2,1};
            temp = mvp*temp;
            glm::vec3 fp2(temp/temp.w);
            DrawFlatShading(fp0, fp1, fp2, {1, 1, 0}); // yellow for point light;
        }
    }
    zb.SetDrawDepth(scene.isDrawDepthToggle());
}
*/

// Camera/face vertices positions are before transformations.
/*glm::vec3 Renderer::CalculateFlatShading(const std::vector<PointLight> &point_lights,
                                         const std::vector<ParallelLight> &parallel_lights, const MeshModel &meshModel,
                                         const glm::vec3 &camera_pos, const glm::mat4 &model_view, const glm::vec3 &p0,
                                         const glm::vec3 &p1, const glm::vec3 &p2) {
//    glm::mat4 mvp = projection * model_view;
    glm::vec3 pixel_rgb(0,0,0);

    glm::vec3 normal = glm::triangleNormal(p0,p1,p2);
    normal = camera_pos.z<0 ? GetTransformedNormal(normal,model_view) : -GetTransformedNormal(normal,model_view);
    glm::vec3 t_cam_pos = TransformAndDivideW(model_view, camera_pos);

    glm::vec3 t_p0(p0),t_p1(p1),t_p2(p2);
    t_p0 = TransformAndDivideW(model_view, p0);
    t_p1 = TransformAndDivideW(model_view, p1);
    t_p2 = TransformAndDivideW(model_view, p2);
    glm::vec3 point_pos((t_p0+t_p1+t_p2)/3.f);

    const glm::vec3& ambient_color = meshModel.getAmbientColor();
    const glm::vec3& diffuse_color = meshModel.getDiffuseColor();
    const glm::vec3& specular_color = meshModel.getSpecularColor();

    for (const auto& point_light: point_lights){
        pixel_rgb+= point_light.CalcTotalLight(ambient_color,diffuse_color,specular_color,normal,point_pos,t_cam_pos,model_view);
    }
    for(const auto& parallel_light: parallel_lights){
        pixel_rgb+= parallel_light.CalcTotalLight(ambient_color, diffuse_color, specular_color, normal, point_pos,
                                                  t_cam_pos, model_view);
    }
    for (int i = 0; i < 3; ++i) {
        if (pixel_rgb[i] > 1)
            pixel_rgb[i] = 1;
    }
    return pixel_rgb;
}*/

glm::vec3
Renderer::GetTransformedNormal(const glm::vec3 &normal, const glm::mat4 &model_view, const glm::mat4 &projection) {
    glm::mat4 normal_transform = glm::transpose(glm::inverse(model_view));
    glm::vec4 normal4{normal,1};
    normal4 = projection * normal4;
    normal4 /= normal4.w;
    assert(normal4.w == 1);
    return glm::normalize(glm::vec3{normal4.x,normal4.y,normal4.z});
}

glm::vec3 Renderer::TransformAndDivideW(const glm::mat4 &mvp, const glm::vec3 &vertex) {
    glm::vec4 temp{vertex,1};
    temp = mvp * temp;
    if (temp.w != 0){
        temp/=temp.w;
    }
    return {temp.x,temp.y,temp.z};
}


glm::vec3 Renderer::CalculateFlatShading(const Lights &lights, const MeshModel &meshModel, const glm::vec3 &cam_pos,
                                         const glm::mat4 &model_view, const glm::vec3 &p0, const glm::vec3 &p1,
                                         const glm::vec3 &p2) {
    const glm::vec3& mesh_ambient = meshModel.getAmbientColor();
    const glm::vec3& mesh_diffuse = meshModel.getDiffuseColor();
    const glm::vec3& mesh_specular = meshModel.getSpecularColor();
    const float shininess = meshModel.getShininess();

    glm::vec3 normal = glm::triangleNormal(p0,p1,p2); // pre-transformation
    normal = GetTransformedNormal(normal,model_view);

    //now need to transform the points.

    glm::vec3 t_p0 = TransformAndDivideW(model_view,p0);
    glm::vec3 t_p1 = TransformAndDivideW(model_view,p1);
    glm::vec3 t_p2 = TransformAndDivideW(model_view,p2);

    glm::vec3 pos = (t_p0+t_p1+t_p2)/3.f;

    glm::vec3 final_color{0,0,0};
    for(const auto& pointLight: lights.point_input_){
        final_color += pointLight.CalcColorAtPoint(mesh_ambient, mesh_diffuse, mesh_specular, shininess, pos, normal,
                                                   cam_pos, model_view);
    }
    for(const auto& parallelLight: lights.parallel_input_){
        final_color+= parallelLight.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,pos,normal,cam_pos);
    }
    for (int i = 0; i < 3; ++i) {
        final_color[i] = final_color[i] <= 1.f ? final_color[i] : 1.f;
        assert(final_color[i] >= 0.f);
    }
    return final_color;
}

void Renderer::DrawGouraudShading(const Lights &lights, const MeshModel &meshModel, const glm::vec3 &cam_pos,
                                  const glm::mat4 &view, const glm::mat4 &projection) {
    int face_count = meshModel.GetFacesCount();
    const glm::mat4 mv = view * meshModel.GetTransform();
    const glm::mat4 mvp = projection * mv;
    const glm::vec3& mesh_ambient = meshModel.getAmbientColor();
    const glm::vec3& mesh_diffuse = meshModel.getDiffuseColor();
    const glm::vec3& mesh_specular = meshModel.getSpecularColor();
    const float shininess = meshModel.getShininess();
    for (int i = 0; i < face_count; ++i) {
        glm::vec3 final_intensity_p0{0,0,0},final_intensity_p1{0,0,0},final_intensity_p2{0,0,0};
        const Face& face = meshModel.GetFace(i);
        glm::vec3 p0,p1,p2;
        p0 = meshModel.GetVertex(face.GetVertexIndex(0)-1);
        p1 = meshModel.GetVertex(face.GetVertexIndex(1)-1);
        p2 = meshModel.GetVertex(face.GetVertexIndex(2)-1);
        p0 = TransformAndDivideW(mv,p0);
        p1 = TransformAndDivideW(mv,p1);
        p2 = TransformAndDivideW(mv,p2);
        glm::vec3 n0,n1,n2;
        n0 = meshModel.GetNormal(face.GetNormalIndex(0)-1);
        n1 = meshModel.GetNormal(face.GetNormalIndex(1)-1);
        n2 = meshModel.GetNormal(face.GetNormalIndex(2)-1);
        n0 = GetTransformedNormal(n0,mv);
        n1 = GetTransformedNormal(n1,mv);
        n2 = GetTransformedNormal(n2,mv);
        for(const auto& point_light: lights.point_input_){
            final_intensity_p0+=point_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p0,n0,cam_pos,mv);
            final_intensity_p1+=point_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p1,n1,cam_pos,mv);
            final_intensity_p2+=point_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p2,n2,cam_pos,mv);
        }
        for(const auto& parallel_light: lights.parallel_input_){
            final_intensity_p0+=parallel_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p0,n0,cam_pos);
            final_intensity_p1+=parallel_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p1,n1,cam_pos);
            final_intensity_p2+=parallel_light.CalcColorAtPoint(mesh_ambient,mesh_diffuse,mesh_specular,shininess,p2,n2,cam_pos);
        }
        for (int j = 0; j < 3; ++j) {
            final_intensity_p0[j] = final_intensity_p0[j] <= 1.f ? final_intensity_p0[j] : 1.f;
            final_intensity_p1[j] = final_intensity_p1[j] <= 1.f ? final_intensity_p1[j] : 1.f;
            final_intensity_p2[j] = final_intensity_p2[j] <= 1.f ? final_intensity_p2[j] : 1.f;
        }
        //now interpolate over the colors...
        float min_x,max_x,min_y,max_y;
        glm::vec3 p0_after_projection(p0);
        p0_after_projection = TransformAndDivideW(projection, p0_after_projection);
        glm::vec3 p1_after_projection(p1);
        p1_after_projection = TransformAndDivideW(projection, p1_after_projection);
        glm::vec3 p2_after_projection(p2);
        p2_after_projection = TransformAndDivideW(projection, p2_after_projection);

        glm::mat4 viewport_transform = GetViewPortTransform();
        glm::vec2 p0_after_viewport = TransformAndDivideW(viewport_transform,p0_after_projection);
        glm::vec2 p1_after_viewport = TransformAndDivideW(viewport_transform,p1_after_projection);
        glm::vec2 p2_after_viewport = TransformAndDivideW(viewport_transform,p2_after_projection);
        glm::vec2 ptr[3]={p0_after_viewport,p1_after_viewport,p2_after_viewport};
        ComputeBB(ptr,min_x,max_x,min_y,max_y);

        for (int i = min_x; i < max_x; ++i) {
            for (int j = min_y; j < max_y; ++j) {
                if (IsInTriangle(p0_after_viewport,p1_after_viewport,p2_after_viewport,{i,j})){
                    float z = ZLinearInterpolation(p0_after_projection,p1_after_projection,p2_after_projection,{i,j});
                    if(zb.TestAndSetAt(i,j,z)){
                        if (zb.IsDrawDepth())
                            PutPixel(i,j,glm::vec3(zb.NormalizeGreyScale(i, j)));
                        else{
                            glm::vec3 color = ColorIntensityInterpolation(p0_after_projection, p1_after_projection,
                                                                          p2_after_projection, final_intensity_p0,
                                                                          final_intensity_p1, final_intensity_p2,
                                                                          {i, j});
                            PutPixel(i,j,color);
                        }
                    }
                }
            }
        }
    }
}

glm::vec3 Renderer::ColorIntensityInterpolation(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
                                                const glm::vec3 &c0, const glm::vec3 &c1, const glm::vec3 &c2,
                                                const glm::vec2 &p) const {
    float area1,area2,area3,tot_area;
    area1 = ComputeTriangleArea(p1,p2,p);
    area2 = ComputeTriangleArea(p0,p2,p);
    area3 = ComputeTriangleArea(p0,p2,p);
    tot_area = area1+area2+area3;
//    if (tot_area == 0)
//        return {0,0,0};
    glm::vec3 c(c0.r,c1.r,c2.r),rel_area(area1/tot_area,area2/tot_area,area3/tot_area);
    float red = glm::dot(rel_area,c);
    c = {c0.g,c1.g,c2.g};
    float green = glm::dot(rel_area,c);
    c = {c0.b,c1.b,c2.b};
    float blue = glm::dot(rel_area,c);
    return {red,green,blue};
}


/*glm::vec3 Renderer::CalculateGouraudShading(const VertexAttribs &vertexAttribs, const glm::vec3 &pos) {
    std::array<glm::vec3,3> vertices = vertexAttribs.GetVertices();
    std::array<glm::vec3,3> colors = vertexAttribs.GetVertexColors();

    float area1 = ComputeTriangleArea(vertices[1],vertices[2],pos);
    float area2 = ComputeTriangleArea(vertices[0],vertices[2],pos);
    float area3 = ComputeTriangleArea(vertices[1],vertices[0],pos);

    float tot_area = area1+area2+area3;

    float red = glm::dot(glm::vec3(area1,area2,area3)/tot_area,glm::vec3(colors[0].x,colors[1].x,colors[2].x));
    float green = glm::dot(glm::vec3(area1,area2,area3)/tot_area,glm::vec3(colors[0].y,colors[1].y,colors[2].y));
    float blue = glm::dot(glm::vec3(area1,area2,area3)/tot_area,glm::vec3(colors[0].z,colors[1].z,colors[2].z));

    return glm::vec3(red,green,blue);
}*/

Edge operator*(const glm::mat4& mat, const Edge& start_end) {
    glm::vec3 start = start_end.first, end = start_end.second;
    glm::vec4 temp = {start, 1};
    temp = mat * temp;
    start = {temp[0],temp[1],temp[2]};
    temp = {end, 1};
    temp = mat * temp;
    end = {temp[0],temp[1],temp[2]};
    return std::make_pair(start,end);
}

glm::vec3 operator*(const glm::mat4 &mat, const glm::vec3 &vec) {
    glm::vec4 converter = {vec,1};
    converter = mat * converter;
    return glm::vec3(converter[0],converter[1],converter[2]);
}

void Renderer::LoadShaders()
{
    colorShader.loadShaders("vshader.glsl", "fshader.glsl");
}

void Renderer::LoadTextures()
{
    if (!texture1.loadTexture(R"(C:\Users\Dany\CLionProjects\computergraphics2021-dp\Data\spongebob.png)", true))
    {
        texture1.loadTexture(R"(C:\Users\Dany\CLionProjects\computergraphics2021-dp\Data\spongebob.png)", true);
    }
}
