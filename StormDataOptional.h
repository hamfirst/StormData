
#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class T>
class ROptional
{
public:
  using MyType = T;

  ROptional() :
      m_Valid(false)
  {
#ifdef STORM_CHANGE_NOTIFIER
    new (&m_ReflectionInfo) StormReflectionParentInfo();
#endif
  }

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional(const ROptional<T> & rhs)
  {
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      const T * src = reinterpret_cast<const T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
    }
    else
    {
#ifdef STORM_CHANGE_NOTIFIER
      new (&m_ReflectionInfo) StormReflectionParentInfo();
#endif
    }
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional(ROptional<T> && rhs)
  {
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      rhs.ClearInternal();
    }
    else
    {
#ifdef STORM_CHANGE_NOTIFIER
      new (&m_ReflectionInfo) StormReflectionParentInfo(rhs.m_ReflectionInfo);
      rhs.m_ReflectionInfo = {};
#endif
    }
  }

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional(const T & rhs) :
      m_Valid(true)
  {
    new(m_Buffer) T(rhs);
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional(T && rhs) :
      m_Valid(true)
  {
    new(m_Buffer) T(std::move(rhs));
  }

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional<T> & operator =(const ROptional<T> & rhs)
  {
#ifdef STORM_CHANGE_NOTIFIER
    auto prev_valid = m_Valid;
    if(m_Valid == false && rhs.m_Valid)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      const T * src = reinterpret_cast<const T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
    }

    Modified();
    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional<T> & operator =(ROptional<T> && rhs)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if(m_Valid == false && rhs.m_Valid)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      rhs.ClearInternal();
    }

    Modified();
    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional<T> & operator =(const T & rhs)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if(m_Valid == false && rhs.m_Valid)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();

    m_Valid = true;
    new(m_Buffer) T(rhs);

    Modified();
    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional<T> & operator =(T && rhs)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if(m_Valid == false && rhs.m_Valid)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();
    m_Valid = true;
    new(m_Buffer) T(std::move(rhs));

    Modified();
    return *this;
  }

  ~ROptional()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if(m_Valid == false)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();
  }

#ifdef STORM_CHANGE_NOTIFIER
  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional(ROptional<T> && rhs, StormReflectionParentInfo * new_parent)
  {
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
      rhs.ClearInternal();
    }
    else
    {
#ifdef STORM_CHANGE_NOTIFIER
      new (&m_ReflectionInfo) StormReflectionParentInfo(rhs.m_ReflectionInfo);
      rhs.m_ReflectionInfo = {};
#endif
    }

    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
  }
#endif

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(ROptional<T> && val, StormReflectionParentInfo * new_parent)
  {
    if(m_Valid && val.m_Valid)
    {
      T * src = reinterpret_cast<T *>(val.m_Buffer);
      T * dst = reinterpret_cast<T *>(m_Buffer);

      dst->Relocate(std::move(*src), new_parent);
      return;
    }

    ClearInternal();
    m_Valid = val.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(val.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      val.ClearInternal();
    }
    else
    {
      new (&m_ReflectionInfo) StormReflectionParentInfo();
    }

    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional<T> &  SetRaw(const ROptional<T> & rhs)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if(m_Valid == false && rhs.m_Valid)
    {
      m_ReflectionInfo.~StormReflectionParentInfo();
    }
#endif

    ClearInternal();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      const T * src = reinterpret_cast<const T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
    }

    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional<T> & SetRaw(ROptional<T> && rhs)
  {
    ClearInternal();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      rhs.Clear();
    }

    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
  ROptional<T> & SetRaw(const T & rhs)
  {
    ClearInternal();
    m_Valid = true;
    new(m_Buffer) T(rhs);

    return *this;
  }

  template <typename U = T, typename std::enable_if<std::is_move_constructible<U>::value, int>::type = 0>
  ROptional<T> & SetRaw(T && rhs)
  {
    ClearInternal();
    m_Valid = true;
    new(m_Buffer) T(std::move(rhs));

    return *this;
  }

  template <typename ... Args>
  T & Emplace(Args && ... args)
  {
    ClearInternal();

    T * src  = new(m_Buffer) T(std::forward<Args>(args)...);
    m_Valid = true;

    Modified();
    return *src;
  }

  void Clear()
  {
    ClearInternal();
    Modified();
  }

  operator bool() const
  {
    return m_Valid;
  }

  bool IsValid() const
  {
    return m_Valid;
  }

  T & Value()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<T *>(m_Buffer);
  }

  const T & Value() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<const T *>(m_Buffer);
  }

  T & operator *()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<T *>(m_Buffer);
  }

  const T & operator *() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<const T *>(m_Buffer);
  }

  T * operator ->()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return reinterpret_cast<T *>(m_Buffer);
  }

  const T * operator ->() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return reinterpret_cast<const T *>(m_Buffer);
  }

  T * GetPtr()
  {
    if (!m_Valid)
    {
      return nullptr;
    }

    return reinterpret_cast<T *>(m_Buffer);
  }

  const T * GetPtr() const
  {
    if (!m_Valid)
    {
      return nullptr;
    }

    return reinterpret_cast<const T *>(m_Buffer);
  }

private:

  void ClearInternal()
  {
    if (m_Valid)
    {
#ifdef STORM_CHANGE_NOTIFIER
      auto prev_refl_info = m_ReflectionInfo;
#endif

      T * ptr = reinterpret_cast<T *>(m_Buffer);
      ptr->~T();

#ifdef STORM_CHANGE_NOTIFIER
      new (&m_ReflectionInfo) StormReflectionParentInfo(prev_refl_info);
#endif
    }

    m_Valid = false;
  }

  void Modified()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this));
#endif
  }

  bool m_Valid;
  alignas(alignof(T)) unsigned char m_Buffer[sizeof(T)];

public:
  STORM_REFL_IGNORE StormReflectionParentInfo & m_ReflectionInfo = ((T *)&m_Buffer[0])->m_ReflectionInfo;
};

