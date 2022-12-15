#include "uron/vulkan/framebuffer.h"

#include "uron/vulkan/device.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/renderpass.h"
#include "uron/vulkan/swapchain.h"

namespace uron {

FrameBuffer::FrameBuffer(const Device& device, const RenderPass& renderPass,
                         const std::vector<const ImageView*>& attachments,
                         const VkExtent2D& extent)
    : device{device} {
  std::vector<VkImageView> mAttachments;

  for (auto attachment : attachments) {
    mAttachments.push_back(*attachment);
  }

  VkFramebufferCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = renderPass,
      .attachmentCount = static_cast<uint32_t>(mAttachments.size()),
      .pAttachments = mAttachments.data(),
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
  };

  VK_CHECK(vkCreateFramebuffer(device, &createInfo, nullptr, &frameBuffer),
           "create frame buffer");
}

FrameBuffer::FrameBuffer(FrameBuffer&& rhs) noexcept : device{rhs.device} {
  frameBuffer = rhs.frameBuffer;
  rhs.frameBuffer = VK_NULL_HANDLE;
}

FrameBuffer::~FrameBuffer() {
  vkDestroyFramebuffer(device, frameBuffer, nullptr);
}

}  // namespace uron
