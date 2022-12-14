#pragma once

#include "uron/common.h"

namespace uron {

class Device;

class ImageView {
 public:
  NON_COPYABLE(ImageView);

  ImageView(const Device& device, VkImage image,
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkImageAspectFlagBits aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

  ImageView(ImageView&& rhs) noexcept;

  ~ImageView();

  operator VkImageView() const { return imageView; }

 private:
  const Device& device;

  VkImageView imageView;
};

}  // namespace uron
