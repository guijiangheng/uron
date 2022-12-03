#pragma once

#include <vector>

#include "uron/common/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Window;
class Device;
class Surface;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
 public:
  SwapChain(const Device& device, const Window& window, const Surface& surface);
  ~SwapChain();

 private:
  const Device& device;
  const Window& window;
  const Surface& surface;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  SwapChainSupportDetails querySwapChainSupport();

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);

  static VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);

  static VkExtent2D chooseSwapExtent(
      const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
};

}  // namespace uron
