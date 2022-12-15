#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class RenderPass;
class ImageView;

class FrameBuffer {
 public:
  NON_COPYABLE(FrameBuffer);

  FrameBuffer(const Device& device, const RenderPass& renderPass,
              const std::vector<const ImageView*>& attachments,
              const VkExtent2D& extent);

  FrameBuffer(FrameBuffer&& rhs) noexcept;

  ~FrameBuffer();

  operator VkFramebuffer() const { return frameBuffer; }

 private:
  const Device& device;
  VkFramebuffer frameBuffer;
};

}  // namespace uron
