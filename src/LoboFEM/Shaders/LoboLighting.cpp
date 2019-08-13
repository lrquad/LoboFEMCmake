#include "LoboLighting.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "LoboShader.h"

Lobo::LoboLighting::LoboLighting() {
    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

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
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Lobo::LoboLighting::drawImGui(bool *p_open) {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Begin("Lighting control", p_open, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::DragFloat("lightPos.x", &lightPos.x, 0.05f);
    ImGui::DragFloat("lightPos.y", &lightPos.y, 0.05f);
    ImGui::DragFloat("lightPos.z", &lightPos.z, 0.05f);
    ImGui::ColorEdit3("light color", &lightColor.r);
    ImGui::End();
}

void Lobo::LoboLighting::paintGL(LoboShader *shader) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
    shader->setMat4("model", model);
    shader->setVec3("lightColor", lightColor);

    glBindVertexArray(lightVAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}