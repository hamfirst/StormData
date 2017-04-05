
#include "StormDataChangePacket.h"
#include "StormDataChangeType.refl.meta.h"

#include <StormRefl/StormReflMetaEnum.h>

std::string StormDataCreateChangePacket(const ReflectionChangeNotification & notification)
{
  std::string change_packet = StormReflGetEnumAsString(notification.m_Type);
  change_packet += ' ';
  change_packet += notification.m_Path;

  if (notification.m_SubIndex != ReflectionChangeNotification::kInvalidSubIndex)
  {
    change_packet += ' ';
    change_packet += std::to_string(notification.m_SubIndex);
  }

  if (notification.m_Data.size() > 0)
  {
    change_packet += ' ';
    change_packet += notification.m_Data;
  }

  return change_packet;
}

std::string StormDataCreateChangePacket(ReflectionNotifyChangeType type, uint64_t sub_index, const std::string & path, const std::string & data)
{
  std::string change_packet = StormReflGetEnumAsString(type);
  change_packet += ' ';
  change_packet += path;

  if (sub_index != ReflectionChangeNotification::kInvalidSubIndex)
  {
    change_packet += ' ';
    change_packet += std::to_string(sub_index);
  }

  if (data.size() > 0)
  {
    change_packet += ' ';
    change_packet += data;
  }

  return change_packet;
}

bool StormDataParseChangePacket(ReflectionChangeNotification & notification, const char * data)
{
  return StormDataParseChangePacket(notification, data, data);
}

bool StormDataParseChangePacket(ReflectionChangeNotification & notification, const char * data, const char *& result)
{
  ReflectionNotifyChangeType type;

  if (StormDataChangePacketHelpers::ParseNotifyChangeType(type, data, data) == false)
  {
    return false;
  }

  notification.m_Type = type;

  while (*data != ' ' && *data != 0)
  {
    notification.m_Path += *data;
    data++;
  }

  const char * start_data;
  uint64_t index;

  switch (type)
  {
  case ReflectionNotifyChangeType::kSet:
    if (*data == 0)
    {
      return false;
    }

    data++;
    start_data = data;
    if (StormReflJsonParseOverValue(data, data) == false)
    {
      return false;
    }

    notification.m_Data = std::string(start_data, data);
    return true;
  case ReflectionNotifyChangeType::kClear:
  case ReflectionNotifyChangeType::kCompress:
    if (*data != 0)
    {
      return false;
    }

    return true;
  case ReflectionNotifyChangeType::kInsert:
    if (*data == 0)
    {
      return false;
    }

    data++;
    if (StormReflParseJson(index, data, data) == false)
    {
      return false;
    }

    notification.m_SubIndex = index;

    if (*data != ' ')
    {
      return false;
    }
    data++;

    start_data = data;
    if (StormReflJsonParseOverValue(data, data) == false)
    {
      return false;
    }

    notification.m_Data = std::string(start_data, data);
    return true;
  case ReflectionNotifyChangeType::kRemove:
    if (*data == 0)
    {
      return false;
    }

    data++;
    if (StormReflParseJson(index, data, data) == false)
    {
      return false;
    }

    notification.m_SubIndex = index;
    return true;
  case ReflectionNotifyChangeType::kRevert:
    if (*data != 0)
    {
      return false;
    }

    return true;
  }

  return false;
}

