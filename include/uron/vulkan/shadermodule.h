#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class ShaderModule {
 public:
  NON_COPYABLE(ShaderModule);

  ShaderModule(const Device& device, const std::string& filename);

  ~ShaderModule();

  VkPipelineShaderStageCreateInfo createShaderStage(
      VkShaderStageFlagBits stage) const;

  operator VkShaderModule() const { return shaderModule; }

 private:
  const Device& device;
  VkShaderModule shaderModule;

  void createShaderModule(const std::vector<char>& buffer);
};

}  // namespace uron
