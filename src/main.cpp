#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/instance.h"

int main() {
  const std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  try {
    uron::Window window{800, 600, "Hello Vulkan"};
    uron::Instance instance(validationLayers);
    auto surface = instance.createSurface(window);
    auto device = instance.pickDevice(validationLayers, surface);

    while (!window.shouldClose()) {
      glfwPollEvents();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
