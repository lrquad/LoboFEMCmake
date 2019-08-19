#pragma once
#include "LoboMesh/LoboMesh.h"
#include "Shaders/LoboShader.h"
#include "Functions/deleteSTDvector.h"
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
        
        for(int i=0;i<mesh_list.size();i++)
        {
            ImGui::PushID(i);
            mesh_list[i]->drawImGui();
            ImGui::PopID();
        }
        
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