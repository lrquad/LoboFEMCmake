#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSimulator.h"
#include "LoboMesh/LoboMesh.h"

namespace Lobo {
class LoboTetMesh;

class GraspFingerSimulator : public DynamicSimulator {
   public:
    GraspFingerSimulator(Lobo::LoboDynamicScene *parent_scene);
    ~GraspFingerSimulator();

    virtual void drawImGui();

    virtual void runXMLscript(pugi::xml_node &solver_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    protected:
    Lobo::LoboMesh* trimesh;
    int contact_index;
};

}  // namespace Lobo