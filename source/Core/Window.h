#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "../Objects/Seat.h"

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

    void setKeyCallback(GLFWkeyfun callback);
    void setMouseCallback(GLFWcursorposfun callback);
    void setScrollCallback(GLFWscrollfun callback);
    void setFramebufferSizeCallback(GLFWframebuffersizefun callback);


private:
    GLFWwindow* m_window;
    double m_lastTime = glfwGetTime();

    GLFWcursorposfun m_mouseCallback;         // The mouse callback function
    GLFWscrollfun m_scrollCallback;           // The scroll callback function
    GLFWframebuffersizefun m_framebufferSizeCallback;  // The framebuffer size callback function
    GLFWkeyfun m_keyCallback;  // The key callback function

    // static callback functions
    static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void staticFramebufferSizeCallback(GLFWwindow* window, int width, int height);


};

#endif // WINDOW_H
