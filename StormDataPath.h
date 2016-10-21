#pragma once

#include "StormDataPathHelpers.h"

template <typename T, typename Visitor>
bool StormDataVisitPath(T & t, Visitor & visitor, const char * str)
{
  if (*str == 0 || *str == ' ')
  {
    return visitor(t, str);
  }

  auto recall_visitor = [&](auto & new_t, const char * new_str)
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

