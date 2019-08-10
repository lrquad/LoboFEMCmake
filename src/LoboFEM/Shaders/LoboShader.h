#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Lobo
{

class LoboShader
{
public:
    LoboShader(){};
    ~LoboShader(){};

    virtual void drawImGui(bool *p_open = NULL);

    virtual void loadShaderFile(const char *vertexShaderfile, const char *fragmentShaderfile);
    virtual void loadShaderStream(const char *vertexShaderSource, const char *fragmentShaderSource);
    virtual void loadShader();
    virtual void useProgram();


    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setFloat4(const std::string &name, float v0, float v1, float v2, float v3) const;


protected:
    virtual std::string readFile(const char *filename);

    
    int vertex_shader;
    int fragment_shader;
    int shader_program;
};

} // namespace Lobo
