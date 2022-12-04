#include "uron/vulkan/commandpool.h"

#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/device.h"

namespace uron {

CommandPool::CommandPool(const Device& device, uint32_t queueFamilyIndex)
    : device{device} {
  VkCommandPoolCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndex,
  };

  VK_CHECK(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool),
           "create command pool failed");
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(device, commandPool, nullptr);
}

CommandBuffer CommandPool::allocateCommandBuffer() const {
  return CommandBuffer(*this);
}

}  // namespace uron
