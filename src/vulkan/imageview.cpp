#include "uron/vulkan/imageview.h"

#include "uron/vulkan/device.h"

namespace uron {

ImageView::ImageView(const Device& device, VkImage image, VkFormat format,
                     VkImageAspectFlagBits aspectFlags)
    : device{device} {
  VkImageViewCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .components =
          {
              .r = VK_COMPONENT_SWIZZLE_IDENTITY,
              .g = VK_COMPONENT_SWIZZLE_IDENTITY,
              .b = VK_COMPONENT_SWIZZLE_IDENTITY,
              .a = VK_COMPONENT_SWIZZLE_IDENTITY,
          },
      .subresourceRange =
          {
              .aspectMask = aspectFlags,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &imageView),
           "failed to create image view");
}

ImageView::ImageView(ImageView&& rhs) noexcept : device(rhs.device) {
  imageView = rhs.imageView;
  rhs.imageView = VK_NULL_HANDLE;
}

ImageView::~ImageView() { vkDestroyImageView(device, imageView, nullptr); }

}  // namespace uron
