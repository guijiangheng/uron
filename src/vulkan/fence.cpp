#include "uron/vulkan/fence.h"

#include "uron/vulkan/device.h"

namespace uron {

Fence::Fence(const Device& device) : device{device} {
  VkFenceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  VK_CHECK(vkCreateFence(device, &createInfo, nullptr, &fence), "create fence");
}

Fence::Fence(Fence&& other) : device{other.device} {
  fence = other.fence;
  other.fence = VK_NULL_HANDLE;
}

Fence::~Fence() { vkDestroyFence(device, fence, nullptr); }

void Fence::reset() const {
  VK_CHECK(vkResetFences(device, 1, &fence), "reset fence");
}

void Fence::wait(uint64_t timeout) const {
  VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, timeout),
           "wait for fence");
}

}  // namespace uron
