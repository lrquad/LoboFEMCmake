#include "HyperelasticModel.h"
#include "LoboDynamic/LoboDynamic.h"
#include "AutoDiff/AutoDiffCore.h"
typedef Eigen::Triplet<double> EIGEN_TRI;

Lobo::HyperelasticModel::HyperelasticModel(Lobo::LoboDynamicScene *scene_,
                                           Lobo::LoboTetMesh *tetmesh_)
    : scene(scene_), tetmesh(tetmesh_) {
    elastic_material = NULL;
    is_sparse_sovler = true;
    num_DOFs = tetmesh_->getNumVertex() * 3;
}

Lobo::HyperelasticModel::~HyperelasticModel() {
    delete elastic_material;

    int numElements = tetmesh->getNumElements();

    for (int i = 0; i < numElements; i++) {
        delete internalforce_list[i];
        delete stiffness_list[i];
    }
}

void Lobo::HyperelasticModel::runXMLscript(pugi::xml_node &xml_node) {
    this->tetmesh->precomputeElementData();

    if (xml_node.child("Material")) {
        if (strcmp("StVK",
                   xml_node.child("Material").attribute("type").as_string()) ==
            0) {
            this->elastic_material =
                new TypeStVKMaterial<double>(tetmesh, 1, 500.0);
            materialtype = "StVK";
        }
    }
}

void Lobo::HyperelasticModel::precompute() {
    this->tetmesh->precomputeElementData();  // will check if already
                                             // precomptued inside the function

    // Dm Dm_inverse per element
    // volume per element

    // precompute sparse matrix topology
    precomputedFdU();
    initMultiThreadBuffer();
}

void Lobo::HyperelasticModel::computeEnergySparse(Eigen::VectorXd*free_variables, double *energy, Eigen::VectorXd*jacobi, Eigen::SparseMatrix<double> *hessian, int computationflags)
{

}

void Lobo::HyperelasticModel::setAccelerationIndices(int **row, int **column) {
    this->row_ = row;
    this->column_ = column_;
}

void Lobo::HyperelasticModel::computeStiffnessMatrixTopology(
    Eigen::SparseMatrix<double> *K) {
    K->resize(num_DOFs, num_DOFs);
    int numVertices = tetmesh->getNumVertex();
    int numElements = tetmesh->getNumElements();

    // tetmesh 4 nodes
    int numElementVertices = tetmesh->numElementVertices;
    std::vector<int> vertices(numElementVertices);
    std::vector<EIGEN_TRI> entrys;

    for (int ele = 0; ele < numElements; ele++) {
        for (int ver = 0; ver < numElementVertices; ver++) {
            vertices[ver] =
                tetmesh->tet_indices[ele * numElementVertices + ver];
        }

        for (int i = 0; i < numElementVertices; i++) {
            for (int j = 0; j < numElementVertices; j++) {
                for (int k = 0; k < 3; k++)
                    for (int l = 0; l < 3; l++) {
                        // non-fixed
                        entrys.push_back(EIGEN_TRI(3 * vertices[i] + k,
                                                   3 * vertices[j] + l, 1.0));
                    }
            }
        }
    }

    K->setFromTriplets(entrys.begin(), entrys.end());
}

void Lobo::HyperelasticModel::precomputedFdU() {
    int numElement = tetmesh->getNumElements();

    std::vector<Eigen::Matrix3d> dF_tet(12);

    for (int i = 0; i < numElement; i++) {
        this->diffDeformationGradient(i, dF_tet);
        dF.push_back(dF_tet);

        Eigen::MatrixXd dF_du_tet(9, 12);
        Eigen::MatrixXd dF_du_tet9X9(9, 9);

        for (size_t j = 0; j < 12; j++) {
            for (size_t i = 0; i < 9; i++) {
                dF_du_tet(i, j) = dF_tet[j].data()[i];
            }
        }
        for (size_t j = 0; j < 9; j++) {
            for (size_t i = 0; i < 9; i++) {
                dF_du_tet9X9(i, j) = dF_tet[j].data()[i];
            }
        }

        dF_du.push_back(dF_du_tet);
        dF_du_9X9.push_back(dF_du_tet9X9);
    }
}

void Lobo::HyperelasticModel::initMultiThreadBuffer() {
    int numElements = tetmesh->getNumElements();
    internalforce_list.resize(numElements);
    stiffness_list.resize(numElements);
    for (int i = 0; i < numElements; i++) {
        internalforce_list[i] = new Eigen::VectorXd(12);
        stiffness_list[i] = new Eigen::MatrixXd(12, 12);
    }
    energy_list.resize(numElements);
}

void Lobo::HyperelasticModel::diffDeformationGradient(
    int elementid, std::vector<Eigen::Matrix3d> &dF) {
    Eigen::Matrix3d dDs[12];
    Eigen::Matrix3d dm_inverse;
    dm_inverse = tetmesh->getTetElement(elementid)->Dm_inverse;

    for (size_t i = 0; i < 9; i++) {
        dDs[i].setZero();
        dDs[i](i % 3, i / 3) = 1;
        dF[i] = dDs[i] * dm_inverse;
    }

    for (size_t i = 0; i < 3; i++) {
        dDs[i + 9].setZero();
        dDs[i + 9](i, 0) = -1;
        dDs[i + 9](i, 1) = -1;
        dDs[i + 9](i, 2) = -1;
        dF[i + 9] = dDs[i + 9] * dm_inverse;
    }
}