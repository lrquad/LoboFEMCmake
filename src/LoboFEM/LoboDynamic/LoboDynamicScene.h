#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "Utils/pugixml/pugixml.hpp"
#include <iostream>

namespace Lobo {

static std::string demo_path = "./demo/default/";

class LoboScene;
class LoboTetMesh;
class LoboShader;

inline std::string getPath(const char* filename) {
    std::ostringstream oss;
    oss << demo_path << filename;
    std::string s = oss.str();
    return s;
}

class LoboDynamicScene {
   public:
    LoboDynamicScene(LoboScene* scene_);
    ~LoboDynamicScene();

    virtual void drawImGui(bool* p_open = NULL);
    virtual void runXMLscript(pugi::xml_node &scene_node);

    virtual void mouseRectSelect();
    
    virtual void update();  // forward one timestep

    virtual void paintGL(LoboShader* shader, bool depth_render = false);
    virtual void initialGL();
    virtual void updateGL();  // update vertex buffer
    virtual void deleteGL();

    virtual void bindTetMesh(int trimesh_id, const char* filebase,
                             bool loadfile, bool binary = true);

    std::vector<LoboTetMesh*> tetmesh_in_scene;
    std::vector<int> tetmesh_triId;
    LoboScene* scene;
};

}  // namespace Lobo