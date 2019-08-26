#pragma once
#include "LoboDynamic/LoboDynamicSolver/LoboTimeIntegration/LoboTimeIntegration.h"

namespace Lobo
{
    class KineticModel;

    class ImplicitSparseIntegration: public LoboTimeIntegration
    {
    private:
        /* data */
    public:
        ImplicitSparseIntegration(KineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_,int skip_steps_, int flags_);
        ~ImplicitSparseIntegration();

        virtual void stepFoward();

    protected:
        KineticModel *kineticmodel;

    };
    
    
    
}