#pragma once

#include "LoboDynamic/LoboDynamicSolver/DynamicSimulator.h"

namespace Lobo
{
class CUDAAETimeIntegration;
class CUDAAEKineticModel;
class CUDAAEAutoDiffModel;
class CUDAKerasModel;

class HyperelasticModel;
class LoboTetMesh;

class CUDAAESimulator : public DynamicSimulator
{
public:
    CUDAAESimulator(Lobo::LoboDynamicScene *parent_scene);
    ~CUDAAESimulator();

    virtual void drawImGui();
    virtual void runXMLscript(pugi::xml_node &sovler_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    ConstrainModel *constrainmodel;
    CollisionModel *collisionmodel;
    HyperelasticModel *hyperelastic_model;

    CUDAAETimeIntegration *time_integration;
    CUDAAEKineticModel *kinetic_model;
    CUDAKerasModel *keras_encoder;
    CUDAAEAutoDiffModel *ae_ad_model;
};

} // namespace Lobo
