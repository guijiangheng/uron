#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class CommandPool;

class CommandBuffer {
 public:
  NON_COPYABLE(CommandBuffer);

  CommandBuffer(const CommandPool& commandPool);

  operator VkCommandBuffer() const { return commandBuffer; }

  void begin() const;

  void end() const;

 private:
  const CommandPool& commandPool;
  VkCommandBuffer commandBuffer;
};

}  // namespace uron
