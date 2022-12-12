
#pragma once

#include <unordered_map>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class DescriptorSetBindings;
class DescriptorSetLayout;
class DescriptorPool;
class DescriptorSet;

class DescriptorSetBindings {
 public:
  NON_COPYABLE(DescriptorSetBindings);

  DescriptorSetBindings(const Device& device);

  void addBinding(const VkDescriptorSetLayoutBinding& binding);

  DescriptorSetLayout createLayout() const;

  DescriptorPool createPool(uint32_t maxSets) const;

 private:
  const Device& device;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
};

class DescriptorSetLayout {
 public:
  NON_COPYABLE(DescriptorSetLayout);

  DescriptorSetLayout(
      const Device& device,
      const std::vector<VkDescriptorSetLayoutBinding>& bindings);

  DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;

  ~DescriptorSetLayout();

  operator VkDescriptorSetLayout() const { return layout; }

 private:
  const Device& device;
  VkDescriptorSetLayout layout;
};

class DescriptorPool {
 public:
  NON_COPYABLE(DescriptorPool);

  DescriptorPool(const Device& device,
                 const std::vector<VkDescriptorPoolSize>& poolSizes,
                 uint32_t maxSets);

  DescriptorPool(DescriptorPool&& other) noexcept;

  ~DescriptorPool();

  DescriptorSet allocate(const DescriptorSetLayout& layout) const;

  operator VkDescriptorPool() const { return pool; }

 private:
  const Device& device;
  VkDescriptorPool pool;
};

class DescriptorSet {
 public:
  DescriptorSet(const Device& device, const DescriptorPool& pool,
                const DescriptorSetLayout& layout);

  void write(const VkDescriptorBufferInfo& bufferInfo) const;

  operator VkDescriptorSet() const { return descriptorSet; }

 private:
  const Device& device;
  const DescriptorPool& pool;
  VkDescriptorSet descriptorSet;
};

}  // namespace uron
