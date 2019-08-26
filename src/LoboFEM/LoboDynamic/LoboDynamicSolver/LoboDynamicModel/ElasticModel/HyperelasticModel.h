#pragma once
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

template <class TYPE>
class TypeIsotropicMaterial;

namespace Lobo
{

class HyperelasticModel : public DynamicModel
{
private:
    /* data */
public:
    HyperelasticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_);
    ~HyperelasticModel();

    virtual void precompute();
    virtual void setAccelerationIndices(int **row_, int **column_);
    virtual void computeStiffnessMatrixTopology(Eigen::SparseMatrix<double> *K);

    virtual void runXMLscript(pugi::xml_node &xml_node);

    virtual void computeEnergySparse(Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi, Eigen::SparseMatrix<double> *hessian, int computationflags);
    virtual void computeEnergyDense(Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi, Eigen::MatrixXd *hessian, int computationflags){};

    std::string materialtype;
    bool isinvertible;
    double inversion_Threshold;

protected:
    virtual void precomputedFdU();
    virtual void initMultiThreadBuffer();

    virtual void computeElementDeformationshapeMatrix(int eleid, Eigen::Matrix3d &Ds);
    virtual void diffDeformationGradient(int elementid, std::vector<Eigen::Matrix3d> &dF);

    virtual void getTetForceMatrixCSFD(int eleid, Eigen::VectorXd *internalforce, Eigen::MatrixXd *stiffness);
    virtual void getTetForceCSFD(int eleid, Eigen::VectorXd *internalforce);

	virtual void assignTetElementForceAndMatrix(int eleid,double* energy, Eigen::VectorXd *internalforce, Eigen::SparseMatrix<double>* sparseMatrix, int flags, double weights = 1.0);

    LoboDynamicScene *scene;
    LoboTetMesh *tetmesh;
    TypeIsotropicMaterial<double> *elastic_material;

    Eigen::VectorXd currentdisplacement; //buffer

    //precompute
    std::vector<std::vector<Eigen::Matrix3d>> dF; // 12* numelements
    std::vector<Eigen::MatrixXd> dF_du;           // 9X12
    std::vector<Eigen::MatrixXd> dF_du_9X9;       // 9X12

    std::vector<Eigen::VectorXd *> internalforce_list;
    std::vector<Eigen::MatrixXd *> stiffness_list;
    std::vector<double> energy_list;

    // acceleration indices
    int **row_;
    int **column_;
};
} // namespace Lobo