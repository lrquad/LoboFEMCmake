#pragma once
#include "AEAutoDiffModel.h"

class hyperdual;
namespace Lobo
{
class AEHyperdualModel : public AEAutoDiffModel<double>
{

public:
    AEHyperdualModel(const char* decoderfilepath,double data_min,double data_scale);
    ~AEHyperdualModel();


protected:

    LoboKerasModel<hyperdual>* keras_model_hyperdual;
    hyperdual* input_complex;
	hyperdual* output_complex;

};
} // namespace Lobo
