#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class PipelineLayout;
class RenderPass;

class Pipeline {
 public:
  NON_COPYABLE(Pipeline);

  Pipeline(const Device& device, const PipelineLayout& pipelineLayout,
           const RenderPass& renderPass,
           const std::vector<VkPipelineShaderStageCreateInfo> shaderStages);

  ~Pipeline();

  operator VkPipeline() const { return pipeline; }

 private:
  const Device& device;
  VkPipeline pipeline;
};

}  // namespace uron
