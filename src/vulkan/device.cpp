#include "uron/vulkan/device.h"

#include <cstring>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

#include "uron/gui/window.h"
#include "uron/vulkan/buffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/fence.h"
#include "uron/vulkan/framebuffer.h"
#include "uron/vulkan/image.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/pipeline.h"
#include "uron/vulkan/pipelinelayout.h"
#include "uron/vulkan/renderpass.h"
#include "uron/vulkan/sampler.h"
#include "uron/vulkan/semaphore.h"
#include "uron/vulkan/shadermodule.h"
#include "uron/vulkan/surface.h"
#include "uron/vulkan/swapchain.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

// -------- begin function pointers ----------------

PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;

PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = nullptr;
PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR =
    nullptr;
PFN_vkGetAccelerationStructureBuildSizesKHR
    vkGetAccelerationStructureBuildSizesKHR = nullptr;

PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;
PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR =
    nullptr;
PFN_vkGetAccelerationStructureDeviceAddressKHR
    vkGetAccelerationStructureDeviceAddressKHR = nullptr;

PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR =
    nullptr;
PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR =
    nullptr;
PFN_vkCmdWriteAccelerationStructuresPropertiesKHR
    vkCmdWriteAccelerationStructuresPropertiesKHR = nullptr;
PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = nullptr;

// --------- end function pointers ----

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice physicalDevice,
                                             const Surface& surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  for (int i = 0; auto queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                         &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

void Device::checkExtensionsSupport(
    VkPhysicalDevice device, const std::vector<const char*>& extensions) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  for (auto extension : extensions) {
    auto result = std::find_if(
        availableExtensions.begin(), availableExtensions.end(),
        [extension](auto& extensionProperties) {
          return strcmp(extension, extensionProperties.extensionName) == 0;
        });

    if (result == availableExtensions.end()) {
      throw std::runtime_error(std::string(extension) + " is not supported!");
    }
  }
}

bool Device::isDeviceSuitable(VkPhysicalDevice physicalDevice,
                              const Surface& surface,
                              const std::vector<const char*>& extensions) {
  checkExtensionsSupport(physicalDevice, extensions);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(physicalDevice, &features);

  return findQueueFamilies(physicalDevice, surface).isComplete() &&
         features.samplerAnisotropy;
}

Device::Device(VkPhysicalDevice physicalDevice, const Surface& surface,
               const std::vector<const char*> validationLayers,
               const std::vector<const char*>& extensions)
    : surface{surface}, physicalDevice(physicalDevice) {
  auto indices = findQueueFamilies(physicalDevice, surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  for (float queuePriority = 1.0f; auto queueFamily : uniqueQueueFamilies) {
    queueCreateInfos.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    });
  }

  VkPhysicalDeviceFeatures deviceFeatures{
      .samplerAnisotropy = VK_TRUE,
  };

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
      .ppEnabledLayerNames = validationLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
           "failed to create logical device!");

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

  loadDeviceProcAddrs();
}

void Device::loadDeviceProcAddrs() {
  GetDeviceProcAddr(vkSetDebugUtilsObjectNameEXT);

  GetDeviceProcAddr(vkCreateAccelerationStructureKHR);
  GetDeviceProcAddr(vkDestroyAccelerationStructureKHR);
  GetDeviceProcAddr(vkGetAccelerationStructureBuildSizesKHR);

  GetDeviceProcAddr(vkCreateRayTracingPipelinesKHR);
  GetDeviceProcAddr(vkGetRayTracingShaderGroupHandlesKHR);
  GetDeviceProcAddr(vkGetAccelerationStructureDeviceAddressKHR);

  GetDeviceProcAddr(vkCmdBuildAccelerationStructuresKHR);
  GetDeviceProcAddr(vkCmdCopyAccelerationStructureKHR);
  GetDeviceProcAddr(vkCmdWriteAccelerationStructuresPropertiesKHR);
  GetDeviceProcAddr(vkCmdTraceRaysKHR);
}

SwapChain Device::createSwapChain(const Window& window) const {
  return {*this, window, surface};
}

Image Device::createImage(VkExtent2D extent, VkImageType imageType,
                          VkFormat format, VkImageTiling tiling,
                          VkImageUsageFlags usage,
                          VkMemoryPropertyFlags properties) const {
  return {*this, extent, imageType, format, tiling, usage, properties};
}

ImageView Device::createImageView(VkImage image, VkFormat format,
                                  VkImageAspectFlagBits aspectFlags) const {
  return {*this, image, format, aspectFlags};
}

Sampler Device::createSampler(VkSamplerAddressMode addressMode, float minLod,
                              float maxLod) const {
  return {*this, addressMode, minLod, maxLod};
}

ShaderModule Device::createShaderModule(std::string filename) const {
  return {*this, filename};
}

PipelineLayout Device::createPipelineLayout(
    const std::vector<const DescriptorSetLayout*>& layouts) const {
  return {*this, layouts};
}

RenderPass Device::createRenderPass(VkFormat colorImageFormat) const {
  return {*this, colorImageFormat};
}

Pipeline Device::createPipeline(
    const PipelineLayout& pipelineLayout, const RenderPass& renderPass,
    const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) const {
  return {*this, pipelineLayout, renderPass, shaderStages};
}

FrameBuffer Device::createFrameBuffer(
    const RenderPass& renderPass,
    const std::vector<const ImageView*>& attachments,
    const VkExtent2D& extent) const {
  return {*this, renderPass, attachments, extent};
}

CommandPool Device::createCommandPool(uint32_t queueFamilyIndex) const {
  return {*this, queueFamilyIndex};
}

Buffer Device::createBuffer(size_t size, VkBufferUsageFlags usage,
                            VkMemoryPropertyFlags propertyFlags) const {
  return {*this, size, usage, propertyFlags};
}

Fence Device::createFence() const { return {*this}; }

Semaphore Device::createSemaphore() const { return {*this}; }

void Device::waitIdle() const {
  VK_CHECK(vkDeviceWaitIdle(device), "wait for a device to become idle");
}

}  // namespace uron
