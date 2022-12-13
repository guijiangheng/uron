#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace uron {

#define NON_COPYABLE(ClassName)         \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(const ClassName&) = delete

std::vector<char> loadFile(std::string filename);

}  // namespace uron
