#include "uron/vulkan/device.h"

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
