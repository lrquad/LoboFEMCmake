#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Lobo
{

class LoboShader
{
public:
    LoboShader(){};
    ~LoboShader(){};

    virtual void loadShader(const char *vertexShaderSource, const char *fragmentShaderSource);
    virtual void loadShader();

protected:
    int vertex_shader;
    int fragment_shader;
};

} // namespace Lobo
