#include "AEHyperdualModel.h"
#include "LoboDynamic/LoboKeras/LoboKerasModel.h"
#include "LoboDynamic/LoboKeras/hyperdual/hyperdual.h"


Lobo::AEHyperdualModel::AEHyperdualModel(const char *decoderfilepath, double data_min, double data_scale) : AEAutoDiffModel<double>(decoderfilepath, data_min, data_scale)
{
    //keras_model_hyperdual = new LoboKerasModel<hyperdual>();
    //keras_model_hyperdual->loadNN(decoderfilepath);

    hyperdual mesh_min;
    mesh_min = data_min;

}

Lobo::AEHyperdualModel::~AEHyperdualModel()
{
}