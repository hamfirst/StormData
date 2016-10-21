#pragma once

#include <string>

#include "StormDataChangeNotifier.h"
#include "StormDataChangePacketHelpers.h"

std::string StormDataCreateChangePacket(const ReflectionChangeNotification & notification);
std::string StormDataCreateChangePacket(ReflectionNotifyChangeType type, uint64_t sub_index, const std::string & path, const std::string & data);

template <class T>
bool StormDataApplyChangePacketSet(T & t, const char * path, const char * data)
{
  if (*path == 0)
  {
    return StormDataChangePacketHelpers::StormDataApplySet<T>::Process(t, data);
  }
  else if (*path == ' ')
  {
    path++;
    return StormDataChangePacketHelpers::StormDataApplySet<T>::Process(t, path);
  }

  auto visitor = [&](auto & new_t, const char * new_str)
  {
    return StormDataApplyChangePacketSet(new_t, new_str, data);
  };

  return StormDataVisitPathElement(t, visitor, path);
}

template <class T>
bool StormDataApplyChangePacketClear(T & t, const char * path)
{
  if (*path == 0 || *path == ' ')
  {
    return StormDataChangePacketHelpers::StormDataApplyClear<T>::Process(t);
  }

  auto visitor = [&](auto & new_t, const char * new_str)
  {
    return StormDataApplyChangePacketClear(new_t, new_str);
  };

  return StormDataVisitPathElement(t, visitor, path);
}

template <class T>
bool StormDataApplyChangePacketCompress(T & t, const char * path)
{
  if (*path == 0 || *path == ' ')
  {
    return StormDataChangePacketHelpers::StormDataApplyCompress<T>::Process(t);
  }

  auto visitor = [&](auto & new_t, const char * new_str)
  {
    return StormDataApplyChangePacketCompress(new_t, new_str);
  };

  return StormDataVisitPathElement(t, visitor, path);
}

template <class T>
bool StormDataApplyChangePacketInsert(T & t, const char * path, uint64_t index, const char * data)
{
  if (*path == 0)
  {
    return StormDataChangePacketHelpers::StormDataApplyInsert<T>::Process(t, index, data);
  }
  else if(*path == ' ')
  {
    path++;
    if (StormDataChangePacketHelpers::ParseIndex(index, path, path) == false)
    {
      return false;
    }

    if (*path != ' ')
    {
      return false;
    }

    path++;
    return StormDataChangePacketHelpers::StormDataApplyInsert<T>::Process(t, index, path);
  }

  auto visitor = [&](auto & new_t, const char * new_str)
  {
    return StormDataApplyChangePacketInsert(new_t, new_str, index, data);
  };

  return StormDataVisitPathElement(t, visitor, path);
}

template <class T>
bool StormDataApplyChangePacketRemove(T & t, const char * path, uint64_t index)
{
  if (*path == 0)
  {
    return StormDataChangePacketHelpers::StormDataApplyRemove<T>::Process(t, index);
  }
  else if (*path == ' ')
  {
    path++;
    if (StormDataChangePacketHelpers::ParseIndex(index, path, path) == false)
    {
      return false;
    }

    if (*path != ' ')
    {
      return false;
    }

    return StormDataChangePacketHelpers::StormDataApplyRemove<T>::Process(t, index);
  }

  auto visitor = [&](auto & new_t, const char * new_str)
  {
    return StormDataApplyChangePacketRemove(new_t, new_str, index);
  };

  return StormDataVisitPathElement(t, visitor, path);
}

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
    return StormDataApplyChangePacketSet(t, str, nullptr);
  case ReflectionNotifyChangeType::kClear:
    return StormDataApplyChangePacketClear(t, str);
  case ReflectionNotifyChangeType::kCompress:
    return StormDataApplyChangePacketCompress(t, str);
  case ReflectionNotifyChangeType::kInsert:
    return StormDataApplyChangePacketInsert(t, str, ~0, nullptr);
  case ReflectionNotifyChangeType::kRemove:
    return StormDataApplyChangePacketRemove(t, str, ~0);
  }

  return false;
}

template <class T>
bool StormDataApplyChangePacket(T & t, ReflectionNotifyChangeType type, const char * path, uint64_t index, const char * data)
{
  switch (type)
  {
  case ReflectionNotifyChangeType::kSet:
    return StormDataApplyChangePacketSet(t, path, data);
  case ReflectionNotifyChangeType::kClear:
    return StormDataApplyChangePacketClear(t, path);
  case ReflectionNotifyChangeType::kCompress:
    return StormDataApplyChangePacketCompress(t, path);
  case ReflectionNotifyChangeType::kInsert:
    return StormDataApplyChangePacketInsert(t, path, index, data);
  case ReflectionNotifyChangeType::kRemove:
    return StormDataApplyChangePacketRemove(t, path, index);
  }

  return false;
}
