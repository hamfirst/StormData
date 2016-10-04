#pragma once

#include "StormDataPathHelpers.h"

template <typename T, typename Visitor>
bool StormDataVisitPath(T & t, Visitor & visitor, const char * str)
{
  return StormDataPathHelpers::StormDataPath<T>::VisitPath(t, visitor, str);
}

