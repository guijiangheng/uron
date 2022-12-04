#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class RenderPass {
 public:
  NON_COPYABLE(RenderPass);

  RenderPass(const Device& device, VkFormat colorImageFormat);

  ~RenderPass();

 private:
  const Device& device;
  VkRenderPass renderPass;
};

}  // namespace uron