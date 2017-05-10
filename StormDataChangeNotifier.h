#pragma once

#include <string>

#include "StormData.h"
#include "StormDataChangeType.refl.h"

#ifdef STORM_CHANGE_NOTIFIER

struct ReflectionChangeNotification
{
  static const uint64_t kInvalidSubIndex = ~0;

  ReflectionNotifyChangeType m_Type = ReflectionNotifyChangeType::kSet;
  uint64_t m_SubIndex = kInvalidSubIndex;
  std::string m_Path;
  std::string m_Data;
};

bool DoNotifyCallback(StormReflectionParentInfo & parent_info);

void FinishChangeNotification(StormReflectionParentInfo * parent_info, ReflectionChangeNotification & notification);

void ReflectionNotifySet(StormReflectionParentInfo & parent_info, bool value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int8_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int16_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int32_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, int64_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint8_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint16_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint32_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, uint64_t value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, float value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, double value);
void ReflectionNotifySet(StormReflectionParentInfo & parent_info, const std::string & value);

void ReflectionNotifySetObject(StormReflectionParentInfo & parent_info, const std::string & value);

void ReflectionNotifyClearObject(StormReflectionParentInfo & parent_info);
void ReflectionNotifyCompress(StormReflectionParentInfo & parent_info);

void ReflectionNotifyInsertObject(StormReflectionParentInfo & parent_info, uint64_t index, const std::string & data);
void ReflectionNotifyRemoveObject(StormReflectionParentInfo & parent_info, uint64_t index);

#endif
