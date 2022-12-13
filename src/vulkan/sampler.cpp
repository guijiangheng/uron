#include "uron/vulkan/sampler.h"

#include "uron/vulkan/device.h"

namespace uron {

Sampler::Sampler(const Device& device, VkSamplerAddressMode addressMode,
                 float minLod, float maxLod)
    : device{device} {
  auto properties = device.getProperties();

  VkSamplerCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = addressMode,
      .addressModeV = addressMode,
      .addressModeW = addressMode,
      .mipLodBias = 0.0f,
      .anisotropyEnable = VK_TRUE,
      .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .minLod = minLod,
      .maxLod = maxLod,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
  };

  VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &sampler),
           "create sampler");
}

Sampler::Sampler(Sampler&& other) : device{device} {
  sampler = other.sampler;
  other.sampler = VK_NULL_HANDLE;
}

Sampler::~Sampler() { vkDestroySampler(device, sampler, nullptr); }

}  // namespace uron
