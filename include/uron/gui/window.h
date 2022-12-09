#pragma once

#include <functional>
#include <vector>

#include "uron/common.h"

namespace uron {

class Window {
 public:
  NON_COPYABLE(Window);

  Window(int width, int height, const char* title, bool resizable = true);
  ~Window();

  operator GLFWwindow*() const { return window; }

  bool shouldClose() const { return glfwWindowShouldClose(window); }

  VkExtent2D getFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  void addOnResize(std::function<void(int width, int height)> callback) {
    onResizeChanged.emplace_back(callback);
  }

  void addOnKeyChanged(
      std::function<void(int key, int scancode, int action, int mods)>
          callback) {
    onKeyChanged.emplace_back(callback);
  }

  void addOnCursorPositionChanged(
      std::function<void(double xpos, double ypos)> callback) {
    onCursorPositionChanged.emplace_back(callback);
  }

  void addOnMouseButtonChanged(
      std::function<void(int button, int action, int mods)> callback) {
    onMouseButtonChanged.emplace_back(callback);
  }

  void addOnScrollChanged(
      std::function<void(double xoffset, double yoffset)> callback) {
    onScrollChanged.emplace_back(callback);
  }

 private:
  static void glfwErrorCallback(int error, const char* description);

  static void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods);

  static void glfwCursorPositionCallback(GLFWwindow* window, double xpos,
                                         double ypos);

  static void glfwMouseButtonCallback(GLFWwindow* window, int button,
                                      int action, int mods);

  static void glfwScrollCallback(GLFWwindow* window, double xoffset,
                                 double yoffset);

  std::vector<std::function<void(int width, int height)>> onResizeChanged;
  std::vector<std::function<void(int key, int scancode, int action, int mods)>>
      onKeyChanged;
  std::vector<std::function<void(double xpos, double ypos)>>
      onCursorPositionChanged;
  std::vector<std::function<void(int button, int action, int mods)>>
      onMouseButtonChanged;
  std::vector<std::function<void(double xoffset, double yoffset)>>
      onScrollChanged;

  GLFWwindow* window;
};

}  // namespace uron
