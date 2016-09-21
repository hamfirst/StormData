
#include "StormDataChangePacket.h"

std::string StormDataCreateChangePacket(const ReflectionChangeNotification & notification)
{
  std::string change_packet = notification.m_Type._to_string();
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
