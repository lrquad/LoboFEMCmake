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

    

    virtual void addMesh(const char* filename, bool uniform = true);

    virtual LoboMesh* getMesh(int meshid);

    virtual void drawImGui(bool* p_open = NULL);

    virtual void initialGL();

    virtual void paintGL(LoboShader* shader);

    virtual void deleteGL();

    std::vector<LoboMesh*> mesh_list;
};
}  // namespace Lobo