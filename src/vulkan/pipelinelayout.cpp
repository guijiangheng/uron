#include "uron/vulkan/pipelinelayout.h"

#include "uron/vulkan/descriptors.h"
#include "uron/vulkan/device.h"

namespace uron {

PipelineLayout::PipelineLayout(
    const Device& device,
    const std::vector<const DescriptorSetLayout*>& layouts)
    : device{device} {
  std::vector<VkDescriptorSetLayout> mLayouts;
  for (auto& layout : layouts) {
    mLayouts.push_back(*layout);
  }

  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = static_cast<uint32_t>(layouts.size()),
      .pSetLayouts = mLayouts.data(),
      .pushConstantRangeCount = 0,
  };

  VK_CHECK(
      vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout),
      "create pipeline layout");
}

PipelineLayout::PipelineLayout(PipelineLayout&& rhs) : device{device} {
  pipelineLayout = rhs.pipelineLayout;
  rhs.pipelineLayout = VK_NULL_HANDLE;
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

}  // namespace uron
