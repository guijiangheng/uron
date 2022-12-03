#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/instance.h"

int main() {
  try {
    uron::Window window{800, 600, "Hello Vulkan"};
    uron::Instance instance;
    auto surface = instance.createSurface(window);

    while (!window.shouldClose()) {
      glfwPollEvents();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
