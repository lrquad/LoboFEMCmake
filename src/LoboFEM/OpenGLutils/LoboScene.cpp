#include "LoboScene.h"


void Lobo::LoboScene::addMesh(const char *filename, bool uniform)
{
    LoboMesh *mesh = new LoboMesh(filename, uniform);
    mesh_list.push_back(mesh);
}

Lobo::LoboMesh *Lobo::LoboScene::getMesh(int meshid)
{
    return mesh_list[meshid];
}

void Lobo::LoboScene::drawImGui(bool *p_open)
{
    ImGui::Begin("Scene",
                 p_open); // Create a window called "Hello, world!" and
                          // append into it.
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true);
    for (int i = 0; i < mesh_list.size(); i++)
    {
        char label[128];
        sprintf(label, "%s", mesh_list[i]->obj_file_name.c_str());
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    if (selected < mesh_list.size() && selected >= 0)
        mesh_list[selected]->drawImGui();
    ImGui::EndChild();
    ImGui::EndGroup();
    ImGui::End();
}

void Lobo::LoboScene::initialGL()
{
    for (int i = 0; i < mesh_list.size(); i++)
    {
        mesh_list[i]->initialGL();
    }
}

void Lobo::LoboScene::paintGL(LoboShader *shader)
{
    for (int i = 0; i < mesh_list.size(); i++)
    {
        mesh_list[i]->paintGL(shader);
    }
}

void Lobo::LoboScene::deleteGL()
{
    for (int i = 0; i < mesh_list.size(); i++)
    {
        mesh_list[i]->deleteGL();
    }
}