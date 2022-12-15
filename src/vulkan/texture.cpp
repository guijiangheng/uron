#include "uron/vulkan/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "uron/vulkan/buffer.h"
#include "uron/vulkan/commandpool.h"
#include "uron/vulkan/device.h"
#include "uron/vulkan/image.h"
#include "uron/vulkan/imageview.h"
#include "uron/vulkan/sampler.h"

namespace uron {

TextureResource::TextureResource(std::string path) {
  pixels = std::async(std::launch::async, [&, path]() {
    auto mPixels =
        stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (mPixels == nullptr) {
      throw std::runtime_error("failed to load image");
    }

    return mPixels;
  });
}

TextureResource::~TextureResource() {
  if (pixels.valid()) {
    stbi_image_free(pixels.get());
  }
}

Texture::Texture(const Device& device, const CommandPool& commandPool,
                 TextureResource& resource, VkImageType imageType,
                 VkFormat format, VkImageTiling tiling,
                 VkSamplerAddressMode addressMode, float minLod, float maxLod) {
  auto stagingBuffer =
      device.createBuffer(resource.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stagingBuffer.write(resource.getPixels(), 0, resource.size());

  auto extent = VkExtent2D{static_cast<uint32_t>(resource.getWidth()),
                           static_cast<uint32_t>(resource.getHeight())};

  image.reset(
      new Image(device, extent, imageType, format, tiling,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

  image->transitionImageLayout(commandPool, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  image->copy(commandPool, stagingBuffer);
  image->transitionImageLayout(commandPool,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  sampler.reset(new Sampler(device, addressMode, minLod, maxLod));
  imageView.reset(new ImageView(device, *image, format));
}

Texture::operator VkImage() const { return *image; }

Texture::operator VkImageView() const { return *imageView; }

Texture::operator VkSampler() const { return *sampler; }

}  // namespace uron
