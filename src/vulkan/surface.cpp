#include "uron/vulkan/surface.h"

#include "uron/vulkan/instance.h"

namespace uron {

Surface::Surface(const Instance& instance, const Window& window)
    : instance(instance) {
  VkSurfaceKHR surface;
  VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface),
           "create window surface");
}

Surface::~Surface() { vkDestroySurfaceKHR(instance, surface, nullptr); }

}  // namespace uron
