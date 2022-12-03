#pragma once

#include <functional>
#include <vector>

#include "uron/common/common.h"

namespace uron {

class Window {
 public:
  NON_COPYABLE(Window);

  Window(int width, int height, const char* title);
  ~Window();

  operator GLFWwindow*() const { return window; }

  bool shouldClose() const { return glfwWindowShouldClose(window); }

  VkExtent2D getFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
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
