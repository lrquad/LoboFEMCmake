#include "LoboDynamicScene.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "OpenGLutils/LoboScene.h"

Lobo::LoboDynamciScene::LoboDynamciScene(Lobo::LoboScene* scene_):scene(scene_)
{

}

Lobo::LoboDynamciScene::~LoboDynamciScene()
{

}

void Lobo::LoboDynamciScene::drawImGui(bool* p_open)
{

}

void Lobo::LoboDynamciScene::update()
{

}

void Lobo::LoboDynamciScene::paintGL()
{

}

void Lobo::LoboDynamciScene::initialGL()
{
    
}

void Lobo::LoboDynamciScene::bindTetMesh(int trimesh_id,const char* filebase,bool loadfile,bool binary)
{
    Lobo::LoboTetMesh* tetmesh = new Lobo::LoboTetMesh();
    tetmesh->setInputPolygon(scene->getMesh(trimesh_id));
    tetmesh->usebinary = binary;
    tetmesh->filebase = filebase;
    tetmesh_in_scene.push_back(tetmesh);
    tetmesh_triId.push_back(trimesh_id);

}


