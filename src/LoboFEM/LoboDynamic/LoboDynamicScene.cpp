#include "LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "OpenGLutils/LoboScene.h"

Lobo::LoboDynamicScene::LoboDynamicScene(Lobo::LoboScene* scene_):scene(scene_)
{

}

Lobo::LoboDynamicScene::~LoboDynamicScene()
{

}

void Lobo::LoboDynamicScene::drawImGui(bool* p_open)
{

}

void Lobo::LoboDynamicScene::update()
{

}

void Lobo::LoboDynamicScene::paintGL()
{

}

void Lobo::LoboDynamicScene::initialGL()
{
    
}

void Lobo::LoboDynamicScene::bindTetMesh(int trimesh_id,const char* filebase,bool loadfile,bool binary)
{
    Lobo::LoboTetMesh* tetmesh = new Lobo::LoboTetMesh();
    tetmesh->setInputPolygon(scene->getMesh(trimesh_id));
    tetmesh->usebinary = binary;
    tetmesh->filebase = filebase;
    tetmesh_in_scene.push_back(tetmesh);
    tetmesh_triId.push_back(trimesh_id);

}


