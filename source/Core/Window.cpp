#include "Window.h"


static void error_callback(int error, const char* description)
{
    std::cerr << "Error " << error << ": " << description << std::endl;
}

Window::Window(int width, int height, const std::string& title, bool resizable)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

int Window::getHeight()
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return height;
}

int Window::getWidth()
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return width;
}

double Window::getDeltaTime()
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;
    return deltaTime;
}

int Window::getKey(int key)
{
    return glfwGetKey(m_window, key);
}

void Window::setKeyCallback(GLFWkeyfun callback) {
    m_keyCallback = callback;
    glfwSetKeyCallback(m_window, staticKeyCallback);
}

void Window::staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

    // Call the stored key callback function
    if (windowInstance && windowInstance->m_keyCallback) {
        windowInstance->m_keyCallback(window, key, scancode, action, mods);
    }
}
void Window::setMouseCallback(GLFWcursorposfun callback) {
    m_mouseCallback = callback;
    glfwSetCursorPosCallback(m_window, staticMouseCallback);
}

void Window::setScrollCallback(GLFWscrollfun callback) {
    m_scrollCallback = callback;
    glfwSetScrollCallback(m_window, staticScrollCallback);
}

void Window::setFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    m_framebufferSizeCallback = callback;
    glfwSetFramebufferSizeCallback(m_window, staticFramebufferSizeCallback);
}

void Window::staticMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

    // Call the stored mouse callback function
    if (windowInstance && windowInstance->m_mouseCallback) {
        windowInstance->m_mouseCallback(window, xpos, ypos);
    }
}

void Window::staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

    // Call the stored scroll callback function
    if (windowInstance && windowInstance->m_scrollCallback) {
        windowInstance->m_scrollCallback(window, xoffset, yoffset);
    }
}

void Window::staticFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

    // Call the stored framebuffer size callback function
    if (windowInstance && windowInstance->m_framebufferSizeCallback) {
        windowInstance->m_framebufferSizeCallback(window, width, height);
    }
}
