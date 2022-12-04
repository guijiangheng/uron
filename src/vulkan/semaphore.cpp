#include "uron/vulkan/semaphore.h"

#include "uron/vulkan/device.h"

namespace uron {

Semaphore::Semaphore(const Device& device) : device{device} {
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore),
           "create semaphore");
}

Semaphore::~Semaphore() { vkDestroySemaphore(device, semaphore, nullptr); }

}  // namespace uron
