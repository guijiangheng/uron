#pragma once

#include <memory>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class Memory;
class CommandPool;

class Buffer {
 public:
  NON_COPYABLE(Buffer);

  Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
         VkMemoryPropertyFlags propertyFlags);

  Buffer(Buffer&& rhs) noexcept;

  ~Buffer();

  void fill(const void* data, VkDeviceSize offset, VkDeviceSize size) const;

  void* map(VkDeviceSize offset, VkDeviceSize size) const;

  void unmap() const;

  void copy(const CommandPool& commandPool, const Buffer& src,
            VkDeviceSize srcOffset, VkDeviceSize dstOffset,
            VkDeviceSize size) const;

  operator VkBuffer() const { return buffer; }

  operator const Memory&() const { return *memory; }

 private:
  const Device& device;
  VkBuffer buffer;
  std::unique_ptr<Memory> memory;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags flags) const;
};

}  // namespace uron
