#pragma once
#include <vector>

namespace Lobo
{
    class LoboScene;
    class LoboTetMesh;

    class LoboDynamciScene
    {
        public:
        LoboDynamciScene(LoboScene* scene_);
        ~LoboDynamciScene();

        virtual void drawImGui(bool* p_open = NULL);
        virtual void update(); //forward one timestep
        virtual void paintGL();
        virtual void initialGL();

        virtual void bindTetMesh(int trimesh_id,const char* filebase,bool loadfile,bool binary=true);
        
        std::vector<LoboTetMesh*> tetmesh_in_scene;
        std::vector<int> tetmesh_triId;
        LoboScene* scene;
    };
}