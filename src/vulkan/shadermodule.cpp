#include "uron/vulkan/shadermodule.h"

#include "uron/common.h"
#include "uron/vulkan/device.h"

namespace uron {

ShaderModule::ShaderModule(const Device& device, std::string filename)
    : device{device} {
  createShaderModule(loadFile(filename));
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(device, shaderModule, nullptr);
}

void ShaderModule::createShaderModule(const std::vector<char>& buffer) {
  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = buffer.size(),
      .pCode = reinterpret_cast<const uint32_t*>(buffer.data()),
  };

  VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule),
           "create shader module");
}

VkPipelineShaderStageCreateInfo ShaderModule::createShaderStage(
    VkShaderStageFlagBits stage) const {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = stage,
      .module = shaderModule,
      .pName = "main",
  };
}

}  // namespace uron
