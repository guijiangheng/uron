#include <chrono>
#include <cstring>
#include <iostream>

#include "uron/gui/window.h"
#include "uron/vulkan/buffer.h"
#include "uron/vulkan/commandbuffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/depthimage.h"
#include "uron/vulkan/descriptors.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/fence.h"
#include "uron/vulkan/framebuffer.h"
#include "uron/vulkan/image.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/instance.h"
#include "uron/vulkan/model.h"
#include "uron/vulkan/pipeline.h"
#include "uron/vulkan/pipelinelayout.h"
#include "uron/vulkan/renderpass.h"
#include "uron/vulkan/sampler.h"
#include "uron/vulkan/semaphore.h"
#include "uron/vulkan/shadermodule.h"
#include "uron/vulkan/swapchain.h"
#include "uron/vulkan/texture.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

std::vector<uron::FrameBuffer> createFrameBuffers(
    const uron::Device& device, const uron::SwapChain& swapChain,
    const uron::RenderPass& renderPass,
    const std::vector<uron::DepthImage>& depthImages) {
  auto& swapChainExtent = swapChain.getExtent();
  auto& imageViews = swapChain.getImageViews();
  std::vector<uron::FrameBuffer> frameBuffers;

  for (auto i = 0; i < imageViews.size(); ++i) {
    const uron::ImageView& depthImageView = depthImages[i];
    std::vector<const uron::ImageView*> attachments = {&imageViews[i],
                                                       &depthImageView};
    frameBuffers.push_back(
        device.createFrameBuffer(renderPass, attachments, swapChainExtent));
  }

  return frameBuffers;
}

int main() {
  try {
    uron::TextureResource textureResource("../assets/viking_room.png");

    auto resized = false;
    uron::Window window{800, 600, "Hello Vulkan"};
    window.addOnResize([&](int, int) { resized = true; });

    uron::Instance instance(validationLayers);
    auto surface = instance.createSurface(window);
    auto device = instance.pickDevice(surface, validationLayers, extensions);

    auto swapChain = device.createSwapChain(window);
    auto& swapChainExtent = swapChain.getExtent();
    auto renderPass = device.createRenderPass(swapChain.getColorImageFormat());

    auto vertexShader = device.createShaderModule("../shaders/simple.vert.spv");
    auto fragmentShader =
        device.createShaderModule("../shaders/simple.frag.spv");
    auto shaderStages = {
        vertexShader.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
        fragmentShader.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT),
    };

    uron::DescriptorSetBindings bindings(device);
    bindings
        .addBinding({
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        })
        .addBinding({
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        });
    auto descriptorSetLayout = bindings.createLayout();
    std::vector<const uron::DescriptorSetLayout*> descriptorSetLayouts = {
        &descriptorSetLayout};

    auto descriptorPool = bindings.createPool(MAX_FRAMES_IN_FLIGHT);
    std::vector<uron::DescriptorSet> descriptorSets;
    descriptorSets.push_back(descriptorPool.allocate(descriptorSetLayout));
    descriptorSets.push_back(descriptorPool.allocate(descriptorSetLayout));

    auto pipelineLayout = device.createPipelineLayout(descriptorSetLayouts);
    auto pipeline =
        device.createPipeline(pipelineLayout, renderPass, shaderStages);

    auto queueFamilyIndices = device.findQueueFamilies();
    auto commandPool =
        device.createCommandPool(queueFamilyIndices.graphicsFamily.value());
    uron::Model model{device, commandPool, "../assets/viking_room.obj"};

    auto swapChainImageCount = swapChain.getImageViews().size();
    std::vector<uron::DepthImage> depthImages;
    for (auto i = 0; i < swapChainImageCount; ++i) {
      depthImages.emplace_back(device, commandPool, swapChainExtent);
    }
    auto frameBuffers =
        createFrameBuffers(device, swapChain, renderPass, depthImages);

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

    std::vector<uron::Buffer> uniformBuffers;
    std::array<void*, MAX_FRAMES_IN_FLIGHT> uniformBufferAddrs;

    auto uniformBufferSize = sizeof(UniformBufferObject);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
      uniformBuffers.emplace_back(device, uniformBufferSize,
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      uniformBufferAddrs[i] = (uniformBuffers[i].map());
    }

    auto texture = uron::Texture(device, commandPool, textureResource);

    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
      descriptorSets[i]
          .write(0,
                 {
                     .buffer = uniformBuffers[i],
                     .offset = 0,
                     .range = uniformBufferSize,
                 })
          .write(1, {
                        .sampler = texture,
                        .imageView = texture,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    });
    }

    auto updateUniformBuffer = [&](uint32_t currentImage) {
      static auto startTime = std::chrono::high_resolution_clock::now();

      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration<float, std::chrono::seconds::period>(
                       currentTime - startTime)
                       .count();

      UniformBufferObject ubo{};
      ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                              glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.view =
          glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.proj = glm::perspective(
          glm::radians(45.0f),
          swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
      ubo.proj[1][1] *= -1;

      memcpy(uniformBufferAddrs[currentImage], &ubo, sizeof(ubo));
    };

    auto recordCommandBuffer = [&](uron::CommandBuffer& commandBuffer,
                                   uint32_t imageIndex) {
      commandBuffers[currentFrame].reset();
      commandBuffers[currentFrame].begin();

      VkClearValue clearValues[] = {
          {.color = {{0.0f, 0.0f, 0.0f, 1.0f}}},
          {.depthStencil = {1.0f, 0}},
      };
      VkRenderPassBeginInfo renderPassInfo{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass = renderPass,
          .framebuffer = frameBuffers[imageIndex],
          .renderArea =
              {
                  .offset = {0, 0},
                  .extent = swapChainExtent,
              },
          .clearValueCount = 2,
          .pClearValues = clearValues,
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

      VkDescriptorSet mDescriptorSet = descriptorSets[currentFrame];
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelineLayout, 0, 1, &mDescriptorSet, 0,
                              nullptr);
      model.bind(commandBuffer);
      model.draw(commandBuffer);

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
        depthImages.clear();
        for (auto i = 0; i < swapChainImageCount; ++i) {
          depthImages.emplace_back(device, commandPool, swapChainExtent);
        }
        frameBuffers =
            createFrameBuffers(device, swapChain, renderPass, depthImages);
        return;
      } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
      }

      updateUniformBuffer(currentFrame);

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
        depthImages.clear();
        for (auto i = 0; i < swapChainImageCount; ++i) {
          depthImages.emplace_back(device, commandPool, swapChainExtent);
        }
        frameBuffers =
            createFrameBuffers(device, swapChain, renderPass, depthImages);
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
