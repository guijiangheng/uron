#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/framebuffer.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/instance.h"
#include "uron/vulkan/pipeline.h"
#include "uron/vulkan/pipelinelayout.h"
#include "uron/vulkan/renderpass.h"
#include "uron/vulkan/shadermodule.h"
#include "uron/vulkan/swapchain.h"

int main() {
  const std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  try {
    uron::Window window{800, 600, "Hello Vulkan"};

    uron::Instance instance(validationLayers);
    auto surface = instance.createSurface(window);
    auto device = instance.pickDevice(surface, validationLayers, extensions);

    auto swapChain = device.createSwapChain(window, surface);
    auto& imageViews = swapChain.getImageViews();
    auto renderPass = device.createRenderPass(swapChain.getColorImageFormat());

    std::vector<uron::FrameBuffer> frameBuffers;

    for (auto& extent = swapChain.getExtent(); auto& imageView : imageViews) {
      std::vector<const uron::ImageView*> attachments = {&imageView};
      frameBuffers.push_back(
          device.createFrameBuffer(renderPass, attachments, extent));
    }

    auto vertexShader = device.createShaderModule("../shaders/simple.vert.spv");
    auto fragmentShader =
        device.createShaderModule("../shaders/simple.frag.spv");
    auto shaderStages = {
        vertexShader.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
        fragmentShader.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT),
    };

    auto pipelineLayout = device.createPipelineLayout();
    auto pipeline =
        device.createPipeline(pipelineLayout, renderPass, shaderStages);

    while (!window.shouldClose()) {
      glfwPollEvents();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
