#pragma once
#include "FullspaceSolver.h"

namespace Lobo
{
class ModalWarpingSovler : public FullspaceSolver
{
public:
    ModalWarpingSovler(Lobo::LoboDynamicScene* parent_scene);
    ~ModalWarpingSovler();

    virtual void precompute();
    virtual void stepForward();
};
} // namespace Lobo