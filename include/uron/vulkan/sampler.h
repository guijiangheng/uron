#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class Sampler {
 public:
  NON_COPYABLE(Sampler);

  Sampler(const Device& device,
          VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          float minLod = 0.0f, float maxLod = 0.0f);

  Sampler(Sampler&& other);

  ~Sampler();

  operator VkSampler() const { return sampler; }

 private:
  const Device& device;
  VkSampler sampler;
};

}  // namespace uron
