#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    // Camera Attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;

    // Euler Angles
    float yaw_;
    float pitch_;

    // Camera options
    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;

    // Camera movement
    bool moving_right = 0;
    bool moving_left = 0;
    bool moving_forward = 0;
    bool moving_backward = 0;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
        float pitch = PITCH);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const;

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    float getZoom() const;

    // Processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(CameraMovement direction, bool stop);

    // Processes the camera movement. This is to make movement more fluid.
    void processMovement(float deltaTime);

    // Processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    // Default camera values
    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float SPEED = 2.5f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;
};

#endif // CAMERA_H
