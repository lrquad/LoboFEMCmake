#pragma once
#include "Eigen/Dense"
#include "Eigen/Sparse"
namespace Lobo
{
class LoboDynamicScene;
class LoboTetMesh;

enum ComputationFlag {
    Computeflags_energy = 1 << 0,
    Computeflags_fisrt = 1 << 1,  // generted mesh by tetgen
    Computeflags_second = 1 << 2,
    Computeflags_reset = 1 << 3,
    };

class DynamicModel
{
    

public:
    DynamicModel();
    ~DynamicModel();

    virtual void computeEnergySparse(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags) = 0;
    virtual void computeEnergyDense(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::MatrixXd* hessian, int computationflags) = 0;

    virtual void getSparseTopoloty(Eigen::SparseMatrix<double> &spmatrix){};

    int num_DOFs;
    bool is_sparse_sovler;

protected:


};
} // namespace Lobo