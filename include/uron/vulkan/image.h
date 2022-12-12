#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class CommandPool;
class Buffer;

class Image {
 public:
  NON_COPYABLE(Image);

  Image(const Device& device);

  ~Image();

  void copy(const CommandPool& commandPool, const Buffer& buffer);

  void TransitionImageLayout(const class CommandPool& commandPool,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  operator VkImage() const { return image; }

 private:
  VkImage image;
};

}  // namespace uron
