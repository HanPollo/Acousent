#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const std::string& title, bool resizable = true);
    ~Window();

    GLFWwindow* getGLFWWindow() const { return m_window; }
    bool shouldClose() const;
    void swapBuffers() const;
    void pollEvents() const;

    int getHeight();
    int getWidth();

    double getDeltaTime();
    int getKey(int key);

private:
    GLFWwindow* m_window;
    double m_lastTime = glfwGetTime();
};

#endif // WINDOW_H
