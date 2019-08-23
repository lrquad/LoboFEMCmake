#include "LoboFEM.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "stb_image_write.h"

Lobo::LoboFEM::LoboFEM()
{
    dynamic_scene = NULL;
    scene = NULL;
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    use_screen_buffer = false;
}

Lobo::LoboFEM::~LoboFEM()
{
    delete dynamic_scene;
    delete scene;
}

void Lobo::LoboFEM::initScreenBuffer()
{
    float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,
                            -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f, 1.0f};
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    unsigned int SCR_WIDTH = 1280;
    unsigned int SCR_HEIGHT = 720;
    screenShader.useProgram();
    screenShader.setInt("screenTexture", 0);
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);           // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderQuad();
void Lobo::LoboFEM::windowLoop(GLFWwindow *window)
{
    ImGui::NewFrame();

    drawImGui();

    ImGui::Render();

    //shadow rendering
    simpleDepthShader.useProgram();
    int numlights = light_manager.getLightNum();
    for (int i = 0; i < numlights; i++)
    {
        if (!light_manager.getLightTrigger(i) || !light_manager.getLightCastShadow(i))
        {
            continue;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, light_manager.getDepthFBO(i));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        light_manager.getTextureSize(SHADOW_WIDTH, SHADOW_HEIGHT, i);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 shadowmodel = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", shadowmodel);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        light_manager.setLightShadow(&simpleDepthShader, i);

        scene->paintGL(&simpleDepthShader);
        dynamic_scene->paintGL(&default_shader, true);

        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    int display_w, display_h;
    // glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);

    //
    //display_w*=0.8;

    //if (use_screen_buffer)
    //   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z,
                 clear_color.w);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // more opengl stuff
    // create transformations
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)display_w / (float)display_h,
        0.001f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    //set global context;
    camera.view_matrix = view;
    camera.projection_matrix = projection;
    camera.view_port[0] = 0.0;
    camera.view_port[1] = 0.0;
    camera.view_port[2] = display_w;
    camera.view_port[3] = display_h;

    default_shader.useProgram();
    default_shader.setMat4(
        "projection",
        projection); // note: currently we set the projection matrix each
                     // frame, but since the projection matrix rarely
                     // changes it's often best practice to set it outside
                     // the main loop only once.
    default_shader.setMat4("view", view);
    default_shader.setMat4("model", model);

    // default_shader.setVec3("lightPos",cubelight.lightPos);
    // default_shader.setVec3("lightColor",cubelight.lightColor);
    default_shader.setVec3("viewPos", camera.Position);
    light_manager.setLight(&default_shader);

    scene->paintGL(&default_shader);
    dynamic_scene->paintGL(&default_shader, false);

    lighting_shader.useProgram();
    lighting_shader.setMat4(
        "projection",
        projection); // note: currently we set the projection matrix each
                     // frame, but since the projection matrix rarely
                     // changes it's often best practice to set it outside
                     // the main loop only once.
    lighting_shader.setMat4("view", view);
    lighting_shader.setMat4("model", model);

    light_manager.paintGL(&lighting_shader);

    if (use_screen_buffer)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        //glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
        //glClear(GL_COLOR_BUFFER_BIT);
        debugDepthQuad.useProgram();
        debugDepthQuad.setInt("depthMap", 0);
        debugDepthQuad.setFloat("near_plane", 0);
        debugDepthQuad.setFloat("far_plane", 10);
        //glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_manager.getDepthMap(0));

        //GLfloat *pixels = new GLfloat[display_w * display_h];
        //glReadPixels also returns 0s
        //glReadPixels( 0, 0, display_w, display_h, GL_RGB,  GL_FLOAT, pixels );
        //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
        //stbi_write_jpg("test.jpg",  display_w,  display_h,  3, pixels,  100.0);
        //delete pixels;
        //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glBindVertexArray(0);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        renderQuad();
    }
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Lobo::LoboFEM::drawImGui()
{
    bool show_demo_window = true;

    showMainWindow(&fileDialog);

    light_manager.drawImGui();
    camera.drawImGui();
    scene->drawImGui();
    dynamic_scene->drawImGui();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void Lobo::LoboFEM::showMainWindow(ImGui::FileBrowser *fileDialog, bool *p_open)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                fileDialog->Open();
                //ImGui::FileBrowser filebrowser;
                //filebrowser.Display();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
            }
            ImGui::EndMenu();
        }

        ImGui::Text("| %.3f ms/frame (%.1f FPS) | %s ", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, config_file_path.c_str());
        if (ImGui::Button("OffScreenBuffer"))
        {
            use_screen_buffer = !use_screen_buffer;
        }
        ImGui::EndMainMenuBar();
    }

    fileDialog->Display();

    if (fileDialog->HasSelected())
    {
        config_file_path = fileDialog->GetSelected().string();
        fileDialog->ClearSelected();
        this->loadXMLfile(config_file_path.c_str());
    }
}

void Lobo::LoboFEM::paintGL(LoboShader *shader)
{
}

void Lobo::LoboFEM::deleteGL()
{
    dynamic_scene->deleteGL();
    scene->deleteGL();
}

void Lobo::LoboFEM::makeContext()
{
    //scripts
    scene = new Lobo::LoboScene();
    //scene->addMesh("./models/floor.obj", false);
    //scene->addMesh("./models/earth/earth.obj", true);
    scene->initialGL();

    dynamic_scene = new Lobo::LoboDynamicScene(scene);
    // dynamic_scene->bindTetMesh(1, "tetmesh/bunny", false, true);
    dynamic_scene->initialGL();

    default_shader.loadShaderFile("./shaders/simplevertex.glsl",
                                  "./shaders/simplefrag.glsl");
    lighting_shader.loadShaderFile("./shaders/lightvertex.glsl",
                                   "./shaders/lightfrag.glsl");
    simpleDepthShader.loadShaderFile("./shaders/depthvertex.glsl",
                                     "./shaders/depthfrag.glsl");
    debugDepthQuad.loadShaderFile("./shaders/debugdepth.vs",
                                  "./shaders/debugdepth.fs");
    screenShader.loadShaderFile("./shaders/framebuffer_screen.vs",
                                "./shaders/framebuffer_screen.fs");
    camera.initCamera(glm::vec3(0.0, 3.0f, 8.0f), glm::vec3(0.0, 1.0, 0.0), -90, -16.0);
}
void Lobo::LoboFEM::setCurrentContext()
{
    //
    Lobo::setCurrentCamera(&camera);
}

void Lobo::LoboFEM::loadXMLfile(const char *filename)
{
    deleteGL();
    delete dynamic_scene;
    delete scene;
    scene = new Lobo::LoboScene();
    dynamic_scene = new Lobo::LoboDynamicScene(scene);

    std::cout << "load scene script" << filename << std::endl;
    pugi::xml_parse_result result = xml_doc.load_file(filename);
    if (!result)
        std::cout << "xml load failed" << std::endl;

    if (xml_doc.child("Scene").child("LoboScene"))
    {
        pugi::xml_node scene_node = xml_doc.child("Scene").child("LoboScene");
        scene->runXMLscript(scene_node);
    }

    scene->initialGL();

    if (xml_doc.child("Scene").child("LoboDynamicScene"))
    {
        pugi::xml_node scene_node = xml_doc.child("Scene").child("LoboDynamicScene");
        dynamic_scene->runXMLscript(scene_node);
    }
    dynamic_scene->initialGL();

    std::cout << "Finished." << std::endl;
    setCurrentContext();
}