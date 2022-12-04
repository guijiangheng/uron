#pragma once

#include "uron/common/common.h"

namespace uron {

class Device;

class ImageView {
 public:
  NON_COPYABLE(ImageView);

  ImageView(const Device& device, VkImage image,
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkImageAspectFlagBits aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

  ImageView(ImageView&& rhs) : device(rhs.device) {
    imageView = rhs.imageView;
    rhs.imageView = VK_NULL_HANDLE;
  }

  ~ImageView();

  operator VkImageView() const { return imageView; }

 private:
  const Device& device;

  VkImageView imageView;
};

}  // namespace uron
