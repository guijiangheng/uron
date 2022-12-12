#include "uron/vulkan/commandpool.h"

#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/fence.h"

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

CommandPool::CommandPool(CommandPool&& other) noexcept : device{other.device} {
  commandPool = other.commandPool;
  other.commandPool = VK_NULL_HANDLE;
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(device, commandPool, nullptr);
}

CommandBuffer CommandPool::allocate() const { return CommandBuffer(*this); }

void CommandPool::execute(
    const std::function<void(CommandBuffer&)>& func) const {
  auto commandBuffer = allocate();
  commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  func(commandBuffer);
  commandBuffer.end();
  VkCommandBuffer mCommandBuffer = commandBuffer;

  VkSubmitInfo submitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &mCommandBuffer,
  };

  auto fence = device.createFence();
  fence.reset();
  auto graphicsQueue = device.getGraphicsQueue();
  vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
  fence.wait(UINT64_MAX);
}

}  // namespace uron
