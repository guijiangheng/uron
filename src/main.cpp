#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/commandpool.h"
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

    auto swapChain = device.createSwapChain(window);
    auto& swapChainExtent = swapChain.getExtent();
    auto& imageViews = swapChain.getImageViews();
    auto renderPass = device.createRenderPass(swapChain.getColorImageFormat());

    std::vector<uron::FrameBuffer> frameBuffers;

    for (auto& imageView : imageViews) {
      std::vector<const uron::ImageView*> attachments = {&imageView};
      frameBuffers.push_back(
          device.createFrameBuffer(renderPass, attachments, swapChainExtent));
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

    auto queueFamilyIndices = device.findQueueFamilies();
    auto commandPool =
        device.createCommandPool(queueFamilyIndices.graphicsFamily.value());
    auto commandBuffer = commandPool.allocateCommandBuffer();

    commandBuffer.begin();

    {
      VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
      VkRenderPassBeginInfo renderPassInfo{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass = renderPass,
          .framebuffer = frameBuffers[0],
          .renderArea =
              {
                  .offset = {0, 0},
                  .extent = swapChainExtent,
              },
          .clearValueCount = 1,
          .pClearValues = &clearColor,
      };

      vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipeline);

      VkViewport viewport{
          .x = 0.0f,
          .y = 0.0f,
          .width = (float)swapChainExtent.width,
          .height = (float)swapChainExtent.height,
          .minDepth = 0.0f,
          .maxDepth = 1.0f,
      };
      vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

      VkRect2D scissor{
          .offset = {0, 0},
          .extent = swapChainExtent,
      };
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

      vkCmdDraw(commandBuffer, 3, 1, 0, 0);
      vkCmdEndRenderPass(commandBuffer);
    }

    commandBuffer.end();

    while (!window.shouldClose()) {
      glfwPollEvents();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
