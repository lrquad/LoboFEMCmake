#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{

    class AEKineticModel;

    class AETimeIntegration:public ImplicitDenseIntegration
    {
    private:
        /* data */
    public:
        AETimeIntegration(AEKineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_,int skip_steps_, int flags_);
        ~AETimeIntegration();

        virtual void precompute();
        
        virtual void setInitLatentsq(Eigen::VectorXd &latents_q_);
        virtual void stepFoward();

        int num_latents;
        Eigen::VectorXd q_latents;
        AEKineticModel* ae_model;
    };

} // namespace Lobo
