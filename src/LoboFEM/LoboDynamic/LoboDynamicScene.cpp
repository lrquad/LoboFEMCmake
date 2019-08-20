#include "LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "OpenGLutils/LoboScene.h"
#include "Shaders/LoboShader.h"

Lobo::LoboDynamicScene::LoboDynamicScene(Lobo::LoboScene *scene_) : scene(scene_)
{
}

Lobo::LoboDynamicScene::~LoboDynamicScene()
{
}

void Lobo::LoboDynamicScene::drawImGui(bool *p_open)
{
    ImGui::Begin("Dynamic Scene",
                 p_open); // Create a window called "Hello, world!" and
                          // append into it.
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        ImGui::PushID(i);
        tetmesh_in_scene[i]->drawImGui();
        ImGui::PopID();
    }

    ImGui::End();
}

void Lobo::LoboDynamicScene::update()
{
    
}

void Lobo::LoboDynamicScene::paintGL(LoboShader *shader, bool depth_render)
{
    for (int i = 0; i < tetmesh_in_scene.size(); i++)
    {
        if (!depth_render)
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

void Lobo::LoboDynamicScene::bindTetMesh(int trimesh_id, const char *filebase, bool loadfile, bool binary)
{
    Lobo::LoboTetMesh *tetmesh = new Lobo::LoboTetMesh();
    tetmesh->setInputPolygon(scene->getMesh(trimesh_id));
    tetmesh->usebinary = binary;
    tetmesh->filebase = filebase;
    tetmesh_in_scene.push_back(tetmesh);
    tetmesh_triId.push_back(trimesh_id);
}
