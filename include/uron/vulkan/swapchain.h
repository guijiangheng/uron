#pragma once

#include <vector>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Window;
class Device;
class Surface;
class ImageView;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
 public:
  SwapChain(const Device& device, const Window& window, const Surface& surface);

  ~SwapChain();

  VkFormat getColorImageFormat() const { return swapChainImageFormat; }

  const VkExtent2D& getExtent() const { return swapChainExtent; }

  const std::vector<ImageView>& getImageViews() const { return imageViews; }

 private:
  const Device& device;
  const Surface& surface;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<ImageView> imageViews;

  SwapChainSupportDetails querySwapChainSupport();

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);

  static VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);

  static VkExtent2D chooseSwapExtent(
      const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
};

}  // namespace uron
