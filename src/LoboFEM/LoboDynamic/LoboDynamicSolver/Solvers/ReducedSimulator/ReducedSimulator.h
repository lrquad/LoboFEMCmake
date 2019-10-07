#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSimulator.h"

namespace Lobo
{
    
class HyperelasticModel;

class ReducedSimulator : public DynamicSimulator
{
public:
    ReducedSimulator(Lobo::LoboDynamicScene *parent_scene);
    ~ReducedSimulator();

    virtual void drawImGui();

    virtual void runXMLscript(pugi::xml_node &solver_node);

    virtual void precompute();
    virtual void stepForward();
    virtual int getCurStep();

    ConstrainModel *constrainmodel;
    CollisionModel *collisionmodel;
    HyperelasticModel *hyperelastic_model;


};
} // namespace Lobo