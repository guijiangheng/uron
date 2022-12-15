#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class Fence {
 public:
  NON_COPYABLE(Fence);

  Fence(const Device& device);

  Fence(Fence&& rhs) noexcept;

  ~Fence();

  operator VkFence() const { return fence; }

  void wait(uint64_t timeout) const;

  void reset() const;

 private:
  const Device& device;
  VkFence fence;
};

}  // namespace uron
