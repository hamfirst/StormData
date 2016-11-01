
#include <StormRefl/StormReflJsonStd.h>
#include <hash/Hash64.h>
#include <optional/optional.hpp>

#include "StormDataChangeNotifier.h"

thread_local std::vector<std::experimental::optional<std::function<void(const ReflectionChangeNotification &)>>> g_NotifyCallback;


bool DoReflectionCallback()
{
  return g_NotifyCallback.size() > 0 && g_NotifyCallback.back();
}

void CreateElementPath(StormReflectionParentInfo * parent_info, ReflectionChangeNotification & notification)
{
  if (parent_info->m_ParentInfo)
  {
    CreateElementPath(parent_info->m_ParentInfo, notification);

    if (parent_info->m_MemberName)
    {
      notification.m_Path.push_back('.');
      notification.m_Path.append(parent_info->m_MemberName);
    }

    if (parent_info->m_ParentIndex != StormReflectionParentInfo::kInvalidParentIndex)
    {
      notification.m_Path.push_back('[');
      notification.m_Path.append(std::to_string(parent_info->m_ParentIndex));
      notification.m_Path.push_back(']');
    }
  }
  else
  {
    notification.m_BaseObject = parent_info->m_MemberName;
  }
}

void ReflectionPushNotifyCallback(std::function<void(const ReflectionChangeNotification &)> && func)
{
  g_NotifyCallback.emplace_back(std::move(func));
}

void ReflectionPushNotifyEmptyCallback()
{
  g_NotifyCallback.emplace_back(std::experimental::optional<std::function<void(const ReflectionChangeNotification &)>>{});
}

void ReflectionPopNotifyCallback()
{
  g_NotifyCallback.pop_back();
}

ReflectionChangeNotification InitChangeNotification(StormReflectionParentInfo * parent_info)
{
  ReflectionChangeNotification notification;
  CreateElementPath(parent_info, notification);

  return notification;
}

void FinishChangeNotification(const ReflectionChangeNotification & notification)
{
  if (g_NotifyCallback.size() > 0)
  {
    (*g_NotifyCallback.back())(notification);
  }
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, bool value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int8_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int16_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int32_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int64_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint8_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint16_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint32_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint64_t value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, float value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, const std::string & value)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  StormReflEncodeJson(value, notification.m_Data);
  FinishChangeNotification(notification);
}

void ReflectionNotifyClearObject(StormReflectionParentInfo & parent_info)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  notification.m_Type = ReflectionNotifyChangeType::kClear;
  FinishChangeNotification(notification);
}

void ReflectionNotifyCompress(StormReflectionParentInfo & parent_info)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  notification.m_Type = ReflectionNotifyChangeType::kCompress;
  FinishChangeNotification(notification);
}

void ReflectionNotifyInsertObject(StormReflectionParentInfo & parent_info, uint64_t index, const std::string & data)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  notification.m_Type = ReflectionNotifyChangeType::kInsert;
  notification.m_Data = data;
  notification.m_SubIndex = index;
  FinishChangeNotification(notification);
}

void ReflectionNotifyRemoveObject(StormReflectionParentInfo & parent_info, uint64_t index)
{
  ReflectionChangeNotification notification = InitChangeNotification(&parent_info);
  notification.m_Type = ReflectionNotifyChangeType::kRemove;
  notification.m_SubIndex = index;
  FinishChangeNotification(notification);
}
