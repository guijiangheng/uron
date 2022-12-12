#include "uron/vulkan/descriptors.h"

#include <cassert>

#include "uron/vulkan/device.h"

namespace uron {

DescriptorSetBindings::DescriptorSetBindings(const Device& device)
    : device{device} {}

void DescriptorSetBindings::addBinding(
    const VkDescriptorSetLayoutBinding& binding) {
  assert(bindings.count(binding.binding) == 0 && "Binding already exists");
  bindings[binding.binding] = binding;
}

DescriptorSetLayout DescriptorSetBindings::createLayout() const {
  std::vector<VkDescriptorSetLayoutBinding> mBindings;

  for (auto& [binding, info] : bindings) {
    mBindings.push_back(info);
  }

  return {device, mBindings};
}

DescriptorPool DescriptorSetBindings::createPool(uint32_t maxSets) const {
  std::vector<VkDescriptorPoolSize> poolSizes;

  for (auto& [binding, info] : bindings) {
    poolSizes.push_back({
        .type = info.descriptorType,
        .descriptorCount = info.descriptorCount * maxSets,
    });
  }

  return {device, poolSizes, maxSets};
}

DescriptorSetLayout::DescriptorSetLayout(
    const Device& device,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    : device{device} {
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data(),
  };

  VK_CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo,
                                       nullptr, &layout),
           "create descriptor set layout");
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : device{other.device} {
  layout = other.layout;
  other.layout = VK_NULL_HANDLE;
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(device, layout, nullptr);
}

DescriptorPool::DescriptorPool(
    const Device& device, const std::vector<VkDescriptorPoolSize>& poolSizes,
    uint32_t maxSets)
    : device{device} {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = maxSets,
      .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
      .pPoolSizes = poolSizes.data(),
  };

  VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &pool),
           "create descriptor pool");
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : device{other.device} {
  pool = other.pool;
  other.pool = VK_NULL_HANDLE;
}

DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(device, pool, nullptr);
}

DescriptorSet DescriptorPool::allocate(
    const DescriptorSetLayout& layout) const {
  return {device, *this, layout};
}

DescriptorSet::DescriptorSet(const Device& device, const DescriptorPool& pool,
                             const DescriptorSetLayout& layout)
    : device{device}, pool{pool} {
  VkDescriptorSetLayout mLayout = layout;
  VkDescriptorSetAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &mLayout,
  };

  VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet),
           "create descriptor set");
}

void DescriptorSet::write(const VkDescriptorBufferInfo& bufferInfo) const {
  VkWriteDescriptorSet descriptorWrite{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptorSet,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = &bufferInfo,
  };

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

}  // namespace uron
