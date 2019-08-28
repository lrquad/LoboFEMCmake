#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSolver.h"

namespace Lobo
{
class HyperelasticModel;
class KineticModel;
class LoboTimeIntegration;
class LoboTetMesh;

class FullspaceSolver : public DynamicSolver
{
public:
    FullspaceSolver(Lobo::LoboDynamicScene *parent_scene);
    ~FullspaceSolver();

    virtual void drawImGui();

    virtual void runXMLscript(pugi::xml_node &solver_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    ConstrainModel *constrainmodel;
    CollisionModel *collisionmodel;
    HyperelasticModel *hyperelastic_model;
    KineticModel *kinetic_model;
    LoboTimeIntegration *time_integraion;
};
} // namespace Lobo