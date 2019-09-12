#include "LoboDynamic/LoboDynamic.h"

template <class TYPE>
class AEAutoDiffModel;

namespace Lobo
{

class AEElasticModel : public DynamicModel
{
public:
    AEElasticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_, HyperelasticModel *elastic_model_, AEAutoDiffModel<double> *ae_ad_model_);
    ~AEElasticModel();

    LoboTetMesh *tetmesh;
    HyperelasticModel *elastic_model;
    AEAutoDiffModel<double> *ae_ad_model;
}
} // namespace Lobo