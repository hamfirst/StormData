#pragma once

#include "StormData.h"
#include "StormDataJsonUtil.h"

#include <StormRefl/StormReflMetaFuncs.h>

#ifdef STORM_CHANGE_NOTIFIER

class RBool;
class RString;
class RDeterministicFloatBase;

template <typename T> class RNumber;
template <typename T> class RDeterministicFloat;
template <typename T> class REnum;
template <typename T> class ROpaque;
template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;
template <class Base, class TypeDatabase, class TypeInfo, bool DefaultFirstNonBase = false> class RPolymorphicBase;

template <typename T, typename Enable = void>
struct SetParentInfoStruct
{
  static void Set(T & t, const StormReflectionParentInfo & info)
  {

  }

  static void SetParent(T & t, StormReflectionParentInfo * parent_info)
  {

  }

  static void ClearParentCallback(T & t)
  {

  }

  static void SetFlag(T & t, StormDataParentInfoFlags flags)
  {

  }

  static void ClearFlag(T & t, StormDataParentInfoFlags flags)
  {

  }

  static void MoveParentInfo(T & src, T & dst)
  {

  }
};

template <typename T>
struct SetBasicParentInfo
{
  static void Set(T & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
  }

  static void SetParent(T & value, StormReflectionParentInfo * parent_info)
  {
    value.m_ReflectionInfo.m_ParentInfo = parent_info;
  }

  static void SetCallback(T & value, StormDataNotifyCallback callback, void * user_ptr)
  {
    value.m_ReflectionInfo.m_Callback = callback;
    value.m_ReflectionInfo.m_CallbackUserPtr = user_ptr;
  }

  static void ClearCallback(T & value)
  {
    value.m_ReflectionInfo.m_Callback = nullptr;
    value.m_ReflectionInfo.m_CallbackUserPtr = nullptr;
  }

  static void ClearParentCallback(T & value)
  {
    ClearFlag(value, (StormDataParentInfoFlags)~(uint32_t)StormDataParentInfoFlags::kParentHasCallback);
  }

  static void SetFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags |= (uint32_t)flags;
  }

  static void ClearFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags &= (uint32_t)flags;
  }
  
  static void MoveParentInfo(T & src, T & dst)
  {
    dst.m_ReflectionInfo = src.m_ReflectionInfo;
    src.m_ReflectionInfo = {};
  }
};

template <>
struct SetParentInfoStruct<RBool> : public SetBasicParentInfo<RBool>
{

};

template <typename NumericType>
struct SetParentInfoStruct<RNumber<NumericType>> : public SetBasicParentInfo<RNumber<NumericType>>
{

};

template <>
struct SetParentInfoStruct<RDeterministicFloatBase> : public SetBasicParentInfo<RDeterministicFloatBase>
{

};

template <typename ParsedType>
struct SetParentInfoStruct<RDeterministicFloat<ParsedType>> : public SetBasicParentInfo<RDeterministicFloat<ParsedType>>
{

};

template <>
struct SetParentInfoStruct<RString> : public SetBasicParentInfo<RString>
{

};

template <class EnumType>
struct SetParentInfoStruct<REnum<EnumType>> : public SetBasicParentInfo<REnum<EnumType>>
{

};

template <class T>
struct SetParentInfoStruct<ROpaque<T>> : public SetBasicParentInfo<ROpaque<T>>
{

};

