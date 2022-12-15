#pragma once

#include <functional>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class CommandBuffer;

class CommandPool {
 public:
  NON_COPYABLE(CommandPool);

  CommandPool(const Device& device, uint32_t queueFamilyIndex);

  CommandPool(CommandPool&& rhs) noexcept;

  ~CommandPool();

  operator VkCommandPool() const { return commandPool; }

  CommandBuffer allocate() const;

  void execute(const std::function<void(CommandBuffer&)>& func) const;

 private:
  const Device& device;
  VkCommandPool commandPool;

  friend class CommandBuffer;
};

}  // namespace uron
