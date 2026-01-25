#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <chrono>
#include <thread>

#include <assimp/config.h>

#include <imgui/imgui.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_opengl3_loader.h>

#include "./Position.h"
#include "./Player.h"
#include "./Model.h"
#include "./SkyBox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <iostream>

#include "./CubeFileParser.h"
#include <fstream>

#include "RenderObject.h"

#include "FrameBuffer.h"

#include "./Scene.h"

#define CUBE_FILE false

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void toggleWireFrame();
bool keyIsHeld(int);
void setKeyIsHeld(int);
void resetKeyIsHeld(int);

std::vector<int> heldKeys; 

static const float frameTime = 0.008f;

// settings
unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 1200;

bool wireframe = false;

bool show_demo_window = false;

bool resizeWindow = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float heightScale = 0.1f;
bool parallax = true;
bool firstMouse = true;

Player player(camera);

Scene* scene;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float currentFrame = static_cast<float>(glfwGetTime());



int main()
{   
    
    if (CUBE_FILE) {
        std::string filename{ "C:\\Users\\chris\\Pictures\\base_lut_1.colorgradinginput.cube"};
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "cubefile could not be opened" << std::endl;
        }
        else {
            std::vector<glm::vec3> cubeColors = parse(file);
        }
        return 0;
    }
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH * main_scale, SCR_HEIGHT * main_scale, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // build and compile our shader zprogram
    // ------------------------------------
    Shader model_loading_shader("shader/model_loading.vs", "shader/model_loading.fs");
    Shader lightShader("shader/lightCubeShader.vs", "shader/lightCubeShader.fs");
	Shader fbx_shader("shader/fbx_shader.vs", "shader/fbx_shader.fs");
    Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");
	Shader fbo_shader("shader/fbo_shader.vs", "shader/fbo_shader.fs");
	Shader boxShader("shader/boxShader.vs", "shader/boxShader.fs");
	Shader terrainShader("shader/terrain.vs", "shader/terrain.fs");

	Assimp::Importer importer;
	std::string path = ".\\resources\\animations\\Shooting Arrow.fbx";
	const aiScene* animationScene = importer.ReadFile(path, aiProcess_Triangulate);
    
    if (not animationScene)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return -1;
	}

    std::cout << "FBX file loaded successfully." << std::endl;
    if (animationScene->HasAnimations())
    {

        std::cout << "Number of animations: " << animationScene->mNumAnimations << " Name: " << animationScene->mName.C_Str() << " Number of skeletons: " << animationScene->mNumSkeletons << std::endl;
    }


    Model plane("C:\\Users\\chris\\Documents\\blender_models\\grass_ground.obj");
    Model ball("C:\\Users\\chris\\Documents\\blender_models\\ball.obj");
    Model backpack(".\\resources\\backpack\\backpack.obj");
	Model death("C:\\Users\\chris\\Documents\\blender_models\\death.obj");
	Model pill("C:\\Users\\chris\\Documents\\blender_models\\pill.obj");
    Model box("C:\\Users\\chris\\Documents\\blender_models\\cube.obj");
	Model quad("C:\\Users\\chris\\Documents\\blender_models\\quad.obj");
	//Model terrain("C:\\Users\\chris\\Documents\\blender_models\\terrain_plain.fbx");
    Model terrain(100.0f, 2600, "C:\\Users\\chris\\source\\repos\\learnopengl\\resources\\textures\\iceland_heightmap.png");
	RenderObject box_1(box, boxShader);

    RenderObject plane_1(plane, model_loading_shader, glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(5.0f));
    plane_1.genBoundingBox();
    plane_1.setGamma(0.5f);
    plane_1.setTexScale(8.0f);
    RenderObject ball_1(ball, lightShader, glm::vec3(5.3f, 5.0f, 5.0f), glm::vec3(0.5f));
    RenderObject backpack_1(backpack, model_loading_shader, glm::vec3(0.0f, 0.38f, -0.38f), glm::vec3(0.18f), glm::vec3(0.0f, 180.0f, 0.0f));
	RenderObject death_1(death, fbx_shader, glm::vec3(0.0f, -1.0f, 0.0f));
	death_1.genBoundingBox();
	death_1.setGamma(0.5f);

	RenderObject quad_1(quad, model_loading_shader, glm::vec3(0.0f));

    SkyBox skyBox{".\\resources\\skybox", &skyboxShader};

	RenderObject terrain_1(terrain, terrainShader, glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(1.0f));

    std::vector<Shader*> shaders{&model_loading_shader, &lightShader, &fbx_shader, &fbo_shader, &boxShader};
    std::vector<RenderObject> objs{plane_1, ball_1, backpack_1, death_1, quad_1, terrain_1 };
	std::vector<Camera*> cameras{ &player.getCamera() };
    scene = new Scene{ shaders, objs, cameras, &skyBox, SCR_WIDTH, SCR_HEIGHT};

   // unsigned int fbo;
   // glGenFramebuffers(1, &fbo);

    glm::vec3 lightPos(5.3f, 5.0f, 5.0f);

    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.95f);

    glm::mat4 projection = glm::perspective(glm::radians(player.getCamera().Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
    glm::mat4 view = player.getCamera().GetViewMatrix();

    glm::vec3 diffuseColor = glm::vec3(1.0f); // decrease the influence
    glm::vec3 ambientColor = glm::vec3(0.3f); // low influence

	fbo_shader.use();
	fbo_shader.setInt("screenTexture", 0);

    model_loading_shader.use();
    model_loading_shader.setMat4("projection", projection);
    model_loading_shader.setMat4("view", view);
    model_loading_shader.setFloat("gamma", 1.0f);
	model_loading_shader.setFloat("heightScale", heightScale);

    model_loading_shader.setVec3("viewPos", player.getCamera().Position);

    model_loading_shader.setVec3("light.position", lightPos);
    model_loading_shader.setVec3("light.ambient", ambientColor);
    model_loading_shader.setVec3("light.diffuse", diffuseColor);
    model_loading_shader.setVec3("light.specular", lightColor);

    model_loading_shader.setFloat("light.constant", 1.0f);
    model_loading_shader.setFloat("light.linear", 0.09f);
    model_loading_shader.setFloat("light.quadratic", 0.032f);

    terrainShader.use();
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);
    terrainShader.setFloat("gamma", 1.0f);
    terrainShader.setFloat("heightScale", heightScale);

    terrainShader.setVec3("viewPos", player.getCamera().Position);

    terrainShader.setVec3("light.position", lightPos);
    terrainShader.setVec3("light.ambient", ambientColor);
    terrainShader.setVec3("light.diffuse", diffuseColor);
    terrainShader.setVec3("light.specular", lightColor);

    terrainShader.setFloat("light.constant", 1.0f);
    terrainShader.setFloat("light.linear", 0.09f);
    terrainShader.setFloat("light.quadratic", 0.032f);

	fbx_shader.use();
	fbx_shader.setMat4("projection", projection);
	fbx_shader.setMat4("view", view);
	fbx_shader.setVec3("viewPos", player.getCamera().Position);
	fbx_shader.setFloat("gamma", 1.0f);

	fbx_shader.setVec3("light.position", lightPos);
	fbx_shader.setVec3("light.ambient", ambientColor);
	fbx_shader.setVec3("light.diffuse", diffuseColor);
	fbx_shader.setVec3("light.specular", lightColor);

	fbx_shader.setFloat("light.constant", 1.0f);
	fbx_shader.setFloat("light.linear", 0.09f);
	fbx_shader.setFloat("light.quadratic", 0.032f);

	FrameBuffer fbo(SCR_WIDTH, SCR_HEIGHT);

	glm::vec3 viewPos = player.getCamera().Position;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwSetWindowTitle(window, ("LearnOpenGL - FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime))).c_str());

        if (resizeWindow)
        {
			fbo.Resize(SCR_WIDTH, SCR_HEIGHT);
            resizeWindow = false;
		}
        
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // input
        // -----
        processInput(window);

        fbo.Bind();
		glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // render
        // ------
		if (wireframe) 
        { 
            glEnable(GL_DEBUG_OUTPUT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } 
        model_loading_shader.use();

		model_loading_shader.setBool("parallaxMappingEnabled", parallax);
		model_loading_shader.setFloat("heightScale", heightScale);

		terrainShader.use();
		terrainShader.setFloat("heightScale", heightScale);

		fbx_shader.use();
        scene->Draw();

        if (wireframe)
        {
            glDisable(GL_DEBUG_OUTPUT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        fbo.Unbind();
		glDisable(GL_DEPTH_TEST);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

		fbo_shader.use();
		glBindVertexArray(quadVAO);
        fbo.BindTexture();
		fbo_shader.setFloat("width", static_cast<float>(SCR_WIDTH));
		fbo_shader.setFloat("height", static_cast<float>(SCR_HEIGHT));
		fbo_shader.setFloat("time", currentFrame);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
		float drawTime = static_cast<float>(glfwGetTime()) - currentFrame;

        if (drawTime < frameTime) // 125 FPS
        {
			std::chrono::milliseconds sleepDuration(static_cast<int>((frameTime - drawTime) * 1000));
            std::this_thread::sleep_for(sleepDuration); // Sleep for the remaining time to maintain a stable frame rate
		}
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window)
{
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !keyIsHeld(GLFW_KEY_ESCAPE))
    {
        setKeyIsHeld(GLFW_KEY_ESCAPE);
        show_demo_window = !show_demo_window;
        if (show_demo_window)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } 
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && keyIsHeld(GLFW_KEY_ESCAPE))
    {
        resetKeyIsHeld(GLFW_KEY_ESCAPE);
        if (!show_demo_window)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player.processKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		player.processKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		heightScale += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		heightScale -= 0.01f;
    
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !keyIsHeld(GLFW_KEY_C))
    {
		setKeyIsHeld(GLFW_KEY_C);
		wireframe = !wireframe;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && keyIsHeld(GLFW_KEY_C))
    {
        resetKeyIsHeld(GLFW_KEY_C);
	}
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !keyIsHeld(GLFW_KEY_X))
	{
        setKeyIsHeld(GLFW_KEY_X);
        parallax = !parallax;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && keyIsHeld(GLFW_KEY_X))
    {
        resetKeyIsHeld(GLFW_KEY_X);
    }
}
bool keyIsHeld(int key)
{
	return std::find(heldKeys.begin(), heldKeys.end(), key) != heldKeys.end();
}
void setKeyIsHeld(int key)
{
    if (!keyIsHeld(key)) {
        heldKeys.push_back(key);
    }
}
void resetKeyIsHeld(int key)
{
    auto it = std::find(heldKeys.begin(), heldKeys.end(), key);
    if (it != heldKeys.end()) {
        heldKeys.erase(it);
    }
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
	resizeWindow = true;
	scene->updateScreenSize(width, height);
	std::cout << "Framebuffer size changed to: " << width << "x" << height << std::endl;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    player.processMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player.processMouseScroll(static_cast<float>(yoffset));
}

void toggleWireFrame()
{
    if (wireframe) {
        glEnable(GL_DEBUG_OUTPUT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glDisable(GL_DEBUG_OUTPUT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
