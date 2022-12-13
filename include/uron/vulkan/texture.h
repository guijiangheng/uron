#pragma once

#include <future>
#include <memory>

#include "uron/common.h"
#include "uron/vulkan/vulkan.h"

namespace uron {

class Device;
class CommandPool;
class Image;
class ImageView;
class Sampler;

class TextureResource {
 public:
  TextureResource(std::string path);

  TextureResource(TextureResource&& other) = default;

  ~TextureResource();

  int size() const { return width * height * 4; }

  int getWidth() const { return width; }

  int getHeight() const { return height; }

  int getChannels() const { return channels; }

  void* getPixels() { return pixels.valid() ? pixels.get() : nullptr; }

 private:
  int width;
  int height;
  int channels;
  std::future<unsigned char*> pixels;
};

class Texture {
 public:
  NON_COPYABLE(Texture);

  Texture(const Device& device, const CommandPool& commandPool,
          TextureResource& resource, VkImageType imageType = VK_IMAGE_TYPE_2D,
          VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
          VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
          VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          float minLod = 0.0f, float maxLod = 0.0f);

  operator VkImage() const;

  operator VkImageView() const;

  operator VkSampler() const;

 private:
  std::unique_ptr<Image> image;
  std::unique_ptr<ImageView> imageView;
  std::unique_ptr<Sampler> sampler;
};

}  // namespace uron
