#include "uron/vulkan/buffer.h"

#include <stdexcept>

#include "uron/vulkan/device.h"

namespace uron {

Buffer::Buffer(const Device& device, size_t size, VkBufferUsageFlagBits usage)
    : device{device} {
  VkBufferCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateBuffer(device, &createInfo, nullptr, &buffer),
           "create buffer");

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memRequirements.size,
      .memoryTypeIndex =
          findMemoryType(memRequirements.memoryTypeBits,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
  };

  VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory),
           "allocate buffer memory");

  vkBindBufferMemory(device, buffer, memory, 0);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags flags) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

Buffer::~Buffer() {
  vkDestroyBuffer(device, buffer, nullptr);
  vkFreeMemory(device, memory, nullptr);
}

void* Buffer::map(size_t offset, size_t size) const {
  void* address;
  VK_CHECK(vkMapMemory(device, memory, offset, size, 0, &address),
           "map buffer memory");

  return address;
}

void Buffer::unmap() const { vkUnmapMemory(device, memory); }

}  // namespace uron
