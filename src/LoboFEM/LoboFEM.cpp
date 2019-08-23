#include "LoboFEM.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

Lobo::LoboFEM::LoboFEM()
{
    dynamic_scene = NULL;
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

Lobo::LoboFEM::~LoboFEM()
{
    delete dynamic_scene;
    delete scene;
}

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
        if (!light_manager.getLightTrigger(i)||!light_manager.getLightCastShadow(i))
        {
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

        scene->paintGL(&simpleDepthShader);
        dynamic_scene->paintGL(&default_shader, true);

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

        ImGui::Text("| %.3f ms/frame (%.1f FPS) | %s ",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate,config_file_path.c_str());
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
    scene->addMesh("./models/floor.obj", false);
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

    camera.initCamera(glm::vec3(0.0, 3.0f, 8.0f), glm::vec3(0.0, 1.0, 0.0), -90, -16.0);
}
void Lobo::LoboFEM::setCurrentContext()
{
    //
    Lobo::setCurrentCamera(&camera);

}


void Lobo::LoboFEM::loadXMLfile(const char* filename)
{
    deleteGL();
    delete dynamic_scene;
    delete scene;
    scene = new Lobo::LoboScene();
    dynamic_scene = new Lobo::LoboDynamicScene(scene);


    std::cout<<"load scene script" << filename <<std::endl;
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

    std::cout<<"Finished." <<std::endl;
    setCurrentContext();
}