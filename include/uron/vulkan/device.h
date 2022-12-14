#pragma once

#include <optional>
#include <string>
#include <vector>

#include "uron/vulkan/vulkan.h"

namespace uron {

// ------------- begin function pointers ----------

extern PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;

extern PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
extern PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
extern PFN_vkGetAccelerationStructureBuildSizesKHR
    vkGetAccelerationStructureBuildSizesKHR;

extern PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
extern PFN_vkGetRayTracingShaderGroupHandlesKHR
    vkGetRayTracingShaderGroupHandlesKHR;
extern PFN_vkGetAccelerationStructureDeviceAddressKHR
    vkGetAccelerationStructureDeviceAddressKHR;

extern PFN_vkCmdBuildAccelerationStructuresKHR
    vkCmdBuildAccelerationStructuresKHR;
extern PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
extern PFN_vkCmdWriteAccelerationStructuresPropertiesKHR
    vkCmdWriteAccelerationStructuresPropertiesKHR;
extern PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;

// ------------- end function pointers ----------

class Window;
class Surface;
class SwapChain;
class Sampler;
class Image;
class ImageView;
class FrameBuffer;
class ShaderModule;
class DescriptorSetLayout;
class RenderPass;
class PipelineLayout;
class Pipeline;
class Fence;
class Semaphore;
class CommandPool;
class Buffer;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class Device {
 public:
  Device(VkPhysicalDevice physicalDevice, const Surface& surface,
         const std::vector<const char*> validationLayers,
         const std::vector<const char*>& extensions);

  ~Device() { vkDestroyDevice(device, nullptr); }

  static void checkExtensionsSupport(
      VkPhysicalDevice device, const std::vector<const char*>& extensions);

  static bool isDeviceSuitable(VkPhysicalDevice physicalDevice,
                               const Surface& surface,
                               const std::vector<const char*>& extensions);

  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice,
                                              const Surface& surface);

  QueueFamilyIndices findQueueFamilies() const {
    return findQueueFamilies(physicalDevice, surface);
  };

  VkPhysicalDeviceProperties getProperties() const {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    return properties;
  }

  SwapChain createSwapChain(const Window& window) const;

  Image createImage(VkExtent2D extent, VkImageType imageType, VkFormat format,
                    VkImageTiling tiling, VkImageUsageFlags usage,
                    VkMemoryPropertyFlags properties) const;

  ImageView createImageView(
      VkImage image, VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
      VkImageAspectFlagBits aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT) const;

  Sampler createSampler(VkSamplerAddressMode addressMode, float minLod = 0.0f,
                        float maxLod = 0.0f) const;

  ShaderModule createShaderModule(std::string filename) const;

  PipelineLayout createPipelineLayout(
      const std::vector<const DescriptorSetLayout*>& layouts) const;

  RenderPass createRenderPass(VkFormat colorImageFormat) const;

  Pipeline createPipeline(
      const PipelineLayout& pipelineLayout, const RenderPass& renderPass,
      const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) const;

  FrameBuffer createFrameBuffer(
      const RenderPass& renderPass,
      const std::vector<const ImageView*>& attachments,
      const VkExtent2D& extent) const;

  CommandPool createCommandPool(uint32_t queueFamilyIndex) const;

  Buffer createBuffer(size_t size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags propertyFlags) const;

  Fence createFence() const;

  Semaphore createSemaphore() const;

  void waitIdle() const;

  operator VkDevice() const { return device; }

  operator VkPhysicalDevice() const { return physicalDevice; }

  VkQueue getGraphicsQueue() const { return graphicsQueue; }

  VkQueue getPresentQueue() const { return presentQueue; }

 private:
  const Surface& surface;

  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  void loadDeviceProcAddrs();
};

}  // namespace uron
