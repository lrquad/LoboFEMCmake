#include "ModalWarpingSolver.h"
#include "LoboDynamic/LoboDynamic.h"
#include "imgui.h"

Lobo::ModalWarpingSovler::ModalWarpingSovler(Lobo::LoboDynamicScene *parent_scene):FullspaceSolver(parent_scene)
{

}

Lobo::ModalWarpingSovler::~ModalWarpingSovler()
{

}

void Lobo::ModalWarpingSovler::precompute()
{
    Lobo::FullspaceSolver::precompute();
    //precompute modal warping matrices 

}
void Lobo::ModalWarpingSovler::stepForward()
{
    time_integraion->stepFoward();

    bind_tetMesh->updateTetVertices(&(time_integraion->q));

}