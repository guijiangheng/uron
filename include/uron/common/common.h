#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define NON_COPYABLE(ClassName)                    \
  ClassName(const ClassName&) = delete;            \
  ClassName(ClassName&&) = delete;                 \
  ClassName& operator=(const ClassName&) = delete; \
  ClassName& operator=(const ClassName&&) = delete;
