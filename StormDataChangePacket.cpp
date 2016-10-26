
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
