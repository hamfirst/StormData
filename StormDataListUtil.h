#pragma once

#include <vector>

#include "StormDataList.h"


template <typename T>
void MergeListRemoveDuplicates(RMergeList<T> & list)
{
  std::vector<std::size_t> dead_indices;

  for (auto itr = list.begin(), end = list.end(); itr != end; ++itr)
  {
    auto next_itr = itr;
    for (++next_itr; next_itr != end; ++next_itr)
    {
      if (next_itr->second == itr->second)
      {
        dead_indices.emplace_back(next_itr->first);
      }
    }
  }

  for (auto itr = dead_indices.rbegin(), end = dead_indices.rend(); itr != end; ++itr)
  {
    if (list.HasAt(*itr))
    {
      list.RemoveAt(*itr);
    }
  }
}

template <typename T>
void MergeListRemoveDuplicates(RSparseList<T> & list)
{
  std::vector<std::size_t> dead_indices;

  for (auto itr = list.begin(), end = list.end(); itr != end; ++itr)
  {
    auto next_itr = itr;
    for (++next_itr; next_itr != end; ++next_itr)
    {
      if (next_itr->second == itr->second)
      {
        dead_indices.emplace_back(next_itr->first);
      }
    }
  }

  for (auto itr = dead_indices.rbegin(), end = dead_indices.rend(); itr != end; ++itr)
  {
    if (list.HasAt(*itr))
    {
      list.RemoveAt(*itr);
    }
  }
}


