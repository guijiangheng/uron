#include "uron/vulkan/depthimage.h"

#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/image.h"
#include "uron/vulkan/imageview.h"

namespace uron {

DepthImage::DepthImage(const Device& device, const CommandPool& commandPool,
                       VkExtent2D extent)
    : image{device,
            extent,
            VK_IMAGE_TYPE_2D,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT},
      imageView{device, image, VK_FORMAT_D24_UNORM_S8_UINT,
                VK_IMAGE_ASPECT_DEPTH_BIT} {
  image.transitionImageLayout(commandPool, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

}  // namespace uron
