#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;

class Semaphore {
 public:
  NON_COPYABLE(Semaphore);

  Semaphore(const Device& device);

  Semaphore(Semaphore&& rhs) noexcept;

  ~Semaphore();

  operator VkSemaphore() const { return semaphore; }

 private:
  const Device& device;
  VkSemaphore semaphore;
};

}  // namespace uron
