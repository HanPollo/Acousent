#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position_(position), worldUp_(up), yaw_(yaw), pitch_(pitch), front_(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed_(SPEED),
    mouseSensitivity_(SENSITIVITY), zoom_(ZOOM)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

float Camera::getZoom() const
{
    return zoom_;
}

void Camera::processKeyboard(CameraMovement direction, bool stop)
{
    if (direction == CameraMovement::FORWARD && stop) {
        moving_forward = 0;
    }
    else if (direction == CameraMovement::BACKWARD && stop) {
        moving_backward = 0;
    }
    else if (direction == CameraMovement::LEFT && stop) {
        moving_left = 0;
    }
    else if (direction == CameraMovement::RIGHT && stop) {
        moving_right = 0;
    }

    if (direction == CameraMovement::FORWARD && !stop) {
        moving_forward = 1;
    }
    else if (direction == CameraMovement::BACKWARD && !stop) {
        moving_backward = 1;
    }
    else if (direction == CameraMovement::LEFT && !stop) {
        moving_left = 1;
    }
    else if (direction == CameraMovement::RIGHT && !stop) {
        moving_right = 1;
    }
}

void Camera::processMovement(float deltaTime)
{
    float velocity = movementSpeed_ * deltaTime;
    if (moving_backward) position_ -= front_ * velocity;
    if (moving_forward) position_ += front_ * velocity;
    if (moving_left) position_ -= right_ * velocity;
    if (moving_right) position_ += right_ * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (constrainPitch) {
        if (pitch_ > 89.0f) {
            pitch_ = 89.0f;
        }
        else if (pitch_ < -89.0f) {
            pitch_ = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    if (zoom_ >= 1.0f && zoom_ <= 45.0f) {
        zoom_ -= yoffset;
    }
    if (zoom_ <= 1.0f) {
        zoom_ = 1.0f;
    }
    if (zoom_ >= 45.0f) {
        zoom_ = 45.0f;
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);

    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
