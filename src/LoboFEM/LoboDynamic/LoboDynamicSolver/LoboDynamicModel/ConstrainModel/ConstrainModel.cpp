#include "ConstrainModel.h"
#include <fstream>

Lobo::ConstrainModel::ConstrainModel()
{
    num_constraints = 0;
    constrained_DoFs = NULL;
}

Lobo::ConstrainModel::~ConstrainModel()
{
    free(constrained_DoFs);
}

void Lobo::ConstrainModel::loadConstraints(const char* filename)
{
    std::ifstream inputstream(filename);
    inputstream >> num_constraints;
    constrained_DoFs = (int*)malloc(sizeof(int)*num_constraints);
    for(int i=0;i<num_constraints;i++)
    {
        inputstream >> constrained_DoFs[i];
    }
}
