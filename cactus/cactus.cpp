#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "camera.h"
#include "model.h"
#include "shader_m.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include <chrono>
#include <format>
#include <iostream>
using namespace std::literals;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
int  load_texture(const char *path);

// horse number
const int CACTUS_NUMBER = 128;

// settings
const unsigned int SCR_WIDTH  = 1400;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(1.0f, 1.0f, 3.0f));
float  lastX      = SCR_WIDTH / 2.0f;
float  lastY      = SCR_HEIGHT / 2.0f;
bool   firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "仙人掌", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 1000, 1000);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // When sticky mouse buttons mode is enabled, the pollable state of a mouse button will remain GLFW_PRESS until the state of that button is polled with glfwGetMouseButton.
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    const char *glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("simhei.ttf", 20, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    (void) io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    glm::vec3 diffuse_color  = {0.1f, 0.9f, 0.1f};
    glm::vec3 specular_color = {0.5f, 0.5f, 0.5f};


    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader groundShader("1.model_loading.vs", "1.model_loading.fs");

    float ground_vertices[] = {
        3.0f, -0.0f, 2.0f, 1.0f, 0.0f,
        3.0f, -0.0f, -2.0f, 1.0f, 1.0f,
        -3.0f, -0.0f, 2.0f, 0.0f, 0.0f,

        3.0f, -0.0f, -2.0f, 1.0f, 1.0f,
        -3.0f, -0.0f, -2.0f, 0.0f, 1.0f,
        -3.0f, -0.0f, 2.0f, 0.0f, 0.0f};

    GLfloat ctrlpoints[4][4][3] = {
        {{-1.5, -1.5, 4.0}, {-0.5, -1.5, 2.0}, {0.5, -1.5, -1.0},  {1.5, -1.5, 2.0}},
        {{-1.5, -0.5, 1.0}, {-0.5, -0.5, 3.0},  {0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}},
        { {-1.5, 0.5, 4.0},  {-0.5, 0.5, 0.0},   {0.5, 0.5, 3.0},   {1.5, 0.5, 4.0}},
        {{-1.5, 1.5, -2.0}, {-0.5, 1.5, -2.0},   {0.5, 1.5, 0.0},  {1.5, 1.5, -1.0}}
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture
    unsigned int texture = load_texture("desert-2.png");
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3(-0.7f, 0.2f, 2.0f),
        glm::vec3(-2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
    };
    // load models
    // -----------
    vector<Model> horseModel;
    for (int i = 0; i < CACTUS_NUMBER; i++) {
        horseModel.emplace_back(std::format("./cactus/cactus_{}.obj", i));
    }
    cout << "Load Success\n";


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    auto startTime = chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setVec3("customDiffuse", diffuse_color);
        lightingShader.setVec3("customSpecular", specular_color);
        //lightingShader.setFloat("customShininess", 32.0f);

        /*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // spotLight // flashlight
        //lightingShader.setVec3("spotLight.position", camera.Position);
        //lightingShader.setVec3("spotLight.direction", camera.Front);
        //lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        //lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        //lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("spotLight.constant", 1.0f);
        //lightingShader.setFloat("spotLight.linear", 0.09);
        //lightingShader.setFloat("spotLight.quadratic", 0.032);
        //lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        //lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view       = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // render the loaded model
        const static glm::vec3 init_position = {-0.509197, -0.497593, -0.068486};
        static glm::vec3       translate     = {0.0f, 0.0f, 0.0f};
        static glm::vec3       rotate        = {1.0f, 0.0f, 0.0f};
        static float           angle         = glm::radians(- 90.f) ;
        static float           scale         = 1.0f;

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, translate);
        model           = glm::scale(model, glm::vec3(scale));// it's a bit too big for our scene, so scale it down
        model           = glm::rotate(model, /*glm::radians(angle)*/angle, rotate);
        model           = glm::translate(model, init_position);// translate it down so it's at the center of the scene
        lightingShader.setMat4("model", model);

        static auto speed   = 0.1f;
        auto        nowTime = chrono::high_resolution_clock::now();
        auto        index   = chrono::duration_cast<chrono::milliseconds>(nowTime - startTime).count() / (10 / speed);
        if (index >= CACTUS_NUMBER) {
            startTime = nowTime;
            index     = 0;
        }
        horseModel[index].Draw(lightingShader);

        //model = glm::mat4(1.0f);
        //model = glm::translate

        //ground
        glEnable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, texture);
        groundShader.use();
        groundShader.setMat4("projection", projection);
        groundShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        groundShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {

            ImGui::Begin("修改窗口!");// Create a window called "Hello, world!" and append into it.

            ImGui::SliderFloat("动画速度", &speed, 0.1f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("漫反射", (float *) &diffuse_color);// Edit 3 floats representing a color
            ImGui::ColorEdit3("镜面光照", (float *) &specular_color);
            ImGui::SliderFloat3("模型移动", (float *) &translate, -2.0f, 2.0f);
            ImGui::SliderFloat("模型缩放", &scale, 0.1f, 10.0f);
            ImGui::SliderFloat3("旋转轴 ", (float *) & rotate, 0.0f, 1.0f);
            ImGui::SliderAngle("旋转角度", &angle);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX      = xpos;
        lastY      = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (state == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

int load_texture(const char* path) {
    // texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);// all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int            width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    //unsigned char* data = stbi_load("C:/Users/isudfv/Desktop/LearnOpenGL/resources/textures/grassland.png", &width, &height, &nrChannels, 0);
    if (data) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}
