#include "uron/vulkan/instance.h"

#include <cstring>
#include <stdexcept>

namespace uron {

PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;

std::vector<const char*> Instance::getRequiredInstanceExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  return std::vector<const char*>(glfwExtensions,
                                  glfwExtensions + glfwExtensionCount);
}

void Instance::checkValidationLayerSupport(
    const std::vector<const char*>& validationLayers) {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (auto layer : validationLayers) {
    auto result =
        std::find_if(availableLayers.begin(), availableLayers.end(),
                     [layer](const VkLayerProperties& layerProperties) {
                       return strcmp(layer, layerProperties.layerName) == 0;
                     });

    if (result == availableLayers.end()) {
      throw std::runtime_error(std::string(layer) + " is not supported!");
    }
  }
}

VkDebugUtilsMessengerCreateInfoEXT Instance::makeDebugMessengerCreateInfo() {
  return {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
  };
}

Instance::Instance(const std::vector<const char*>& validationLayers) {
  createInstance(validationLayers);
  setupDebugMessenger(validationLayers);
}

Instance::~Instance() {
  if (debugMessenger) {
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);
}

void Instance::createInstance(
    const std::vector<const char*>& validationLayers) {
  checkValidationLayerSupport(validationLayers);

  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Uron",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_2,
  };

  auto extensions = getRequiredInstanceExtensions();

  if (!validationLayers.empty()) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
      .ppEnabledLayerNames = validationLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  if (!validationLayers.empty()) {
    auto debugMessengerCreateInfo = makeDebugMessengerCreateInfo();
    createInfo.pNext = &debugMessengerCreateInfo;
  }

  VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance),
           "create vulkan instance");

  loadInstanceProcAddrs();
}

void Instance::loadInstanceProcAddrs() {
  GetInstanceProcAddr(vkCreateDebugUtilsMessengerEXT);
  GetInstanceProcAddr(vkDestroyDebugUtilsMessengerEXT);
}

void Instance::setupDebugMessenger(
    const std::vector<const char*>& validationLayers) {
  if (!validationLayers.empty()) {
    auto createInfo = makeDebugMessengerCreateInfo();
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                            &debugMessenger),
             "setup up debug messenger");
  }
}

std::vector<VkPhysicalDevice> Instance::getPhysicalDevices() const {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  return devices;
}

}  // namespace uron
