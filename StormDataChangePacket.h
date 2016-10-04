#pragma once

#include <string>

#include "StormDataChangeNotifier.h"
#include "StormDataChangePacketHelpers.h"

std::string StormDataCreateChangePacket(const ReflectionChangeNotification & notification);

template <class T>
bool StormDataApplyChangePacket(T & t, const char * str)
{
  ReflectionNotifyChangeType type;

  if (StormDataChangePacketHelpers::ParseNotifyChangeType(type, str, str) == false)
  {
    return false;
  }

  switch (type)
  {
  case ReflectionNotifyChangeType::kSet:
    return StormDataChangePacketHelpers::StormDataApplySetPacket::Process(t, str);
  case ReflectionNotifyChangeType::kClear:
    return StormDataChangePacketHelpers::StormDataApplyClearPacket::Process(t, str);
  case ReflectionNotifyChangeType::kCompress:
    return StormDataChangePacketHelpers::StormDataApplyCompressPacket::Process(t, str);
  case ReflectionNotifyChangeType::kInsert:
    return StormDataChangePacketHelpers::StormDataApplyInsertPacket::Process(t, str);
  case ReflectionNotifyChangeType::kRemove:
    return StormDataChangePacketHelpers::StormDataApplyRemovePacket::Process(t, str);
  }

  return false;
}
