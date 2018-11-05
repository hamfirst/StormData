#pragma once

#include "StormDataParentInfo.h"
#include "StormDataPathHelpers.h"

#include <string>
#include <vector>

template <typename T>
std::string StormDataGetPath(const T & t)
{
  std::string path;
  const StormReflectionParentInfo * parent_info = &t.m_ReflectionInfo;

  while (parent_info)
  {
    if (parent_info->m_ParentIndex != StormReflectionParentInfo::kInvalidParentIndex)
    {
      path = std::string("[") + std::to_string(parent_info->m_ParentIndex) + "]" + path;
    }

    if (parent_info->m_MemberName)
    {
      path = std::string(".") + parent_info->m_MemberName + path;
    }

    parent_info = parent_info->m_ParentInfo;
  }

  return path;
}

template <typename T, typename Visitor>
bool StormDataVisitPath(T & t, Visitor & visitor, const char * str)
{
  if (*str == 0 || *str == ' ')
  {
    return visitor(t, str);
  }

  auto recall_visitor = [&](auto & new_t, auto * field_data, const char * new_str)
  {
    return StormDataVisitPath(new_t, visitor, new_str);
  };

  return StormDataPathHelpers::StormDataPath<T>::VisitPath(t, recall_visitor, str);
}

template <typename T, typename Visitor>
bool StormDataVisitPathElement(T & t, Visitor & visitor, const char * str)
{
  return StormDataPathHelpers::StormDataPath<T>::VisitPath(t, visitor, str);
}

bool StormDataMatchPathExact(const char * path, const char * test);
bool StormDataMatchPathPartial(const char * path, const char * partial);
bool StormDataMatchPathWildcard(const char * path, const char * test);

enum class StormDataPathComponentType
{
  kFieldName,
  kIndex,
};

struct StormDataPathComponent
{
  StormDataPathComponentType m_Type;
  std::string m_Value;
};

bool StormDataParsePathComponents(const char * path, std::vector<StormDataPathComponent> & comps);
std::string StormDataConvertPathComponentList(const std::vector<StormDataPathComponent> & comps);

inline bool StormDataParsePath(const char * path, const char * pattern)
{
  return StormDataMatchPathPartial(path, pattern);
}

template <typename ... Args>
bool StormDataParsePath(const char * path, const char * pattern, uint64_t * index, Args && ... args)
{
  while (true)
  {
    if (*pattern == 0)
    {
      return false;
    }

    if (*path == '.')
    {
      if (*pattern != '.')
      {
        return false;
      }

      path++;
      pattern++;
      while (*path != 0 && *path != '.' && *path != '[' && *path != ']')
      {
        if (*path != *pattern)
        {
          return false;
        }

        path++;
        pattern++;
      }

      if (*path != *pattern)
      {
        return false;
      }
    }
    if (*path == '[')
    {
      if (*pattern != '[')
      {
        return false;
      }

      path++;
      pattern++;

      if (*pattern == '?')
      {
        path++;

        if (*path < '0' || *path > '9')
        {
          return false;
        }

        uint64_t val = *path - '0';
        
        path++;
        while (*path != ']')
        {
          if (*path == 0)
          {
            return false;
          }

          if (*path < '0' || *path > '9')
          {
            return false;
          }

          val *= 10;
          val += *path - '0';
        }

        if (*path != *pattern)
        {
          return false;
        }

        path++;
        pattern++;

        return StormDataParsePath(path, pattern, std::forward<Args>(args)...);
      }
    }
  }
}
