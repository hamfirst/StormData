#pragma once

#include <string>

#include "StormDataChangeNotifier.h"
#include "StormDataChangePacketHelpers.h"

std::string StormDataCreateChangePacket(const ReflectionChangeNotification & notification);
std::string StormDataCreateChangePacket(ReflectionNotifyChangeType type, uint64_t sub_index, const std::string & path, const std::string & data);

bool StormDataParseChangePacket(ReflectionChangeNotification & notification, const char * data);
bool StormDataParseChangePacket(ReflectionChangeNotification & notification, const char * data, const char *& result);


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
    return StormDataChangePacketHelpers::StormDataApplyChangePacketSet<T>::Process(t, str, nullptr);
  case ReflectionNotifyChangeType::kClear:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketClear<T>::Process(t, str);
  case ReflectionNotifyChangeType::kCompress:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketCompress<T>::Process(t, str);
  case ReflectionNotifyChangeType::kInsert:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketInsert<T>::Process(t, str, ~0, nullptr);
  case ReflectionNotifyChangeType::kRemove:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketRemove<T>::Process(t, str, ~0);
  case ReflectionNotifyChangeType::kRevert:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketRevertDefault<T>::Process(t, str);
  //case ReflectionNotifyChangeType::kRearrange:
  //  return StormDataChangePacketHelpers::StormDataApplyChangePacketRearrangeDefault<T>::Process(t, str);
  default:
    return false;
  }

  return false;
}

template <class T>
bool StormDataApplyChangePacket(T & t, ReflectionNotifyChangeType type, const char * path, uint64_t index, const char * data)
{
  switch (type)
  {
  case ReflectionNotifyChangeType::kSet:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketSet<T>::Process(t, path, data);
  case ReflectionNotifyChangeType::kClear:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketClear<T>::Process(t, path);
  case ReflectionNotifyChangeType::kCompress:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketCompress<T>::Process(t, path);
  case ReflectionNotifyChangeType::kInsert:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketInsert<T>::Process(t, path, index, data);
  case ReflectionNotifyChangeType::kRemove:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketRemove<T>::Process(t, path, index);
  case ReflectionNotifyChangeType::kRevert:
    return StormDataChangePacketHelpers::StormDataApplyChangePacketRevertDefault<T>::Process(t, path);
  default:
    return false;
  }
}

template <class T>
bool StormDataApplyChangePacket(T & t, const ReflectionChangeNotification & notification)
{
  return StormDataApplyChangePacket(t, notification.m_Type, notification.m_Path.data(), notification.m_SubIndex, notification.m_Data.data());
}
