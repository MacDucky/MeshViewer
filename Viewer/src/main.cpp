#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <algorithm>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;

bool show_model_window = false;
bool show_camera_window = false;
bool show_z_menu = false;
bool show_lighting_window = false;
glm::vec4 clear_color = glm::vec4(0.15f, 0.15f, 0.15f, 1.00f);

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{
	int windowWidth = 1280, windowHeight = 720;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
    renderer.LoadShaders();
    renderer.LoadTextures();
	Scene scene = Scene();
	
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
    }

	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
        renderer.SetViewportSizes(frameBufferWidth,frameBufferHeight);
        renderer.ResizeZBuffer(frameBufferWidth,frameBufferHeight);
	}

    static float lookat_x;
    static float lookat_y;
    //TODO : update those at lookat for cameras_input_from_GUI
    if (!io.WantCaptureKeyboard)
	{

        // TODO: Handle keyboard events here
        if (io.KeysDown[120])
		{
            if (ImGui::IsMousePosValid()) {
                lookat_x = io.MousePos.x;
            }
        }
        if(io.KeysDown[121]){
            if (ImGui::IsMousePosValid())
                lookat_y = io.MousePos.y;

		}
	}

	if (!io.WantCaptureMouse)
	{
		// TODO: Handle mouse events here
		if (io.MouseDown[0])
		{
			// Left mouse button is down
		}
	}

//	renderer.ClearColorBuffer(clear_color);
    glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);  // These replace the Renderer::ClearColorBuffer(color)
	glEnable(GL_DEPTH_TEST);    // These replace the Renderer::ClearColorBuffer(color)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // These replace the Renderer::ClearColorBuffer(color)
    renderer.Render(scene);
