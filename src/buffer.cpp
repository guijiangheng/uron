#include "uron/vulkan/buffer.h"

#include <stdexcept>

#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/memory.h"

namespace uron {

Buffer::Buffer(const Device& device, size_t size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags propertyFlags)
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

Buffer::Buffer(Buffer&& other) noexcept
    : device{other.device}, memory{std::move(other.memory)} {
  buffer = other.buffer;
  other.buffer = VK_NULL_HANDLE;
}

Buffer::~Buffer() { vkDestroyBuffer(device, buffer, nullptr); }

void Buffer::copy(const CommandPool& commandPool, const Buffer& src,
                  VkDeviceSize size, VkDeviceSize srcOffset,
                  VkDeviceSize dstOffset) const {
  commandPool.execute([&](const CommandBuffer& commandBuffer) {
    VkBufferCopy copyRegion = {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size,
    };
    vkCmdCopyBuffer(commandBuffer, src, buffer, 1, &copyRegion);
  });
}

void* Buffer::map(VkDeviceSize offset, VkDeviceSize size) const {
  return memory->map(offset, size);
}

void Buffer::unmap() const { memory->unmap(); }

}  // namespace uron
