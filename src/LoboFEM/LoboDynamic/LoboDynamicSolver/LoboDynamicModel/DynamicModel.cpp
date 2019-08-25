#include "DynamicModel.h"

Lobo::DynamicModel::DynamicModel()
{
    num_DOFs = 0;
    is_sparse_sovler = true; // default sparse
}

Lobo::DynamicModel::~DynamicModel()
{
    
}