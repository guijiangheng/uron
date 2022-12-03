#pragma once

#include <optional>
#include <vector>

#include "uron/vulkan/vulkan.h"

namespace uron {

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

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() { return graphicsFamily.has_value(); }
};

class Device {
 public:
  static bool isDeviceSuitable(VkPhysicalDevice physicalDevice);

  Device(VkPhysicalDevice physicalDevice,
         const std::vector<const char*> validationLayers);

  ~Device() { vkDestroyDevice(device, nullptr); }

  operator VkDevice() const { return device; }

 private:
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;

  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);

  void loadDeviceProcAddrs();
};

}  // namespace uron
