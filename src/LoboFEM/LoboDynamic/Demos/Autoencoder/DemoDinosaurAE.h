#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSolver.h"

namespace Lobo
{

class AEElasticModel;
class HyperelasticModel;
class LoboTimeIntegration;
class LoboTetMesh;
class AEKineticModel;

class DemoDinosaurAE : public DynamicSolver
{
public:
    DemoDinosaurAE(Lobo::LoboDynamicScene *parent_scene);
    ~DemoDinosaurAE();

    virtual void drawImGui();

    virtual void runXMLscript(pugi::xml_node &solver_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    ConstrainModel *constrainmodel;
    CollisionModel *collisionmodel;
    HyperelasticModel *hyperelastic_model;
    AEKineticModel *kinetic_model;
    LoboTimeIntegration *time_integraion;
};

} // namespace Lobo