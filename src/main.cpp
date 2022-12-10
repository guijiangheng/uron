#include <cstring>
#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/buffer.h"
#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/fence.h"
#include "uron/vulkan/framebuffer.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/instance.h"
#include "uron/vulkan/model.h"
#include "uron/vulkan/pipeline.h"
#include "uron/vulkan/pipelinelayout.h"
#include "uron/vulkan/renderpass.h"
#include "uron/vulkan/semaphore.h"
#include "uron/vulkan/shadermodule.h"
#include "uron/vulkan/swapchain.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<uron::Vertex> vertices = {
    {
        .position = {0.0f, -0.5f, 0.0f},
        .color = {1.0f, 0.0f, 0.0f},
    },
    {
        .position = {0.5f, 0.5f, 0.0f},
        .color = {0.0f, 1.0f, 0.0f},
    },
    {
        .position = {-0.5f, 0.5f, 0.0f},
        .color = {0.0f, 0.0f, 1.0f},
    },
};

std::vector<uron::FrameBuffer> createFrameBuffers(
    const uron::Device& device, const uron::SwapChain& swapChain,
    const uron::RenderPass& renderPass) {
  auto& swapChainExtent = swapChain.getExtent();
  auto& imageViews = swapChain.getImageViews();
  std::vector<uron::FrameBuffer> frameBuffers;

  for (auto& imageView : imageViews) {
    std::vector<const uron::ImageView*> attachments = {&imageView};
    frameBuffers.push_back(
        device.createFrameBuffer(renderPass, attachments, swapChainExtent));
  }

  return frameBuffers;
}

int main() {
  try {
    auto resized = false;
    uron::Window window{800, 600, "Hello Vulkan"};
    window.addOnResize([&](int, int) { resized = true; });

    uron::Instance instance(validationLayers);
    auto surface = instance.createSurface(window);
    auto device = instance.pickDevice(surface, validationLayers, extensions);

    auto swapChain = device.createSwapChain(window);
    auto& swapChainExtent = swapChain.getExtent();
    auto renderPass = device.createRenderPass(swapChain.getColorImageFormat());
    auto frameBuffers = createFrameBuffers(device, swapChain, renderPass);

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

    int currentFrame = 0;
    std::vector<uron::Fence> inFlightFences;
    std::vector<uron::Semaphore> imageAvailableSemaphores;
    std::vector<uron::Semaphore> renderFinishedSemaphores;
    std::vector<uron::CommandBuffer> commandBuffers;

    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
      inFlightFences.emplace_back(device);
      imageAvailableSemaphores.emplace_back(device);
      renderFinishedSemaphores.emplace_back(device);
      commandBuffers.emplace_back(commandPool);
    }

    auto bufferSize = sizeof(uron::Vertex) * vertices.size();
    auto buffer =
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto data = buffer.map(0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    buffer.unmap();

    auto recordCommandBuffer = [&](uron::CommandBuffer& commandBuffer,
                                   uint32_t imageIndex) {
      commandBuffers[currentFrame].reset();
      commandBuffers[currentFrame].begin();

      VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
      VkRenderPassBeginInfo renderPassInfo{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass = renderPass,
          .framebuffer = frameBuffers[imageIndex],
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

      VkBuffer buffers[] = {buffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

      vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

      vkCmdEndRenderPass(commandBuffer);

      commandBuffer.end();
    };

    auto drawFrame = [&]() {
      inFlightFences[currentFrame].wait(UINT64_MAX);

      uint32_t imageIndex;
      auto result = vkAcquireNextImageKHR(
          device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
          VK_NULL_HANDLE, &imageIndex);

      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain.recreate();
        frameBuffers = createFrameBuffers(device, swapChain, renderPass);
        return;
      } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
      }

      inFlightFences[currentFrame].reset();

      recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

      VkCommandBuffer rawCommandBuffer = commandBuffers[currentFrame];
      VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
      VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
      VkPipelineStageFlags waitStages[] = {
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

      VkSubmitInfo submitInfo{
          .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = waitSemaphores,
          .pWaitDstStageMask = waitStages,
          .commandBufferCount = 1,
          .pCommandBuffers = &rawCommandBuffer,
          .signalSemaphoreCount = 1,
          .pSignalSemaphores = signalSemaphores,
      };

      if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
                        inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      }

      VkSwapchainKHR swapChains[] = {swapChain};
      VkPresentInfoKHR presentInfo{
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = signalSemaphores,
          .swapchainCount = 1,
          .pSwapchains = swapChains,
          .pImageIndices = &imageIndex,
      };
      result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

      if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
          resized) {
        resized = false;
        swapChain.recreate();
        frameBuffers = createFrameBuffers(device, swapChain, renderPass);
      } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
      }

      currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    };

    while (!window.shouldClose()) {
      glfwPollEvents();
      drawFrame();
    }

    device.waitIdle();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
