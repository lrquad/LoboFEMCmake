// dear imgui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to dear imgui, see examples/README.txt
// and documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "LoboImGui/MainWindow.h"
#include "LoboImGui/imfilebrowser.h"

// These headers are for test
// will be refactored in the future
#include "LoboMesh/LoboMesh.h"
#include "OpenGLutils/LoboScene.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"

#include "OpenGLutils/LoboCamera.h"
#include "Shaders/LoboLighting.h"
#include "Shaders/LoboShader.h"

#include <stdio.h>
// About OpenGL function loaders: modern OpenGL doesn't have a standard header
// file and requires individual function pointers to be loaded manually. Helper
// libraries are often used for this purpose! Here we are supporting a few
// common ones: gl3w, glew, glad. You may use another loader/header of your
// choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>  // Initialize with gl3wInit()
#pragma message("C Preprocessor got here!")
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>  // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Eigen/Dense>
#include <iostream>

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
void renderQuad();

int main() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char *glsl_version = "#version 130";
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 16);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    int window_w = mode->width;
    int window_h = mode->height;
    window_w = 1280;
    window_h = 720;
    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(
        window_w, window_h, "New LoboFEM", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err =
        false;  // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is
                // likely to requires some form of initialization.
#endif
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    ImGui::FileBrowser fileDialog;
    Lobo::LoboScene scene;

    //scripts
    scene.addMesh("./models/floor.obj", false);
    scene.addMesh("./models/bunny.obj",false);

    scene.initialGL();

    Lobo::LoboDynamicScene dynamic_scene(&scene);
    dynamic_scene.bindTetMesh(1,"tetmesh/bunny",false,true);
    dynamic_scene.initialGL();


    Lobo::LoboLightManager light_manager;

    // init shader
    Lobo::LoboShader default_shader;
    Lobo::LoboShader lighting_shader;
    Lobo::LoboShader simpleDepthShader;
    Lobo::LoboShader debugDepthQuad;

    default_shader.loadShaderFile("./shaders/simplevertex.glsl",
                                  "./shaders/simplefrag.glsl");
    lighting_shader.loadShaderFile("./shaders/lightvertex.glsl",
                                   "./shaders/lightfrag.glsl");
    simpleDepthShader.loadShaderFile("./shaders/depthvertex.glsl",
                                     "./shaders/depthfrag.glsl");
    debugDepthQuad.loadShaderFile("./shaders/debugdepth.vs",
                                  "./shaders/debugdepth.fs");

    // test code
    unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

    // init camera
    Lobo::Camera camera(glm::vec3(0.0f, 4.0f, 8.0f),glm::vec3(0.0,1.0,0.0),-90,-30);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data
        // to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application. Generally you may always pass all
        // inputs to dear imgui, and hide them from your application based on
        // those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Lobo::ShowMainWindow(&fileDialog);

        // cubelight.drawImGui();
        light_manager.drawImGui();
        camera.drawImGui();
        scene.drawImGui();
        dynamic_scene.drawImGui();


        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about
        // Dear ImGui!).
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();

        // shadow

        simpleDepthShader.useProgram();
        // light_manager.setLightShadow(&simpleDepthShader);
        int numlights = light_manager.getLightNum();
        for (int i = 0; i < numlights; i++) {
            if (!light_manager.getLightTrigger(i)) {
                continue;
            }
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            light_manager.getTextureSize(SHADOW_WIDTH, SHADOW_HEIGHT, i);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, light_manager.getDepthFBO(i));
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::mat4 shadowmodel = glm::mat4(1.0f);
            simpleDepthShader.setMat4("model", shadowmodel);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            light_manager.setLightShadow(&simpleDepthShader, i);
            
            scene.paintGL(&simpleDepthShader);
            dynamic_scene.paintGL(&default_shader,true);

            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        int display_w, display_h;
        // glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        //display_w*=0.8;

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z,
                     clear_color.w);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // more opengl stuff
        // create transformations
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), (float)display_w / (float)display_h,
            0.001f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        default_shader.useProgram();
        default_shader.setMat4(
            "projection",
            projection);  // note: currently we set the projection matrix each
                          // frame, but since the projection matrix rarely
                          // changes it's often best practice to set it outside
                          // the main loop only once.
        default_shader.setMat4("view", view);
        default_shader.setMat4("model", model);

        // default_shader.setVec3("lightPos",cubelight.lightPos);
        // default_shader.setVec3("lightColor",cubelight.lightColor);
        default_shader.setVec3("viewPos", camera.Position);
        light_manager.setLight(&default_shader);

        scene.paintGL(&default_shader);
        dynamic_scene.paintGL(&default_shader,false);

        lighting_shader.useProgram();
        lighting_shader.setMat4(
            "projection",
            projection);  // note: currently we set the projection matrix each
                          // frame, but since the projection matrix rarely
                          // changes it's often best practice to set it outside
                          // the main loop only once.
        lighting_shader.setMat4("view", view);
        lighting_shader.setMat4("model", model);

        light_manager.paintGL(&lighting_shader);

        debugDepthQuad.useProgram();
        debugDepthQuad.setInt("depthMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_manager.getDepthMap(0));
        //renderQuad();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapInterval(1);  // Enable vsync

        glfwSwapBuffers(window);
    }

    scene.deleteGL();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
