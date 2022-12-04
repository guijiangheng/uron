#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Instance;
class Window;

class Surface {
 public:
  NON_COPYABLE(Surface);

  Surface(const Instance& instance, const Window& window);

  ~Surface();

  operator VkSurfaceKHR() const { return surface; }

 private:
  const Instance& instance;
  VkSurfaceKHR surface;
};

}  // namespace uron
