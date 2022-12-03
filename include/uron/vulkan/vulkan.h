#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GetInstanceProcAddr(name) \
  PFN_##name name =               \
      reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance, #name))

#define GetDeviceProcAddr(name) \
  PFN_##name name =             \
      reinterpret_cast<PFN_##name>(vkGetDeviceProcAddr(device, #name))

namespace uron {

void VK_CHECK(VkResult result, const char* operation);

}  // namespace uron
