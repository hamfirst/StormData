#pragma once

#include <cstdint>
#include <type_traits>

struct ReflectionChangeNotification;

typedef void(*StormDataNotifyCallback)(void * user_ptr, const ReflectionChangeNotification & change);

enum class StormDataParentInfoFlags : uint32_t
{
  kNone = 0,
  kParentHasCallback = 0x01,
};

struct StormReflectionParentInfo
{
  static const uint64_t kInvalidParentIndex = ~0;

  uint32_t m_Flags = (uint32_t)StormDataParentInfoFlags::kNone;
  uint64_t m_ParentIndex = kInvalidParentIndex;
  const char * m_MemberName = nullptr;
  StormReflectionParentInfo * m_ParentInfo = nullptr;
  StormDataNotifyCallback m_Callback = nullptr;
  void * m_CallbackUserPtr = nullptr;
};
