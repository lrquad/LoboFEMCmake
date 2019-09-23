#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{
class CUDAAEKineticModel;
class CUDAAETimeIntegration : public ImplicitDenseIntegration
{
public:
    CUDAAETimeIntegration(CUDAAEKineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_, int skip_steps_, int flags_);
    ~CUDAAETimeIntegration();

    virtual void precompute();
    virtual void setInitLatentsq(Eigen::VectorXd &latents_q_);
    virtual void stepFoward();

    int num_latents;
    Eigen::VectorXd q_latents;
    CUDAAEKineticModel *ae_model;
};
} // namespace Lobo