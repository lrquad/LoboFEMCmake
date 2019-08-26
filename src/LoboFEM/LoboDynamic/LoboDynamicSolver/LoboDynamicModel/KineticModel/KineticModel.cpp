#include "KineticModel.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/ElasticModel/HyperelasticModel.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/ConstrainModel/ConstrainModel.h"

Lobo::KineticModel::KineticModel(LoboDynamicScene *scene_,
                                 LoboTetMesh *tetmesh_,
                                 HyperelasticModel *elastic_model_,
                                 ConstrainModel *constrain_model_)
    : scene(scene_),
      tetmesh(tetmesh_),
      hyperelasticmodel(elastic_model_),
      constrainmodel(constrain_model_) {

    num_DOFs = tetmesh->getNumVertex() * 3;  // all vertices are free variables
    timestep = 0;
    row_ = NULL;
    column_ = NULL;
    external_forces.resize(num_DOFs);
    external_forces.setZero();
}

Lobo::KineticModel::~KineticModel() {
    int numElements = tetmesh->getNumElements();
    for (int i = 0; i < numElements; i++) free(row_[i]);
    free(row_);

    for (int i = 0; i < numElements; i++) free(column_[i]);
    free(column_);
}

// precomptue final sparse matrix topology
void Lobo::KineticModel::precompute() {
    std::cout << "KineticModel precompute" << std::endl;
    tetmesh->computeDiagMassMatrix(&mass_matrix);
    hyperelasticmodel->computeStiffnessMatrixTopology(
        &stiffness_matrix_topology);
    stiffness_matrix = stiffness_matrix_topology;
    // in this model, the final system matrix has the same topology as stiffness
    // matrix
    SparseMatrixTopologyTYPE<double> sparseMatrixTopology(
        &stiffness_matrix_topology);

    computeAccelerationIndices(&sparseMatrixTopology);

    hyperelasticmodel->precompute();
    hyperelasticmodel->setAccelerationIndices(row_, column_);

    //precomptue gravity force
    gravity_force.resize(num_DOFs);
    for(int i=0;i<num_DOFs/3;i++)
    {
        gravity_force.data()[i*3+1] = -9.8;
    }
    gravity_force = mass_matrix*gravity_force;


    //test 
    

}

void Lobo::KineticModel::computeEnergySparse(
    Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi,
    Eigen::SparseMatrix<double> *hessian, int computationflags) {
    

    Eigen::VectorXd elastic_force(num_DOFs);
    //jacobi = - internal force
    hyperelasticmodel->computeEnergySparse(free_variables, energy,
                                           jacobi, hessian,
                                           computationflags);
    double elastic_energy = energy;
    
    //just add value to stiffness_matrix
    computationflags &= ~Computeflags_reset;

    Eigen::VectorXd constrain_force(num_DOFs);
    constrainmodel->computeEnergySparse(free_variables, energy,
                                        jacobi, hessian,
                                        computationflags);
    double constrain_energy = energy-elastic_energy;


    //kinetic parts
    Eigen::VectorXd q_buffer = (q - q_1 - timestep*q_vel);

    //energy
    double inv_t = 1.0/timestep;
    Eigen::VectorXd e = q_buffer.transpose()*mass_matrix*q_buffer;
	*energy += e.data()[0] * inv_t*inv_t*0.5;

    //jacobi
    Eigen::VectorXd all_ext = -mass_matrix*(q_buffer) / (timestep*timestep) + external_forces + gravity_force;
    *jacobi += all_ext;
}

void Lobo::KineticModel::runXMLscript(pugi::xml_node &xml_node) {}

void Lobo::KineticModel::setKineticStatus(Eigen::VectorXd &q_,
                                          Eigen::VectorXd &q_vel_,
                                          Eigen::VectorXd &q_1_) {
    this->q = q_;
    this->q_vel = q_vel_;
    this->q_1 = q_1_;
}

void Lobo::KineticModel::computeAccelerationIndices(
    SparseMatrixTopologyTYPE<double> *sparsetopology) {
    int numElements = tetmesh->getNumElements();

    if (row_ != NULL) {
        for (int i = 0; i < numElements; i++) free(row_[i]);
        free(row_);
    }

    if (column_ != NULL) {
        for (int i = 0; i < numElements; i++) free(column_[i]);
        free(column_);
    }

    row_ = (int **)malloc(sizeof(int *) * numElements);
    column_ = (int **)malloc(sizeof(int *) * numElements);

    int numElementVertices = tetmesh->numElementVertices;
    for (int el = 0; el < numElements; el++) {
        // std::cout << el << "\r";
        row_[el] = (int *)malloc(sizeof(int) * numElementVertices);
        column_[el] = (int *)malloc(sizeof(int) * numElementVertices *
                                    numElementVertices * 3 * 3);

        for (int ver = 0; ver < numElementVertices; ver++) {
            row_[el][ver] = tetmesh->tet_indices[el * numElementVertices + ver];
        }

        for (int i = 0; i < numElementVertices; i++)
            for (int j = 0; j < numElementVertices; j++) {
                for (int k = 0; k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        int block_r = i * 3 + k;
                        int block_c = j * 3 + l;

                        column_[el][3 * numElementVertices * block_c +
                                    block_r] =
                            sparsetopology->getValueIndex(3 * row_[el][i] + k,
                                                          3 * row_[el][j] + l);
                    }
                }
            }
    }
}