#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class CommandPool;

class CommandBuffer {
 public:
  NON_COPYABLE(CommandBuffer);

  CommandBuffer(const CommandPool& commandPool);

  CommandBuffer(CommandBuffer&& other) noexcept;

  operator VkCommandBuffer() const { return commandBuffer; }

  void begin(VkCommandBufferUsageFlags flags =
                 VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT) const;

  void end() const;

  void reset() const;

 private:
  const CommandPool& commandPool;
  VkCommandBuffer commandBuffer;
};

}  // namespace uron
