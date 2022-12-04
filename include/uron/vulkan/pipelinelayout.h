#pragma once

#include "uron/common.h"

namespace uron {

class Device;

class PipelineLayout {
 public:
  NON_COPYABLE(PipelineLayout);

  PipelineLayout(const Device& device);

  ~PipelineLayout();

 private:
  const Device& device;
  VkPipelineLayout pipelineLayout;
};

}  // namespace uron
