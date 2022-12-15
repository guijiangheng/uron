#include "uron/vulkan/commandbuffer.h"

#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"

namespace uron {

CommandBuffer::CommandBuffer(const CommandPool& commandPool)
    : commandPool{commandPool} {
  VkCommandBufferAllocateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VK_CHECK(
      vkAllocateCommandBuffers(commandPool.device, &createInfo, &commandBuffer),
      "create command buffer");
}

CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
    : commandPool{rhs.commandPool} {
  commandBuffer = rhs.commandBuffer;
  rhs.commandBuffer = VK_NULL_HANDLE;
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = flags,
  };

  VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo),
           "begin recording command buffer");
}

void CommandBuffer::end() const {
  VK_CHECK(vkEndCommandBuffer(commandBuffer), "record command buffer");
}

void CommandBuffer::reset() const {
  VK_CHECK(vkResetCommandBuffer(commandBuffer, 0), "reset command buffer");
}

}  // namespace uron
