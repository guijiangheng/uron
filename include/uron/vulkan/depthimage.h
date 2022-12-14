#pragma once

#include "uron/common.h"
#include "uron/vulkan/image.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class CommandPool;

class DepthImage {
 public:
  NON_COPYABLE(DepthImage);

  DepthImage(const Device& device, const CommandPool& commandPool,
             VkExtent2D extent);

  DepthImage(DepthImage&&) = default;

  operator const Image&() const { return image; }

  operator const ImageView&() const { return imageView; }

 private:
  Image image;
  ImageView imageView;
};

}  // namespace uron
