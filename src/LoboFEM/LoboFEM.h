#pragma once
#include "LoboMesh/LoboMesh.h"
#include "OpenGLutils/LoboScene.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "OpenGLutils/LoboCamera.h"
#include "Shaders/LoboLighting.h"
#include "Shaders/LoboShader.h"
#include "LoboImGui/imfilebrowser.h"
#include "Utils/pugixml/pugixml.hpp"

namespace Lobo
{
class LoboDynamicScene;

static unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

class LoboFEM
{
public:
    LoboFEM();
    ~LoboFEM();

    virtual void windowLoop(GLFWwindow* window);
    virtual void drawImGui();
    virtual void paintGL(LoboShader *shader);
    virtual void deleteGL();
    virtual void makeContext();
    virtual void setCurrentContext();

protected:

    void loadXMLfile(const char* filename);
    void showMainWindow(ImGui::FileBrowser *fileDialog, bool *p_open=NULL);

    ImGui::FileBrowser fileDialog;
    LoboScene* scene;
    LoboDynamicScene *dynamic_scene;

    LoboShader default_shader;
    LoboShader lighting_shader;
    LoboShader simpleDepthShader;
    LoboShader debugDepthQuad;
    LoboLightManager light_manager;
    Camera camera;
    ImVec4 clear_color;

    std::string config_file_path;
    pugi::xml_document xml_doc;
};

} // namespace Lobo