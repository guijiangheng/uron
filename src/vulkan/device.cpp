#include "uron/vulkan/device.h"

#include <vector>

#include "uron/vulkan/vulkan.h"

namespace uron {

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

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice physicalDevice) {
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

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

bool Device::isDeviceSuitable(VkPhysicalDevice physicalDevice) {
  auto indices = findQueueFamilies(physicalDevice);

  return indices.isComplete();
}

Device::Device(VkPhysicalDevice physicalDevice,
               const std::vector<const char*> validationLayers)
    : physicalDevice(physicalDevice) {
  auto indices = findQueueFamilies(physicalDevice);

  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = indices.graphicsFamily.value(),
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueCreateInfo,
      .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
      .ppEnabledLayerNames = validationLayers.data(),
      .enabledExtensionCount = 0,
      .pEnabledFeatures = &deviceFeatures,
  };

  VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
           "failed to create logical device!");

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

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

}  // namespace uron
