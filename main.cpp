#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <assimp/config.h>

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

#define CUBE_FILE false

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void toggleWireFrame();
void render();
//void picking();

// settings
unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 1200;

bool wireframe = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Player player(camera);
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

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader model_loading_shader("model_loading.vs", "model_loading.fs");
    Shader lightShader("lightCubeShader.vs", "lightCubeShader.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");


    Model plane("C:\\Users\\chris\\Documents\\blender_models\\grass_ground.obj");
    Model ball("C:\\Users\\chris\\Documents\\blender_models\\ball.obj");
    Model backpack(".\\resources\\backpack\\backpack.obj");

    SkyBox skyBox;

   // unsigned int fbo;
   // glGenFramebuffers(1, &fbo);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // input
        // -----
        processInput(window);
        // render
        // ------
        glm::vec3 lightPos(5.3f, 5.0f, 5.0f);

        glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.95f);

        glm::mat4 projection = glm::perspective(glm::radians(player.getCamera().Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
        glm::mat4 view = player.getCamera().GetViewMatrix();

        glm::vec3 diffuseColor = glm::vec3(1.0f); // decrease the influence
        glm::vec3 ambientColor = glm::vec3(0.3f); // low influence

        model_loading_shader.use();
        model_loading_shader.setMat4("projection", projection);
        model_loading_shader.setMat4("view", view);
        model_loading_shader.setFloat("gamma", 1.0f);

        model_loading_shader.setVec3("viewPos", player.getCamera().Position);

        model_loading_shader.setVec3("light.position", lightPos);
        model_loading_shader.setVec3("light.ambient", ambientColor);
        model_loading_shader.setVec3("light.diffuse", diffuseColor);
        model_loading_shader.setVec3("light.specular", lightColor);

        model_loading_shader.setFloat("light.constant", 1.0f);
        model_loading_shader.setFloat("light.linear", 0.09f);
        model_loading_shader.setFloat("light.quadratic", 0.032f);

        glm::mat4 model1 = glm::mat4(1.0f);
        //model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 0.0f));
        //model1 = glm::scale(model1, glm::vec3(1.0f));
        model_loading_shader.setMat4("model", model1);
        model_loading_shader.setFloat("texScale", 1.0f);
        backpack.Draw(model_loading_shader);

        model1 = glm::translate(model1, glm::vec3(0.0f, -3.0f, 0.0f));

        model_loading_shader.setMat4("model", model1);

        model_loading_shader.setFloat("gamma", 0.5f);
        model_loading_shader.setFloat("texScale", 8.0f);
        plane.Draw(model_loading_shader);
        glm::mat4 model2 = glm::mat4(1.0f);

        model2 = glm::translate(model2, lightPos);
        model2 = glm::scale(model2, glm::vec3(.1f + 0.125f * sin(currentFrame) + 0.25f));

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", model2);
        lightShader.setVec3("lightColor", diffuseColor);

        ball.Draw(lightShader);

        // remove translation from the view matrix
        glDepthFunc(GL_LEQUAL);
        skyBox.Draw(skyboxShader, projection, view);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
bool held = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
    {
		if (!held) {
			held = true;
            toggleWireFrame();
		}
    }
    else
    {
		held = false;
    }
}
void render()
{
   
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
    wireframe = !wireframe;
    if (wireframe) {
        glEnable(GL_DEBUG_OUTPUT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glDisable(GL_DEBUG_OUTPUT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
