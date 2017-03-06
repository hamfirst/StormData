#pragma once

#include <utility>

namespace StormDataHelpers
{
  template <class A, class B> struct RTempPair
  {
    RTempPair(std::pair<A, B> && pair_data) :
      m_Data(pair_data)
    {

    }

    template <class AA, class BB>
    RTempPair(AA && a, BB && b) :
      m_Data(a, b)
    {
    }

    RTempPair(RTempPair && rhs) = default;

    std::pair<A, B> * operator ->()
    {
      return &m_Data;
    }

  private:
    std::pair<A, B> m_Data;
  };
}




