#include "uron/vulkan/pipelinelayout.h"

#include "uron/vulkan/device.h"

namespace uron {

PipelineLayout::PipelineLayout(const Device& device) : device{device} {
  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pushConstantRangeCount = 0,
  };

  VK_CHECK(
      vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout),
      "create pipeline layout");
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

}  // namespace uron
