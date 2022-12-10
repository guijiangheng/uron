#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class Buffer {
 public:
  NON_COPYABLE(Buffer);

  Buffer(const Device& device, size_t size, VkBufferUsageFlagBits usage);

  ~Buffer();

  void* map(size_t offset, size_t size) const;

  void unmap() const;

  operator VkBuffer() const { return buffer; }

  operator VkDeviceMemory() const { return memory; }

 private:
  const Device& device;
  VkBuffer buffer;
  VkDeviceMemory memory;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags flags) const;
};

}  // namespace uron
