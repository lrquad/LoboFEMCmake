#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSimulator.h"

template <class TYPE>
class AEAutoDiffModel;

template <class TYPE>
class LoboKerasModel;

namespace Lobo
{

class HyperelasticModel;
class AETimeIntegration;
class LoboTetMesh;
class AEKineticModel;


class AESimulator : public DynamicSimulator
{
public:
    AESimulator(Lobo::LoboDynamicScene *parent_scene);
    ~AESimulator();

    virtual void drawImGui();

    virtual void runXMLscript(pugi::xml_node &solver_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    ConstrainModel *constrainmodel;
    CollisionModel *collisionmodel;
    HyperelasticModel *hyperelastic_model;
    
    AEKineticModel *kinetic_model;
    AETimeIntegration *time_integraion;

    AEAutoDiffModel<double> *ae_ad_model;
    LoboKerasModel<double>* keras_encoder;


};

} // namespace Lobo