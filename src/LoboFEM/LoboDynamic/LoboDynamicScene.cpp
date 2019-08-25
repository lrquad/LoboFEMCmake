#include "LoboDynamic.h"
#include "LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "OpenGLutils/LoboScene.h"
#include "Shaders/LoboShader.h"
#include "LoboImGui/cpp/imgui_stdlib.h"
#include "Functions/deleteSTDvector.h"

std::string demo_path = "./demo/default/";

std::string Lobo::getPath(const char *filename)
{
    std::ostringstream oss;
    oss << demo_path << filename;
    std::string s = oss.str();
    return s;
}

Lobo::LoboDynamicScene::LoboDynamicScene(Lobo::LoboScene *scene_)
    : scene(scene_) {}

Lobo::LoboDynamicScene::~LoboDynamicScene()
{
    deleteStdvectorPointer(tetmesh_in_scene);
    deleteStdvectorPointer(dynamic_solvers);
}

void Lobo::LoboDynamicScene::runXMLscript(pugi::xml_node &scene_node)
{
    deleteStdvectorPointer(tetmesh_in_scene);
    if (scene_node.child("SceneFolder"))
    {
        demo_path = scene_node.child("SceneFolder").text().as_string();
    }
    prepareDir();

    for (pugi::xml_node tetmesh_node : scene_node.children("BindTetMesh"))
    {
        bool use_bindary = true; // default
        bool tetgen = true;
        bool loadmesh = false;
        bool savemesh = false;

        if (tetmesh_node.attribute("tetgen"))
            tetgen = tetmesh_node.attribute("tetgen").as_bool();
        if (tetmesh_node.attribute("loadmesh"))
            loadmesh = tetmesh_node.attribute("loadmesh").as_bool();
        if (tetmesh_node.attribute("savemesh"))
            savemesh = tetmesh_node.attribute("savemesh").as_bool();

        // required
        if (tetmesh_node.child("TrimeshId"))
        {
            int triid = tetmesh_node.child("TrimeshId").text().as_int();
            if (tetmesh_node.child("TetFileBase"))
            {
                std::string tetbase =
                    tetmesh_node.child("TetFileBase").text().as_string();
                this->bindTetMesh(triid, tetbase.c_str(), use_bindary);

                if (tetgen)
                {
                    const char *tetgen_command =
                        this->tetmesh_in_scene.back()->tetgen_command.c_str();
                    this->tetmesh_in_scene.back()->generateTet(tetgen_command);
                }
                if (loadmesh)
                {
                    this->tetmesh_in_scene.back()->loadTetMesh();
                }
                if (savemesh)
                {
                    this->tetmesh_in_scene.back()->exportTetMesh();
                }
            }
        }
    }

    for (pugi::xml_node solver_node : scene_node.children("DynamicSolver"))
    {
        // we only have on type here
        Lobo::DynamicSolver *dynamic_solver = NULL;

        if (solver_node.attribute("type"))
        {
            if(strcmp(solver_node.attribute("type").as_string(),"fullspace")==0)
            dynamic_solver = new Lobo::FullspaceSolver(this);
            else 
            {
                //other type of solver
            }
        }




        if (dynamic_solver != NULL)
        {
            dynamic_solver->runXMLscript(solver_node);
            dynamic_solvers.push_back(dynamic_solver);
        }
    }
}

void Lobo::LoboDynamicScene::drawImGui(bool *p_open)
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Begin("Dynamic Scene",
                 p_open); // Create a window called "Hello, world!" and
                          // append into it.

    ImGui::InputText("Data folder ", &demo_path);
    if (ImGui::Button("Prepare folder"))
    {
        prepareDir();
    }
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("TetMesh"))
        {
            drawImGuiTetMesh();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Solvers"))
        {
            drawImGuiSolvers();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    if (ImGui::IsMouseDragging(1) && io.KeysDownDuration[340] >= 0.0f)
    {
        mouseRectSelect();
    }
}

void Lobo::LoboDynamicScene::mouseRectSelect()
{
    ImGuiIO &io = ImGui::GetIO();

    static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 0.4f);
    const ImU32 col = ImColor(colf);
    ImGui::GetForegroundDrawList()->AddRectFilled(
        ImVec2(io.MouseClickedPos[1].x, io.MouseClickedPos[1].y),
        ImVec2(io.MousePos.x, io.MousePos.y), col, 4.0f, ImDrawCornerFlags_All);
}

void Lobo::LoboDynamicScene::update() {}

void Lobo::LoboDynamicScene::paintGL(LoboShader *shader, bool depth_render)
{
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        if ((!depth_render) ||
            (tetmesh_in_scene[i]->shader_config.cast_shadow))
        {
            tetmesh_in_scene[i]->paintGL(shader);
        }
    }
}

void Lobo::LoboDynamicScene::initialGL()
{
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        tetmesh_in_scene[i]->initialGL();
    }
}

void Lobo::LoboDynamicScene::updateGL()
{
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        tetmesh_in_scene[i]->updateGL();
    }
}

void Lobo::LoboDynamicScene::deleteGL()
{
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        tetmesh_in_scene[i]->deleteGL();
    }
}

void Lobo::LoboDynamicScene::bindTetMesh(int trimesh_id, const char *filebase,
                                         bool loadfile, bool binary)
{
    Lobo::LoboTetMesh *tetmesh = new Lobo::LoboTetMesh();
    tetmesh->setInputPolygon(scene->getMesh(trimesh_id));
    tetmesh->setBindingTriMesh(scene->getMesh(trimesh_id));

    tetmesh->usebinary = binary;
    tetmesh->filebase = filebase;
    tetmesh_in_scene.push_back(tetmesh);
    tetmesh_triId.push_back(trimesh_id);
}

void Lobo::LoboDynamicScene::prepareDir()
{
    std::ostringstream stringStream;
    stringStream << "mkdir -p " << demo_path << " &&";
    stringStream << "mkdir -p " << demo_path << "tetmesh"
                 << " &&";
    stringStream << "mkdir -p " << demo_path << "constraints"
                 << " &&";
    stringStream << "mkdir -p " << demo_path << "solveroutput"
                 << "";
    std::string system_command_string = stringStream.str();
    system(system_command_string.c_str());
    std::cout << system_command_string.c_str() << std::endl;
}

void Lobo::LoboDynamicScene::drawImGuiTetMesh()
{
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true);
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        char label[128];
        sprintf(label, "%s", tetmesh_in_scene[i]->filebase.c_str());
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("item view",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    if (selected < tetmesh_in_scene.size() && selected >= 0)
        tetmesh_in_scene[selected]->drawImGui();
    ImGui::EndChild();
    ImGui::EndGroup();
}

void Lobo::LoboDynamicScene::drawImGuiSolvers()
{
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true);
    for (int i = 0; i < dynamic_solvers.size(); i++)
    {
        char label[128];
        sprintf(label, "%d", i);
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("item view",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    if (selected < dynamic_solvers.size() && selected >= 0)
        dynamic_solvers[selected]->drawImGui();
    ImGui::EndChild();
    ImGui::EndGroup();
}