#include "uron/vulkan/memory.h"

#include <cstring>
#include <stdexcept>

#include "uron/vulkan/device.h"

namespace uron {

Memory::Memory(const Device& device, VkMemoryRequirements requirements,
               VkMemoryPropertyFlags properties)
    : device{device} {
  VkMemoryAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = requirements.size,
      .memoryTypeIndex =
          findMemoryType(requirements.memoryTypeBits, properties),
  };

  VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory),
           "allocate memory");
}

Memory::Memory(Memory&& other) noexcept : device{other.device} {
  memory = other.memory;
  other.memory = VK_NULL_HANDLE;
}

Memory::~Memory() { vkFreeMemory(device, memory, nullptr); }

void* Memory::map(size_t offset, size_t size) const {
  void* address;
  VK_CHECK(vkMapMemory(device, memory, offset, size, 0, &address),
           "map buffer memory");
  return address;
}

void Memory::unmap() const { vkUnmapMemory(device, memory); }

void Memory::fill(const void* data, size_t offset, size_t size) const {
  auto address = map(offset, size);
  memcpy(address, data, size);
  unmap();
}

uint32_t Memory::findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) const {
  // TODO 缓存起来
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

  for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

}  // namespace uron