//	renderer.SwapBuffers(); //basically this says to opengl to draw the color_buffer onto the screen.

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
    static std::vector<std::string> loaded_model_names; //list of models
    static std::vector<int> cameras; // list of cameras_input_from_GUI
    static MInput model_bars; // holds 6 bars for each model.
    static CameraBars cameraBars; //holds all the information for each camera;
    static ZBufferInput iZBuff;
    static ModelColorsInput modelColorsInput;
	/**
	 * MeshViewer menu
	 */
	ImGui::Begin("MeshViewer Menu");
	
	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
            if (ImGui::MenuItem("Open", "CTRL+O")) {
                nfdchar_t *outPath = NULL;
                nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
                if (result == NFD_OKAY) {
                    bool is_added = false;
                    scene.AddModel(Utils::LoadMeshModel(outPath)); // load the meshmodel to the scene.
                    //***********************************************************************************************************
                    std::string file_name = Utils::GetFileName(outPath);
                    std::string name = file_name.substr(0, file_name.find(".")); // filter name out
                    for (auto iterator = loaded_model_names.rbegin(); iterator != loaded_model_names.rend(); ++iterator) {
                        std::string loaded_name = (*iterator).substr(0, (*iterator).find_first_of(
                                "0123456789")); // mode_namel0 -> model_name
                        if (loaded_name == name) { // if we found another instance of the model
                            int model_num = std::stoi((*iterator).substr((*iterator).find_first_of("0123456789"),
                                                                         std::string::npos)); //model_name0 -> 0
                            std::string final_str = name + std::to_string(model_num + 1); //next mode name
                            loaded_model_names.push_back(final_str); // new model_name
                            is_added = true;
                            break;
                        }
                    }
                    // the object was not found in the container
                    if (!is_added)
                        loaded_model_names.push_back(name + "1");
                    //***********************************************************************************************************
                    model_bars.AddBars(); // new set of bars for the new model.
                    modelColorsInput.AddBars();
                    free(outPath);
                }
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
		    if (ImGui::MenuItem("Model"))
		    {
                show_model_window = !show_model_window;
		    }
            if(ImGui::MenuItem("Camera")){
                show_camera_window = !show_camera_window;
            }

            if(ImGui::MenuItem("ZBuffer")){
                show_z_menu = !show_z_menu;
            }

            if (ImGui::MenuItem("Lighting")){
                show_lighting_window = !show_lighting_window;
            }

            ImGui::EndMenu();
		}
		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
	// TODO: Add more controls as needed

	// Model & World transformation input
	if(show_model_window){
        ImGui::Begin("Model", &show_model_window);
        {
            static bool vn,fn,bb;
            static int selected = -1;   //current selection of model
            if (ImGui::TreeNode("Select active model:"))
            {
                for (int n = 0; n < loaded_model_names.size(); n++)
                {
                    char buf[32];
                    sprintf(buf, loaded_model_names[n].c_str(), n);
                    if (ImGui::Selectable(buf, selected == n))
                        selected = n;
                }
                ImGui::TreePop();
            }
            glm::vec3& mt = model_bars.GetMT(selected);
            glm::vec3& mr = model_bars.GetMR(selected);
            glm::vec3& ms = model_bars.GetMS(selected);
            glm::vec3& wt = model_bars.GetWT(selected);
            glm::vec3& wr = model_bars.GetWR(selected);
            glm::vec3& ws = model_bars.GetWS(selected);
            float& vnlen = model_bars.GetVNLen(selected);
            float& fnlen = model_bars.GetFNLen(selected);
            bool bars_is_changed = false;
            bool status;
            if (selected != -1)
            {
                ImGui::Text("Model Transformations:");
                status = ImGui::DragFloat("MTranslate X", &mt.x, 0.01f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MTranslate Y", &mt.y, 0.01f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MTranslate Z", &mt.z, 0.01f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("MRotate X",&mr.x);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("MRotate Y", &mr.y);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("MRotate Z", &mr.z);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MScale X", &ms.x, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MScale Y", &ms.y, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MScale Z", &ms.z, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("MUniform scale", &ms.x, 0.1f);
                if(status)
                    ms.y = ms.z = ms.x;
                bars_is_changed = status || bars_is_changed;
                ImGui::Text("World Transformations:");
                status = ImGui::DragFloat("WTranslate X", &wt.x, 0.5f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WTranslate Y", &wt.y, 0.5f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WTranslate Z", &wt.z, 0.5f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("WRotate X", &wr.x);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("WRotate Y", &wr.y);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::SliderAngle("WRotate Z", &wr.z);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WScale X", &ws.x, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WScale Y", &ws.y, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WScale Z", &ws.z, 0.1f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("WUniform scale", &ws.x, 0.1f);
                if(status)
                    ws.y = ws.z = ws.x;
                bars_is_changed = status || bars_is_changed;
            }
            if(ImGui::Button("Reset Transformations") && scene.GetModelCount()>0){
                MeshModel& model = scene.GetActiveModel();
                model.ResetTransformations();
                for (int i = 0; i < 3; ++i) {
                    mt[i]=mr[i]=wr[i]=wt[i] = 0;
                    ms[i]=ws[i]=1;
                }
                bars_is_changed = true;
            }
            bool checkbox_is_changed = false;

            checkbox_is_changed = ImGui::Checkbox("Draw vertices normals", &vn);
            if(vn){
                status = ImGui::DragFloat("VNormal Len", &vnlen, 0.01f,0.001,100);
                bars_is_changed = status || bars_is_changed;
            }

            checkbox_is_changed = checkbox_is_changed || ImGui::Checkbox("BB", &bb);

            checkbox_is_changed = checkbox_is_changed || ImGui::Checkbox("Draw face normals", &fn);
            if(fn){
                status = ImGui::DragFloat("FNormal Len", &fnlen, 0.01f,0.001,100);
                bars_is_changed = status || bars_is_changed;
            }
            if (selected != -1) {
                static float ambientf[3], diffusef[3], specularf[3], shininessf;
                status = ImGui::ColorEdit3("Ambient Color", ambientf, ImGuiColorEditFlags_Float);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::ColorEdit3("Diffuse Color", diffusef, ImGuiColorEditFlags_Float);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::ColorEdit3("Specular Color", specularf, ImGuiColorEditFlags_Float);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("Shininess", &shininessf, 0.25, 0.f, 128.f);
                bars_is_changed = status || bars_is_changed;

                if (bars_is_changed) {
                    for (int i = 0; i < 3; ++i) {
                        modelColorsInput.ambient_[selected][i] = ambientf[i];
                        modelColorsInput.diffuse_[selected][i] = diffusef[i];
                        modelColorsInput.specular_[selected][i] = specularf[i];
                    }
                    modelColorsInput.shininess_[selected] = shininessf;
                    scene.SetModelColors(selected, modelColorsInput.ambient_[selected],
                                         modelColorsInput.diffuse_[selected],
                                         modelColorsInput.specular_[selected], modelColorsInput.shininess_[selected]);
                }

            }
            if ((selected != -1) && (checkbox_is_changed || bars_is_changed)) {
                scene.SetModelInputFromGui(model_bars,selected,vn,fn,bb);
            }
        }
        ImGui::End();
	}

	if(show_camera_window){
        ImGui::Begin("Camera", &show_camera_window);
        static int selected = -1;   //current selection of camera
        if(ImGui::Button("Add camera")){
            cameras.push_back(cameras.size());   // new camera index
            cameraBars.AddBars();   // set of bars for the new camera

            glm::vec3 vpos(0, 0, 0);
            glm::vec3 vat(0,0,1);
            glm::vec3 vup(0, 1, 0);
            scene.AddCamera(std::make_shared<Camera>(vpos,vat,vup));
        }
        if (ImGui::TreeNode("Select active camera:"))
        {
            for (int n = 0; n < cameras.size(); n++)
            {
                char buf[32];
                std::string str = "camera"+std::to_string(cameras[n]+1);
                sprintf(buf, str.c_str(), n);
                if (ImGui::Selectable(buf, selected == n))
                    selected = n;
            }
            ImGui::TreePop();
        }
        bool bars_is_changed = false;
        bool status;
        if(selected != -1){
            cameraBars.SetActiveCam(selected);
            scene.SetActiveCameraIndex(selected);
            glm::vec3& pos = cameraBars.GetPosition(selected);
            glm::vec3& look_at = cameraBars.GetAt(selected);
            glm::vec3& up = cameraBars.GetUp(selected);
            if (ImGui::TreeNode("Camera Translation:"))
            {
                status = ImGui::DragFloat("pos X", &pos.x, 0.05);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("pos Y", &pos.y, 0.05);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("pos Z", &pos.z, 0.25f);
                bars_is_changed = status || bars_is_changed;
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Camera Look At:"))
            {
                status = ImGui::DragFloat("at X", &look_at.x, 0.05f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("at Y", &look_at.y, 0.05);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("at Z", &look_at.z, 0.05);
                bars_is_changed = status || bars_is_changed;
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Camera Up:"))
            {
                status = ImGui::DragFloat("up X", &up.x, 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("up Y", &up.y, 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("up Z", &up.z, 0.5f);
                bars_is_changed = status || bars_is_changed;
                ImGui::TreePop();
            }

            static bool cam_mode = cameraBars.GetCamMode(selected); // false for Orthographic , true for Perspective
            bool prev_cam_mode = cam_mode;
            if (ImGui::RadioButton("Orthographic", !cam_mode)) { cam_mode = false; }
            ImGui::SameLine();
            if (ImGui::RadioButton("Perspective", cam_mode)) { cam_mode = true; }
            cameraBars.SetCamMode(cam_mode,selected);
            if(prev_cam_mode != cam_mode){
                bars_is_changed = true;
            }
            if (!cam_mode) { //orthographic
                glm::vec4& ortho = cameraBars.GetOrthographic(selected);
                status = ImGui::DragFloat("Left", &ortho[0], 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("Right", &ortho[1], 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("Bottom", &ortho[2], 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("Top", &ortho[3], 1.0f);
                bars_is_changed = status || bars_is_changed;
            } else { //perspective
                glm::vec3& perspect = cameraBars.GetPerspective(selected);
                status = ImGui::DragFloat("fovY", &perspect[0], 1.0f,0.1f,179.9,"%.0f");
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("width", &perspect[1], 1.0f);
                bars_is_changed = status || bars_is_changed;
                status = ImGui::DragFloat("height", &perspect[2], 1.0f);
                bars_is_changed = status || bars_is_changed;
            }
            if(bars_is_changed){
                scene.SetCameraInputFromGui(cameraBars);
            }
            scene.SetActiveCameraFromGui(cameraBars);
        }
        ImGui::End();

	}

    if(show_z_menu){
        ImGui::Begin("ZBuffer", &show_z_menu);
        ImGui::Checkbox("Draw Depth",&iZBuff.DrawGetRef());
        scene.setDrawDepthToggle(iZBuff.DrawGetRef());
        ImGui::End();
    }

    if(show_lighting_window){
        static int selected_pt = -1 , selected_pl = -1;
        ImGui::Begin("Lighting" , &show_lighting_window);
        static int point_num=0,parallel_num=0;
        if(ImGui::Button("Add Point Light")){
            scene.lights_.AddPointBars();
            ++point_num;
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Parallel Light")){
            scene.lights_.AddParallelBars();
            ++parallel_num;
        }

        if (ImGui::TreeNode("Select active point light:"))
        {
            for (int n = 0; n < point_num; n++)
            {
                char buf[32];
                std::string str = "Point Light"+std::to_string(n+1);
                sprintf(buf, str.c_str(), n);
                if (ImGui::Selectable(buf, selected_pt == n))
                    selected_pt = n;
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Select active parallel light:"))
        {
            for (int n = 0; n < parallel_num; n++)
            {
                char buf[32];
                std::string str = "Parallel Light"+std::to_string(n+1);
                sprintf(buf, str.c_str(), n);
                if (ImGui::Selectable(buf, selected_pl == n))
                    selected_pl = n;
            }
            ImGui::TreePop();
        }

        if (selected_pt != -1 && ImGui::TreeNode("Set properties to active point light:")) {
            PointLight &active_point_light = scene.lights_.getPointLight(selected_pt);
            glm::vec3 &ambient = active_point_light.getAmbientIntensity();
            glm::vec3 &diffuse = active_point_light.getDiffuseIntensity();
            glm::vec3 &specular = active_point_light.getSpecularIntensity();
            glm::vec3 &position = active_point_light.getPosition();


            ImGui::ColorEdit3("Ambient", (float *) &ambient, ImGuiColorEditFlags_Float);
            ImGui::ColorEdit3("Diffuse", (float *) &diffuse, ImGuiColorEditFlags_Float);
            ImGui::ColorEdit3("Specular", (float *) &specular, ImGuiColorEditFlags_Float);
            static float pos_sensitivity = 0.005;
            ImGui::DragFloat("Pos X", (float *) &position.x, pos_sensitivity);
            ImGui::DragFloat("Pos Y", (float *) &position.y, pos_sensitivity);
            ImGui::DragFloat("Pos Z", (float *) &position.z, pos_sensitivity);
            ImGui::SliderFloat("Pos drag sensitivity",&pos_sensitivity,0.001,5);
            ImGui::TreePop();
        }
        if (selected_pl != -1 && ImGui::TreeNode("Set properties to active parallel light:")) {
            ParallelLight &active_parallel_light = scene.lights_.getParallelLight(selected_pl);
            glm::vec3 &ambient = active_parallel_light.getAmbientIntensity();
            glm::vec3 &diffuse = active_parallel_light.getDiffuseIntensity();
            glm::vec3 &specular = active_parallel_light.getSpecularIntensity();
            glm::vec3 &direction = active_parallel_light.getLightDirection();

            ImGui::ColorEdit3("Ambient", (float *) &ambient, ImGuiColorEditFlags_Float);
            ImGui::ColorEdit3("Diffuse", (float *) &diffuse, ImGuiColorEditFlags_Float);
            ImGui::ColorEdit3("Specular", (float *) &specular, ImGuiColorEditFlags_Float);
            ImGui::DragFloat("Direction X", (float *) &direction.x, 0.005f);
            ImGui::DragFloat("Direction Y", (float *) &direction.y, 0.005f);
            ImGui::DragFloat("Direction Z", (float *) &direction.z, 0.005f);

            ImGui::TreePop();
        }
        ShadeMode& shadeMode = scene.getShadeMode();
        static bool flat=false,gouroud=false,phong=true,toon=false;
        if (ImGui::Checkbox("Flat",&flat)){
            shadeMode = FLAT;
            gouroud = false;
            phong = false;
            toon = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Gouroud",&gouroud)){
            shadeMode = GOURAUD;
            flat = false;
            phong = false;
            toon = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Phong",&phong)){
            shadeMode = PHONG;
            flat = false;
            gouroud = false;
            toon = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Toon",&toon)){
            shadeMode = TOON;
            flat = false;
            gouroud = false;
            phong = false;
        }
        if(toon){
            ImGui::SliderInt("# of colors",&scene.getToonColors(),2,8);
        }
        ImGui::End();
    }

	ImGui::End();
	/**
	 * Imgui demo - you can remove it once you are familiar with imgui
	 */
	
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}