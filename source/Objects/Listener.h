#ifndef LISTENER_H
#define LISTENER_H

#include "Rendering/Camera.h"
#include "Core/Window.h"

class Listener {
public:
    Listener(Camera* camera);
    ~Listener();

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


private:
    Camera* binded_camera;
    Window* window;
};

#endif // LISTENER_H
