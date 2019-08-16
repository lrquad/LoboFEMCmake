#include "LoboLighting.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include "Functions/deleteSTDvector.h"
#include "LoboShader.h"

Lobo::LoboLightManager::LoboLightManager() {
    // 3 direction 3 point
    lighting_list.resize(6);
    for (int i = 0; i < lighting_list.size(); i++) {
        lighting_list[i] = new LoboLighting();
    }
    lighting_list[0]->trigger = true;
}

Lobo::LoboLightManager::~LoboLightManager() {
    deleteStdvectorPointer(lighting_list);
}

void Lobo::LoboLightManager::drawImGui(bool* p_open) {
    ImGui::Begin("Lighting control", NULL);
    for (int i = 0; i < lighting_list.size(); i++) {
        ImGui::PushID(i);
        lighting_list[i]->drawImGui();
        ImGui::PopID();
    }
    ImGui::End();
}

void Lobo::LoboLightManager::paintGL(LoboShader* shader) {
    for (int i = 0; i < lighting_list.size(); i++) {
        if (lighting_list[i]->trigger) lighting_list[i]->paintGL(shader);
    }
}

void Lobo::LoboLightManager::setLight(LoboShader* render_shader) {
    for (int i = 0; i < lighting_list.size(); i++) {
        lighting_list[i]->setLight(render_shader, i);
    }
}

Lobo::LoboLighting::LoboLighting() {
    lightPos = glm::vec3(0.0f, 1.0f, 1.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    direction = glm::vec3(0.0f, -1.0f, -1.0f);
    constant = 1.0;
    linear = 0.09;
    quadratic = 0.032;
    light_type = 0;
    trigger = false;

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // we only need to bind to the VBO (to link it with glVertexAttribPointer),
    // no need to fill it; the VBO's data already contains all we need (it's
    // already bound, but we do it again for educational purposes)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Lobo::LoboLighting::drawImGui() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Checkbox("On/Off", &trigger);
    ImGui::SameLine();
    const char* items[] = {"Point light", "Directional light"};
    ImGui::Combo("combo", &light_type, items, IM_ARRAYSIZE(items));
    if (trigger) {
        ImGui::DragFloat("lightPos.x", &lightPos.x, 0.05f);
        ImGui::DragFloat("lightPos.y", &lightPos.y, 0.05f);
        ImGui::DragFloat("lightPos.z", &lightPos.z, 0.05f);
        ImGui::ColorEdit3("light color", &lightColor.r);
        if (light_type == 0) drawPointLightImGui();
        if (light_type == 1) drawDirectionalLightImGui();

    }
}

void Lobo::LoboLighting::paintGL(LoboShader* shader) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
    shader->setMat4("model", model);
    shader->setVec3("lightColor", lightColor);

    glBindVertexArray(lightVAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Lobo::LoboLighting::drawPointLightImGui() {
    if (ImGui::CollapsingHeader("PointLight")) {
        ImGui::DragFloat("constant", &constant, 0.1f);
        ImGui::DragFloat("linear", &linear, 0.01f);
        ImGui::DragFloat("quadratic", &quadratic, 0.001f);
    }
}

void Lobo::LoboLighting::drawDirectionalLightImGui() {
    if (ImGui::CollapsingHeader("PointLight")) {
        ImGui::DragFloat("direction.x", &direction.x, 0.05f);
        ImGui::DragFloat("direction.y", &direction.y, 0.05f);
        ImGui::DragFloat("direction.z", &direction.z, 0.05f);
    }
}

void Lobo::LoboLighting::setLight(LoboShader* render_shader, int lightid) {
    std::ostringstream stringStream;
    stringStream << "lights[" << lightid << "]";
    std::string lightname = stringStream.str();

    if (light_type == 0) {
        setPointLight(render_shader, lightname);
    }
    if (light_type == 1) {
        setDirectionalLight(render_shader, lightname);
    }

    render_shader->setInt(lightname + ".light_type", light_type);
    render_shader->setBool(lightname + ".trigger", trigger);
}

void Lobo::LoboLighting::setPointLight(LoboShader* render_shader,
                                       std::string lightname) {
    render_shader->setVec3(lightname + ".position", lightPos);
    render_shader->setVec3(lightname + ".lightColor", lightColor);
    render_shader->setFloat(lightname + ".constant", constant);
    render_shader->setFloat(lightname + ".linear", linear);
}

void Lobo::LoboLighting::setDirectionalLight(LoboShader* render_shader,std::string lightname)
{
    render_shader->setVec3(lightname + ".direction", direction);
}