#include "uron/vulkan/renderpass.h"

#include "uron/vulkan/device.h"

namespace uron {

RenderPass::RenderPass(const Device& device, VkFormat colorImageFormat)
    : device{device} {
  VkAttachmentDescription colorAttachment{
      .format = colorImageFormat,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentDescription depthAttachment{
      .format = VK_FORMAT_D24_UNORM_S8_UINT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference colorAttachmentRef{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference depthAttachmentRef{
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  VkSubpassDescription subpass{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentRef,
      .pDepthStencilAttachment = &depthAttachmentRef,
  };

  VkSubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
  };

  VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 2,
      .pAttachments = attachments,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
  };

  VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass),
           "create render pass");
}

RenderPass::RenderPass(RenderPass&& rhs) : device{rhs.device} {
  renderPass = rhs.renderPass;
  rhs.renderPass = VK_NULL_HANDLE;
}

RenderPass::~RenderPass() { vkDestroyRenderPass(device, renderPass, nullptr); }

}  // namespace uron
