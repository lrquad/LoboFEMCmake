#pragma once
#include "Eigen/Dense"
#include "Eigen/Sparse"
namespace Lobo
{
class LoboDynamicScene;
class LoboTetMesh;

class DynamicModel
{
    enum ComputationFlag {
    Computeflags_energy = 0,
    Computeflags_fisrt = 1 << 0,  // generted mesh by tetgen
    Computeflags_second = 1 << 1,
    Computeflags_reset = 1 << 2,
    };

public:
    DynamicModel();
    ~DynamicModel();

    virtual void computeEnergySparse(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags) = 0;
    virtual void computeEnergyDense(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::MatrixXd* hessian, int computationflags) = 0;

    int num_DOFs;
    bool is_sparse_sovler;

protected:


};
} // namespace Lobo