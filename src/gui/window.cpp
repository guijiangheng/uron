#include "uron/gui/window.h"

#include <iostream>

namespace uron {

void Window::glfwErrorCallback(const int error, const char* const description) {
  std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")"
            << std::endl;
}

void Window::glfwKeyCallback(GLFWwindow* window, const int key,
                             const int scancode, const int action,
                             const int mods) {
  auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  for (auto& callback : self->onKeyChanged)
    callback(key, scancode, action, mods);
}

void Window::glfwCursorPositionCallback(GLFWwindow* window, const double xpos,
                                        const double ypos) {
  auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  for (auto& callback : self->onCursorPositionChanged) callback(xpos, ypos);
}

void Window::glfwMouseButtonCallback(GLFWwindow* window, const int button,
                                     const int action, const int mods) {
  auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  for (auto& callback : self->onMouseButtonChanged)
    callback(button, action, mods);
}

void Window::glfwScrollCallback(GLFWwindow* window, const double xoffset,
                                const double yoffset) {
  auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  for (auto& callback : self->onScrollChanged) callback(xoffset, yoffset);
}

Window::Window(int width, int height, const char* title) {
  glfwSetErrorCallback(glfwErrorCallback);

  if (!glfwInit()) {
    throw std::runtime_error("glfwInit() failed!");
  }

  if (!glfwVulkanSupported()) {
    throw std::runtime_error("glfwVulkanSupported() failed!");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(width, height, "Uron", nullptr, nullptr);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, glfwKeyCallback);
  glfwSetCursorPosCallback(window, glfwCursorPositionCallback);
  glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
  glfwSetScrollCallback(window, glfwScrollCallback);
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace uron