template <typename T>
struct SetParentInfoStruct<T, typename std::enable_if_t<StormReflCheckReflectable<T>::value && StormDataCheckReflectable<T>::value>>
{
  static void Set(T & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &value.m_ReflectionInfo;
    new_info.m_Flags = (info.m_Callback != nullptr || (info.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ? 
                        (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    auto parent_setter = [&](auto f)
    {
      using member_type = typename decltype(f)::member_type;
      member_type & elem = f.Get();

      new_info.m_MemberName = f.GetName();
      SetParentInfo(elem, new_info);
    };

    StormReflVisitEach(value, parent_setter);
  }

  static void SetParent(T & value, StormReflectionParentInfo * parent_info)
  {
    value.m_ReflectionInfo.m_ParentInfo = parent_info;
  }

  static void SetCallback(T & value, StormDataNotifyCallback callback, void * user_ptr)
  {
    value.m_ReflectionInfo.m_Callback = callback;
    value.m_ReflectionInfo.m_CallbackUserPtr = user_ptr;

    auto visitor = [](auto f) { SetParentInfoStruct<typename decltype(f)::member_type>::SetFlag(f.Get(), StormDataParentInfoFlags::kParentHasCallback); };
    StormReflVisitEach(value, visitor);
  }

  static void ClearCallback(T & value)
  {
    value.m_ReflectionInfo.m_Callback = nullptr;
    value.m_ReflectionInfo.m_CallbackUserPtr = nullptr;

    auto visitor = [](auto f) { SetParentInfoStruct<typename decltype(f)::member_type>::ClearParentCallback(f.Get()); };
    StormReflVisitEach(value, visitor);
  }

  static void ClearParentCallback(T & value)
  {
    ClearFlag(value, (StormDataParentInfoFlags)~(uint32_t)StormDataParentInfoFlags::kParentHasCallback);
    if (value.m_ReflectionInfo.m_Callback == nullptr)
    {
      auto visitor = [](auto f) { SetParentInfoStruct<typename decltype(f)::member_type>::ClearParentCallback(f.Get()); };
      StormReflVisitEach(value, visitor);
    }
  }

  static void SetFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags |= (uint32_t)flags;
    auto visitor = [&](auto f) { SetParentInfoStruct<typename decltype(f)::member_type>::SetFlag(f.Get(), flags); };
    StormReflVisitEach(value, visitor);
  }

  static void ClearFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags &= (uint32_t)flags;
    auto visitor = [&](auto f) { SetParentInfoStruct<typename decltype(f)::member_type>::ClearFlag(f.Get(), flags); };
    StormReflVisitEach(value, visitor);
  }

  static void MoveParentInfo(T & src, T & dst)
  {
    dst.m_ReflectionInfo = src.m_ReflectionInfo;
    src.m_ReflectionInfo = {};

    auto visitor = [](auto src, auto dst) { SetParentInfoStruct<typename decltype(src)::member_type>::MoveParentInfo(src.Get(), dst.Get()); };
    StormReflVisitEach(src, dst, visitor);
  }
};

template <typename T, int i>
struct SetParentInfoStruct<T[i]>
{
  static void Set(T(&value)[i], const StormReflectionParentInfo & info)
  {
    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = info.m_ParentInfo;
    new_info.m_MemberName = info.m_MemberName;
    new_info.m_Flags = (info.m_Callback != nullptr || (info.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ? 
                        (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    for (int index = 0; index < i; index++)
    {
      new_info.m_ParentIndex = index;
      SetParentInfo(value[index], new_info);
    }
  }

  static void SetParent(T(&value)[i], StormReflectionParentInfo * parent_info)
  {
    for (int index = 0; index < i; index++)
    {
      value[index].m_ReflectionInfo.m_ParentInfo = parent_info;
    }
  }

  static void ClearParentCallback(T(&value)[i])
  {
    for (int index = 0; index < i; index++)
    {
      SetParentInfoStruct<T>::ClearParentCallback(value[index]);
    }
  }

  static void SetFlag(T(&value)[i], StormDataParentInfoFlags flags)
  {
    for (int index = 0; index < i; index++)
    {
      SetParentInfoStruct<T>::SetFlag(value[index], flags);
    }
  }

  static void ClearFlag(T(&value)[i], StormDataParentInfoFlags flags)
  {
    for (int index = 0; index < i; index++)
    {
      SetParentInfoStruct<T>::ClearFlag(value[index], flags);
    }
  }

  static void MoveParentInfo(T(&src)[i], T(&dst)[i])
  {
    for (int index = 0; index < i; index++)
    {
      SetParentInfoStruct<T>::MoveParentInfo(src[index], dst[index]);
    }
  }
};

template <class T>
struct SetHashMapParentInfo
{
  static void Set(T & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &value.m_ReflectionInfo;
    new_info.m_Flags = (info.m_Callback != nullptr || (info.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ? 
                        (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    for (auto t : value)
    {
      new_info.m_ParentIndex = t.first;
      SetParentInfo(t.second, new_info);
    }
  }

  static void SetParent(T & value, StormReflectionParentInfo * parent_info)
  {
    value.m_ReflectionInfo.m_ParentInfo = parent_info;
  }

  static void SetCallback(T & value, StormDataNotifyCallback callback, void * user_ptr)
  {
    value.m_ReflectionInfo.m_Callback = callback;
    value.m_ReflectionInfo.m_CallbackUserPtr = user_ptr;

    for (auto t : value) { SetParentInfoStruct<typename T::ContainerType>::SetFlag(t.second, StormDataParentInfoFlags::kParentHasCallback); };
  }

  static void ClearCallback(T & value)
  {
    value.m_ReflectionInfo.m_Callback = nullptr;
    value.m_ReflectionInfo.m_CallbackUserPtr = nullptr;

    for (auto t : value) { SetParentInfoStruct<typename T::ContainerType>::ClearParentCallback(t.second); };
  }

  static void ClearParentCallback(T & value)
  {
    ClearFlag(value, (StormDataParentInfoFlags)~(uint32_t)StormDataParentInfoFlags::kParentHasCallback);
    if (value.m_ReflectionInfo.m_Callback == nullptr)
    {
      for (auto t : value) { SetParentInfoStruct<typename T::ContainerType>::ClearParentCallback(t.second); };
    }
  }

  static void SetFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags |= (uint32_t)flags;
    for (auto t : value) { SetParentInfoStruct<typename T::ContainerType>::SetFlag(t.second, flags); };
  }

  static void ClearFlag(T & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags &= (uint32_t)flags;
    for (auto t : value) { SetParentInfoStruct<typename T::ContainerType>::ClearFlag(t.second, flags); };
  }

  static void MoveParentInfo(T & src, T & dst)
  {
    dst.m_ReflectionInfo = src.m_ReflectionInfo;
    src.m_ReflectionInfo = {};

    for (auto src_itr = src.begin(), dst_itr = dst.begin(), end = src.end(); src_itr != end; ++src_itr)
    {
      SetParentInfoStruct<typename T::ContainerType>::MoveParentInfo(src_itr->second, dst_itr->second);
    }
  }
};

template <class T>
struct SetParentInfoStruct<RSparseList<T>> : public SetHashMapParentInfo<RSparseList<T>>
{
};

template <class T>
struct SetParentInfoStruct<RMergeList<T>> : public SetHashMapParentInfo<RMergeList<T>>
{

};

template <class K, class T>
struct SetParentInfoStruct<RMap<K, T>> : public SetHashMapParentInfo<RMap<K, T>>
{

};

template <class Base, class TypeDatabase, class TypeInfo, bool DefaultFirstNonBase>
struct SetParentInfoStruct<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
{
  static void Set(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
    value.SetParentInfo();
  }

  static void SetParent(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value, StormReflectionParentInfo * parent_info)
  {
    value.m_ReflectionInfo.m_ParentInfo = parent_info;
  }

  static void SetCallback(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value, StormDataNotifyCallback callback, void * user_ptr)
  {
    value.m_ReflectionInfo.m_Callback = callback;
    value.m_ReflectionInfo.m_CallbackUserPtr = user_ptr;

    if (value.GetTypeInfo())
    {
      value.GetTypeInfo()->SetParentInfoFlag(value.GetValue(), StormDataParentInfoFlags::kParentHasCallback);
    }
  }

  static void ClearCallback(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value)
  {
    value.m_ReflectionInfo.m_Callback = nullptr;
    value.m_ReflectionInfo.m_CallbackUserPtr = nullptr;

    if (value.GetTypeInfo())
    {
      value.GetTypeInfo()->ClearParentInfoCallback(value.GetValue());
    }
  }

  static void ClearParentCallback(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value)
  {
    ClearFlag(value, (StormDataParentInfoFlags)~(uint32_t)StormDataParentInfoFlags::kParentHasCallback);

    if (value.m_ReflectionInfo.m_Callback == nullptr && value.GetTypeInfo())
    {
      value.GetTypeInfo()->ClearParentInfoCallback(value.GetValue());
    }
  }

  static void SetFlag(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags |= (uint32_t)flags;

    if (value.GetTypeInfo())
    {
      value.GetTypeInfo()->SetParentInfoFlag(value.GetValue(), flags);
    }
  }

  static void ClearFlag(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & value, StormDataParentInfoFlags flags)
  {
    value.m_ReflectionInfo.m_Flags &= (uint32_t)flags;

    if (value.GetTypeInfo())
    {
      value.GetTypeInfo()->ClearParentInfoFlag(value.GetValue(), flags);
    }
  }

  static void MoveParentInfo(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & src, RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & dst)
  {
    dst.m_ReflectionInfo = src.m_ReflectionInfo;
    src.m_ReflectionInfo = {};

    if (dst.GetTypeInfo() != nullptr && src.GetTypeNameHash() == dst.GetTypeNameHash())
    {
      dst.GetTypeInfo()->MoveParentInfo(src.GetValue(), dst.GetValue());
    }
  }
};

template <typename T>
void SetParentInfo(T & t, const StormReflectionParentInfo & info)
{
  SetParentInfoStruct<T>::Set(t, info);
}

template <typename T>
void SetNotifyCallback(T & t, StormDataNotifyCallback callback, void * user_ptr)
{
  SetParentInfoStruct<T>::SetCallback(t, callback, user_ptr);
}

template <typename T>
void ClearNotifyCallback(T & t)
{
  SetParentInfoStruct<T>::ClearCallback(t);
}

template <typename T>
void MoveParentInfo(T & src, T & dst)
{
  SetParentInfoStruct<T>::MoveParentInfo(src, dst);
}

template <typename T>
void SetParentInfoPointer(T & t, StormReflectionParentInfo * parent_info)
{
  SetParentInfoStruct<T>::SetParent(t, parent_info);
}

template <typename T>
void InitializeParentInfo(T & value)
{
  StormReflectionParentInfo default_info{};
  SetParentInfo(value, default_info);
}


#endif
