#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{
class ReducedKineticModel;

class RedcuedTimeIntegration : public ImplicitDenseIntegration
{
    public:

    RedcuedTimeIntegration(ReducedKineticModel* kineticmodel_,int full_DoFs_, int reduced_DoFs_, double damping_ratio_, double timestep_, int skip_steps_, int flags_);
    ~RedcuedTimeIntegration();

    virtual void precompute();
    virtual void setInitLatentsq(Eigen::VectorXd &latents_q_);
    virtual void stepFoward();

    int reduced_DOFs;
    Eigen::VectorXd q_reduced;
    ReducedKineticModel* reduced_model;
};

} // namespace Lobo