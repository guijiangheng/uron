#include "uron/vulkan/buffer.h"

#include <stdexcept>

#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/memory.h"

namespace uron {

Buffer::Buffer(const Device& device, VkDeviceSize size,
               VkBufferUsageFlags usage, VkMemoryPropertyFlags propertyFlags)
    : device{device} {
  VkBufferCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateBuffer(device, &createInfo, nullptr, &buffer),
           "create buffer");

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  memory.reset(new Memory(device, memRequirements, propertyFlags));

  vkBindBufferMemory(device, buffer, *memory, 0);
}

Buffer::Buffer(Buffer&& rhs) noexcept
    : device{rhs.device}, memory{std::move(rhs.memory)} {
  buffer = rhs.buffer;
  rhs.buffer = VK_NULL_HANDLE;
}

Buffer::~Buffer() { vkDestroyBuffer(device, buffer, nullptr); }

void* Buffer::map(VkDeviceSize offset, VkDeviceSize size) const {
  return memory->map(offset, size);
}

void Buffer::write(const void* data, VkDeviceSize offset,
                   VkDeviceSize size) const {
  memory->write(data, offset, size);
}

void Buffer::unmap() const { memory->unmap(); }

void Buffer::flush(VkDeviceSize offset, VkDeviceSize size) const {
  memory->flush(offset, size);
}

void Buffer::invalidate(VkDeviceSize offset, VkDeviceSize size) const {
  memory->invalidate(offset, size);
}

void Buffer::copy(const CommandPool& commandPool, const Buffer& src,
                  VkDeviceSize srcOffset, VkDeviceSize dstOffset,
                  VkDeviceSize size) const {
  commandPool.execute([&](const CommandBuffer& commandBuffer) {
    VkBufferCopy copyRegion = {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size,
    };
    vkCmdCopyBuffer(commandBuffer, src, buffer, 1, &copyRegion);
  });
}

}  // namespace uron
