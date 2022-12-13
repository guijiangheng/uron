#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class Memory {
 public:
  NON_COPYABLE(Memory);

  Memory(const Device& device, VkMemoryRequirements requirements,
         VkMemoryPropertyFlags properties);

  Memory(Memory&& other) noexcept;

  ~Memory();

  operator VkDeviceMemory() const { return memory; }

  void* map(VkDeviceSize offset, VkDeviceSize size) const;

  void unmap() const;

  void fill(const void* data, VkDeviceSize offset, VkDeviceSize size) const;

 private:
  const Device& device;
  VkDeviceMemory memory;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties) const;
};

}  // namespace uron
