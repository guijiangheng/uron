#include "uron/vulkan/model.h"

#include "uron/vulkan/buffer.h"
#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/device.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {

template <>
struct hash<uron::Vertex> {
  size_t operator()(const uron::Vertex& vertex) const {
    size_t seed = 0;
    return ((hash<glm::vec3>()(vertex.position) ^
             (hash<glm::vec3>()(vertex.normal) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.uv) << 1);
  }
};

}  // namespace std

namespace uron {

VkVertexInputBindingDescription Vertex::getBindingDescription() {
  return {
      .binding = 0,
      .stride = sizeof(Vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };
}

std::vector<VkVertexInputAttributeDescription>
Vertex::getAttributeDescriptions() {
  return {
      {
          .location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(Vertex, position),
      },
      {
          .location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(Vertex, normal),
      },
      {
          .location = 2,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(Vertex, uv),
      },
  };
}

Model::Model(const Device& device, const CommandPool& commandPool,
             std::string path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        path.c_str())) {
    throw std::runtime_error(warn + err);
  }

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  for (std::unordered_map<Vertex, uint32_t> uniqueVertices;
       auto& shape : shapes) {
    for (auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.position = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2],
      };

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }

  vertexCount = vertices.size();
  indexCount = indices.size();

  auto bufferSize = vertices.size() * sizeof(Vertex);
  auto vertexStagingBuffer =
      device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  auto address = vertexStagingBuffer.map(0, bufferSize);
  memcpy(address, vertices.data(), bufferSize);
  vertexBuffer.reset(new Buffer(
      device, bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
  vertexBuffer->copy(commandPool, vertexStagingBuffer, 0, 0, bufferSize);

  bufferSize = indices.size() * sizeof(uint32_t);
  auto indexStagingBuffer =
      device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  address = indexStagingBuffer.map(0, bufferSize);
  memcpy(address, indices.data(), bufferSize);
  indexBuffer.reset(new Buffer(
      device, bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
  indexBuffer->copy(commandPool, indexStagingBuffer, 0, 0, bufferSize);
}

void Model::bind(const CommandBuffer& commandBuffer) const {
  VkBuffer buffers[] = {*vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Model::draw(const CommandBuffer& commandBuffer) const {
  vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}

};  // namespace uron
