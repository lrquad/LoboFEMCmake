#pragma once
#include "LoboMesh/LoboMesh.h"
#include "Shaders/LoboShader.h"
#include "Functions/deleteSTDvector.h"
#include <imgui.h>

namespace Lobo {
class LoboScene {
public:
    LoboScene(){};
    ~LoboScene(){
        deleteStdvectorPointer(mesh_list);
    };

    virtual void addMesh(const char* filename, bool uniform = true) {
        LoboMesh* mesh = new LoboMesh(filename, uniform);
        mesh_list.push_back(mesh);
    }

    virtual LoboMesh* getMesh(int meshid)
    {
        return mesh_list[meshid];
    }

    virtual void drawImGui(bool* p_open = NULL) {
        ImGui::Begin("Scene",
                     p_open);  // Create a window called "Hello, world!" and
                               // append into it.
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for(int i=0;i<mesh_list.size();i++)
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
        if(selected<mesh_list.size()&&selected>=0)
        mesh_list[selected]->drawImGui();
        ImGui::EndChild();
        ImGui::EndGroup();
        ImGui::End();
    }
    virtual void initialGL() {
        for(int i=0;i<mesh_list.size();i++)
        {
            mesh_list[i]->initialGL();
        }
    }
    virtual void paintGL(LoboShader* shader)
    {
        for(int i=0;i<mesh_list.size();i++)
        {
            mesh_list[i]->paintGL(shader);
        }
    }

    virtual void deleteGL()
    {
        for(int i=0;i<mesh_list.size();i++)
        {
            mesh_list[i]->deleteGL();
        }
    }

    std::vector<LoboMesh*> mesh_list;
};
}  // namespace Lobo