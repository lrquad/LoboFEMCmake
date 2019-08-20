#pragma once
#include <vector>

namespace Lobo
{
    class LoboScene;
    class LoboTetMesh;
    class LoboShader;

    class LoboDynamicScene
    {
        public:
        LoboDynamicScene(LoboScene* scene_);
        ~LoboDynamicScene();

        virtual void drawImGui(bool* p_open = NULL);
        virtual void update(); //forward one timestep
        virtual void paintGL(LoboShader* shader, bool depth_render=false);
        virtual void initialGL();
        virtual void updateGL();//update vertex buffer

        virtual void bindTetMesh(int trimesh_id,const char* filebase,bool loadfile,bool binary=true);
        
        std::vector<LoboTetMesh*> tetmesh_in_scene;
        std::vector<int> tetmesh_triId;
        LoboScene* scene;
    };
}