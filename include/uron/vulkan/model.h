#pragma once

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace uron {

struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec2 uv;

  static VkVertexInputBindingDescription getBindingDescription();

  static std::vector<VkVertexInputAttributeDescription>
  getAttributeDescriptions();
};

}  // namespace uron
