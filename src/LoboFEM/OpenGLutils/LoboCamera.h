#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "imgui.h"
#include "Utils/simpleini/SimpleIni.h"
#include "Utils/simpleini/SimpleiniWarp.h"

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UPWARD, DOWNWARD };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const bool FLYMODE = false;

namespace Lobo {
// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
struct InitCamera{
    glm::vec3 Position;
    glm::vec3 CenterP;
    glm::vec3 Front;
    // Euler Angles
    float Yaw;
    float Pitch;
} ;

    class Camera {
   public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 CenterP;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool flymode;

    glm::vec2 mouse_position;

    InitCamera init_camera_value;

    //CSimpleIniA camera_ini_io;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
           float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          CenterP(glm::vec3(0.0f, 0.0f, 0.0f)),
          MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY),
          Zoom(ZOOM),
          flymode(FLYMODE) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        init_camera_value.Position = Position;
        init_camera_value.CenterP = CenterP;
        init_camera_value.Front = Front;
        init_camera_value.Yaw = Yaw;
        init_camera_value.Pitch = Pitch;

        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(SPEED),
          CenterP(glm::vec3(0.0f, 0.0f, 0.0f)),
          MouseSensitivity(SENSITIVITY),
          Zoom(ZOOM),
          flymode(FLYMODE) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        init_camera_value.Position = Position;
        init_camera_value.CenterP = CenterP;
        init_camera_value.Front = Front;
        init_camera_value.Yaw = Yaw;
        init_camera_value.Pitch = Pitch;
        updateCameraVectors();
    }

    void drawImGui(bool *p_open = NULL) {
        ImGui::Begin("Camera control", p_open,
                     ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Camera position: %.3f %.3f %.3f ", Position.x, Position.y,
                    Position.z);
        ImGui::Text("Rotation center: %.3f %.3f %.3f ", CenterP.x, CenterP.y,
                    CenterP.z);
        ImGui::Text("Yaw: %.3f Pitch: %.3f", Yaw, Pitch);
        ImGui::Checkbox("Flymode", &flymode);
        ImGui::InputFloat("MovementSpeed", &MovementSpeed, 0.1f, 1.0f, "%.3f");
        ImGui::InputFloat("MouseSensitivity", &MouseSensitivity, 0.01f, 1.0f,
                          "%.3f");
        if (ImGui::Button("Reset center")) {
            CenterP = glm::vec3(0.0);
            updateCameraVectors();
        };
        ImGui::SameLine();
        if (ImGui::Button("Reset Camera")) {
            Position = init_camera_value.Position;
            CenterP = init_camera_value.CenterP;
            Front = init_camera_value.Front;
            Yaw = init_camera_value.Yaw;
            Pitch = init_camera_value.Pitch;
            updateCameraVectors();
        };
        if (ImGui::Button("Save Camera")) {
            CSimpleIniA ini;
            char buffer [50];
            ini.SetUnicode();
            Lobo::setValueVec3(&ini,"Camera","Position",Position);
            Lobo::setValueVec3(&ini,"Camera","CenterP",CenterP);
            Lobo::setValuef(&ini,"Camera","Yaw",Yaw);
            Lobo::setValuef(&ini,"Camera","Pitch",Pitch);
            ini.SaveFile("./config/camera.ini");
        };
        ImGui::SameLine();
        if (ImGui::Button("Load Camera")) {
            CSimpleIniA ini;
            ini.SetUnicode();
            ini.LoadFile("./config/camera.ini");
            Lobo::getValueVec3(&ini,"Camera","Position",&Position);
            Lobo::getValueVec3(&ini,"Camera","CenterP",&CenterP);
            Lobo::getValuef(&ini,"Camera","Yaw",&Yaw);
            Lobo::getValuef(&ini,"Camera","Pitch",&Pitch);
            updateCameraVectors();
        };

        // ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::End();

        flythroughMoveMode();
    }

    void flythroughMoveMode() {
        ImGuiIO &io = ImGui::GetIO();

        float deltaTime = 1.0f / ImGui::GetIO().Framerate;

        if (!io.WantCaptureMouse) {
            if (flymode) {
                if (io.KeysDownDuration[int('W')] >= 0.0f) {
                    ProcessKeyboard(FORWARD, deltaTime);
                }
                if (io.KeysDownDuration[int('A')] >= 0.0f) {
                    ProcessKeyboard(LEFT, deltaTime);
                }
                if (io.KeysDownDuration[int('S')] >= 0.0f) {
                    ProcessKeyboard(BACKWARD, deltaTime);
                }
                if (io.KeysDownDuration[int('D')] >= 0.0f) {
                    ProcessKeyboard(RIGHT, deltaTime);
                }
                if (io.KeysDownDuration[341] >= 0.0f) {
                    ProcessKeyboard(DOWNWARD, deltaTime);
                }
                if (io.KeysDownDuration[32] >= 0.0f) {
                    ProcessKeyboard(UPWARD, deltaTime);
                }
            }

            if (io.MouseDownDuration[0] >= 0.0f) {
                ProcessMouseMovement(io.MouseDelta.x, io.MouseDelta.y);
            }

            if (io.MouseDownDuration[2] >= 0.0f) {
                ProcessMiddleMouseMovement(io.MouseDelta.x, io.MouseDelta.y);
            }

            ProcessMouseScroll(io.MouseWheel);
        }
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt
    // Matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts
    // input parameter in the form of camera defined ENUM (to abstract it from
    // windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            Position += Front * velocity;
            CenterP += Front * velocity;
        }
        if (direction == BACKWARD) {
            Position -= Front * velocity;
            CenterP -= Front * velocity;
        }
        if (direction == LEFT) {
            Position -= Right * velocity;
            CenterP -= Right * velocity;
        }
        if (direction == RIGHT) {
            Position += Right * velocity;
            CenterP += Right * velocity;
        }
        if (direction == DOWNWARD) {
            Position -= Up * velocity;
            CenterP -= Up * velocity;
        }
        if (direction == UPWARD) {
            Position += Up * velocity;
            CenterP += Up * velocity;
        }
    }

    // Processes input received from a mouse input system. Expects the offset
    // value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset,
                              GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch -= yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get
        // flipped
        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void ProcessMiddleMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity * 0.1;
        yoffset *= MouseSensitivity * 0.1;

        // Yaw += xoffset;
        // Pitch -= yoffset;
        Position += Up * yoffset;
        Position -= Right * xoffset;
        CenterP += Up * yoffset;
        CenterP -= Right * xoffset;
        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires
    // input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
        yoffset *= MouseSensitivity*2.0;
        Position += Front * yoffset;
        // CenterP += Front * yoffset;
    }

   private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(
            Front, WorldUp));  // Normalize the vectors, because their length
                               // gets closer to 0 the more you look up or down
                               // which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        if (flymode == false) {
            float len = glm::length(Position - CenterP);
            Position = -Front * len + CenterP;
        }
    }
};
}  // namespace Lobo
#endif