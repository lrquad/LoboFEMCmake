#pragma once

namespace Lobo {

class CUDABaseLayer {
   public:
    CUDABaseLayer();
    ~CUDABaseLayer();

    virtual void layerRun(float* input, float* output) = 0;
    virtual void allocBuffer();

    int getInput() const { return n_input; }
    void setInput(int val) { n_input = val; }
    int getOutput() const { return n_output; }
    void setOutput(int val) { n_output = val; }

    float* cpu_input;
    float* cpu_output;
    float* cuda_input;
    float* cuda_output;

   protected:
    int n_input;
    int n_output;
};
}  // namespace Lobo
