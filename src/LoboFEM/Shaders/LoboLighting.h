#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include <vector>
#include <string>

namespace Lobo {
static float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  0.5f,
    -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
    0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f,
    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
};
class LoboShader;
class LoboLighting;

class LoboLightManager{
    public:
    LoboLightManager();
    ~LoboLightManager();

    virtual void drawImGui(bool * p_open = NULL);
    virtual void paintGL(LoboShader* shader);
    virtual void setLight(LoboShader* render_shader);

    std::vector<LoboLighting*> lighting_list;

};


class LoboLighting {
   public:
    LoboLighting();
    ~LoboLighting(){};

    virtual void drawImGui();
    virtual void drawPointLightImGui();
    virtual void drawDirectionalLightImGui();
    virtual void paintGL(LoboShader* shader);
    virtual void setLight(LoboShader* render_shader,int lightid);
    virtual void setPointLight(LoboShader* render_shader,std::string lightname);
    virtual void setDirectionalLight(LoboShader* render_shader,std::string lightname);

    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 direction;

    float constant;
    float linear;
    float quadratic;
    bool trigger;
    int light_type;

   protected:
    unsigned int lightVAO;
};


}  // namespace Lobo