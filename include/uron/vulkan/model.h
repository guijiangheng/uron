#pragma once

#include <memory>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace uron {

class Device;
class Buffer;
class CommandPool;
class CommandBuffer;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;

  static VkVertexInputBindingDescription getBindingDescription();

  static std::vector<VkVertexInputAttributeDescription>
  getAttributeDescriptions();

  bool operator<=>(const Vertex& rhs) const = default;
};

class Model {
 public:
  NON_COPYABLE(Model);

  Model(const Device& device, const CommandPool& commandPool, std::string path);

  Model(Model&& rhs) = default;

  void bind(const CommandBuffer& commandBuffer) const;

  void draw(const CommandBuffer& commandBuffer) const;

 private:
  uint32_t vertexCount;
  uint32_t indexCount;
  std::unique_ptr<Buffer> vertexBuffer;
  std::unique_ptr<Buffer> indexBuffer;
};

}  // namespace uron
