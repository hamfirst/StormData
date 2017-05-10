#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class T>
class ROpaque
{
public:
  ROpaque()
  {

  }

  ROpaque(const T & t) :
    m_T(t)
  {

  }

  ROpaque(T && t) :
    m_T(std::move(t))
  {

  }

  ROpaque(const ROpaque<T> & rhs) :
    m_T(rhs.m_T)
  {

  }

  ROpaque(ROpaque<T> && rhs) :
    m_T(std::move(rhs.m_T))
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif
  }

  ROpaque<T> & operator = (const ROpaque<T> & rhs)
  {
    m_T = rhs.m_T;
    Modified();
    return *this;
  }

  ROpaque<T> & operator = (const ROpaque<T> && rhs)
  {
    m_T = std::move(rhs.m_T);
    Modified();
    return *this;
  }

  ROpaque<T> & operator = (const T & rhs)
  {
    m_T = rhs;
    Modified();
    return *this;
  }

  ROpaque<T> & operator = (T && rhs)
  {
    m_T = std::move(rhs);
    Modified();
    return *this;
  }

  const T * operator * () const
  {
    return &m_T;
  }

  const T * operator -> () const
  {
    return &m_T;
  }

  const T & Value() const
  {
    return m_T;
  }

  const T * GetPtr() const
  {
    return &m_T;
  }

  bool operator == (const T & rhs) const
  {
    return m_T == rhs;
  }

  bool operator == (const ROpaque<T> & rhs) const
  {
    return m_T == rhs.m_T;
  }

  template <typename Visitor>
  void Modify(Visitor && visitor)
  {
    visitor(m_T);
    Modified();
  }

#ifdef STORM_CHANGE_NOTIFIER
  ROpaque(ROpaque<T> && rhs, StormReflectionParentInfo * new_parent) :
    m_T(std::move(rhs.m_T))
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
  }
#endif

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(ROpaque<T> && val, StormReflectionParentInfo * new_parent)
  {
    m_T = val.m_T;
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  void SetRaw(T && rhs)
  {
    m_T = std::move(rhs);
  }

  void SetRaw(ROpaque<T> && rhs)
  {
    m_T = std::move(rhs.m_T);
  }

  void SetRaw(const T & rhs)
  {
    m_T = rhs;
  }

  void SetRaw(const ROpaque<T> & rhs)
  {
    m_T = rhs.m_T;
  }

private:

  void Modified()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(m_T));
#endif
  }

  T m_T;
  STORM_CHANGE_NOTIFIER_INFO;
};
