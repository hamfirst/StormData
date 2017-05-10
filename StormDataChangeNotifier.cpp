
#include <StormRefl/StormReflJsonStd.h>
#include <hash/Hash64.h>
#include <optional/optional.hpp>

#include "StormDataChangeNotifier.h"

void CreateElementPath(StormReflectionParentInfo * parent_info, ReflectionChangeNotification & notification)
{
  if (parent_info->m_ParentIndex != StormReflectionParentInfo::kInvalidParentIndex)
  {
    notification.m_Path = std::string("[") + std::to_string(parent_info->m_ParentIndex) + "]" + notification.m_Path;
  }

  if (parent_info->m_MemberName)
  {
    notification.m_Path = std::string(".") + parent_info->m_MemberName + notification.m_Path;
  }
}

bool DoNotifyCallback(StormReflectionParentInfo & parent_info)
{
  return parent_info.m_Callback || (parent_info.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;
}

void FinishChangeNotification(StormReflectionParentInfo * parent_info, ReflectionChangeNotification & notification)
{
  while(true)
  {
    if (parent_info->m_Callback)
    {
      parent_info->m_Callback(parent_info->m_CallbackUserPtr, notification);
    }

    auto new_parent_info = parent_info->m_ParentInfo;
    if (new_parent_info == nullptr || DoNotifyCallback(*new_parent_info) == false)
    {
      return;
    }

    CreateElementPath(parent_info, notification);
    parent_info = new_parent_info;
  }
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, bool value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int8_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int16_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int32_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int64_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint8_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint16_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint32_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint64_t value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, float value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, double value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, const std::string & value)
{
  ReflectionChangeNotification notification;
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifySetObject(StormReflectionParentInfo & parent_info, const std::string & value)
{
  ReflectionChangeNotification notification;
  notification.m_Data = value;
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifyClearObject(StormReflectionParentInfo & parent_info)
{
  ReflectionChangeNotification notification;
  notification.m_Type = ReflectionNotifyChangeType::kClear;
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifyCompress(StormReflectionParentInfo & parent_info)
{
  ReflectionChangeNotification notification;
  notification.m_Type = ReflectionNotifyChangeType::kCompress;
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifyInsertObject(StormReflectionParentInfo & parent_info, uint64_t index, const std::string & data)
{
  ReflectionChangeNotification notification;
  notification.m_Type = ReflectionNotifyChangeType::kInsert;
  notification.m_Data = data;
  notification.m_SubIndex = index;
  FinishChangeNotification(&parent_info, notification);
}

void ReflectionNotifyRemoveObject(StormReflectionParentInfo & parent_info, uint64_t index)
{
  ReflectionChangeNotification notification;
  notification.m_Type = ReflectionNotifyChangeType::kRemove;
  notification.m_SubIndex = index;
  FinishChangeNotification(&parent_info, notification);
}
