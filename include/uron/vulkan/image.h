#pragma once

#include <memory>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class CommandPool;
class Buffer;
class Memory;

class Image {
 public:
  NON_COPYABLE(Image);

  Image(const Device& device, VkExtent2D extent, VkImageType imageType,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties);

  Image(Image&& other) noexcept;

  ~Image();

  void copy(const CommandPool& commandPool, const Buffer& buffer);

  void transitionImageLayout(const class CommandPool& commandPool,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  operator VkImage() const { return image; }

  operator const Memory&() const { return *memory; }

 private:
  const Device& device;
  VkExtent2D extent;
  VkFormat format;
  VkImage image;
  std::unique_ptr<Memory> memory;
};

}  // namespace uron
