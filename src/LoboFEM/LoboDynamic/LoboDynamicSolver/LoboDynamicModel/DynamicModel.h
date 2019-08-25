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
    Compute_energy = 0,
    Compute_fisrt = 1 << 0,  // generted mesh by tetgen
    Compute_second = 1 << 1
    };

public:
    DynamicModel();
    ~DynamicModel();

    virtual void computeEnergySparse(double* free_variables,double * energy, double* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags) = 0;
    virtual void computeEnergyDense(double* free_variables,double * energy, double* jacobi, double* hessian, int computationflags) = 0;

    int num_DOFs;
    bool is_sparse_sovler;

protected:


};
} // namespace Lobo