#pragma once

#include "Windows.hpp"
#include <fmt/format.h>

#define SIMP_LOG_CRITICAL(...)  simp::Log::getLogger().critical(__VA_ARGS__)
#define SIMP_LOG_ERROR(...)     simp::Log::getLogger().error(__VA_ARGS__)
#define SIMP_LOG_WARNING(...)   simp::Log::getLogger().warn(__VA_ARGS__)
#define SIMP_LOG_INFO(...)      simp::Log::getLogger().info(__VA_ARGS__)
#define SIMP_LOG_TRACE(...)     simp::Log::getLogger().trace(__VA_ARGS__)

#ifndef NDEBUG
#define SIMP_LOG_DEBUG(...)     simp::Log::getLogger().debug(__VA_ARGS__)
#define SIMP_HRESULT(hr) \
if(!SUCCEEDED(_hResult = (hr))) { \
  std::string _desc = simp::HrTranslate(_hResult); \
  /* PT_LOG_CRITICAL("{}:{} {} returned {}", __FILE__, __LINE__, #hr, _desc); \
  pt::Log::getLogger().flush();*/ \
  std::string _message = fmt::format("{} returned {} ({:08x})\nin {}:{}", #hr, _desc, (uint32_t)_hResult, __FILE__, __LINE__); \
  MessageBoxA( \
    nullptr, \
    _message.c_str(), \
    "Operation failed", \
    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL); \
  abort(); \
}
#define SIMP_ASSERT(...) \
if(!(__VA_ARGS__)) { \
  /* PT_LOG_CRITICAL("{}:{} Assertion failed: {}", __FILE__, __LINE__, #__VA_ARGS__); \
  pt::Log::getLogger().flush(); */ \
  std::string _message = fmt::format("Assertion failed: {}\nin {}:{}", #__VA_ARGS__, __FILE__, __LINE__); \
  MessageBoxA( \
    nullptr, \
    _message.c_str(), \
    "Debug assertion failed", \
    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL); \
  abort(); \
}
#else
#define SIMP_LOG_DEBUG(...)
#define SIMP_ASSERT(...)
#define SIMP_HRESULT(hr) \
if(!SUCCEEDED(_hResult = (hr))) { \
  std::string _desc = simp::HrTranslate(_hResult); \
  /* PT_LOG_CRITICAL(_desc); \
  pt::Log::getLogger().flush(); */ \
  MessageBoxA( \
    nullptr, \
    _desc.c_str(), \
    "Operation failed", \
    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL); \
  abort(); \
}
#endif

#define SIMP_GETLASTERROR(op) \
if(!(op)) { \
  SIMP_HRESULT(GetLastError()); \
}
