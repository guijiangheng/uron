#pragma once

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

class Device {
 public:
  operator VkDevice() const { return device; }

 private:
  VkDevice device;

  void loadDeviceProcAddrs();
};

}  // namespace uron
