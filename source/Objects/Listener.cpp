#include "Listener.h"

Listener::Listener(Camera* camera) {
	binded_camera = camera;
}

void Listener::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        binded_camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        binded_camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        binded_camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        binded_camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

void Listener::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    binded_camera->ProcessMouseMovement(xoffset, yoffset);
}

void Listener::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	binded_camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
