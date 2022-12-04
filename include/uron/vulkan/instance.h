#pragma once

#include <iostream>
#include <vector>

#include "uron/common.h"
#include "uron/gui/window.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/surface.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

class Window;

class Instance {
 public:
  NON_COPYABLE(Instance);

  Instance(const std::vector<const char*>& validationLayers = {});

  ~Instance();

  static std::vector<const char*> getRequiredInstanceExtensions(
      const std::vector<const char*>& validationLayers);

  static void checkValidationLayerSupport(
      const std::vector<const char*>& validationLayers);

  Surface createSurface(const Window& window) const {
    return Surface(*this, window);
  }

  Device pickDevice(const Surface& surface,
                    const std::vector<const char*>& validationLayers,
                    const std::vector<const char*>& extensions) const;

  operator VkInstance() const { return instance; }

 private:
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

  static VkDebugUtilsMessengerCreateInfoEXT makeDebugMessengerCreateInfo();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  void createInstance(const std::vector<const char*>& validationLayers);
  void loadInstanceProcAddrs();
  void setupDebugMessenger(const std::vector<const char*>& validationLayers);
};

}  // namespace uron
