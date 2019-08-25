#pragma once
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"

namespace Lobo
{
    class LoboTetMesh;

    class ConstrainModel:public DynamicModel
    {
        public:
        ConstrainModel();
        ~ConstrainModel();

        virtual void computeEnergySparse(double* free_variables,double * energy, double* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags){};
    virtual void computeEnergyDense(double* free_variables,double * energy, double* jacobi, double* hessian, int computationflags){};

        virtual void loadConstraints(const char* filename);

        protected:


        LoboTetMesh* tetmesh;

        int num_constraints;
        int* constrained_DoFs;
    };
}
