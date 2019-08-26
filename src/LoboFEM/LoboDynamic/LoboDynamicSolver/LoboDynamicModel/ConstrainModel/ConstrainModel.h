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

        virtual void computeEnergySparse(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags){};
    virtual void computeEnergyDense(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::MatrixXd* hessian, int computationflags){};

        virtual void loadConstraints(const char* filename);

        protected:


        LoboTetMesh* tetmesh;

        int num_constraints;
        int* constrained_DoFs;
    };
}
