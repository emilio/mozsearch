#pragma clang diagnostic push

#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic pop

template <class T>
class RefPtr
{
private:
  T* mRawPtr;
};


template <class T>
class already_AddRefed
{
private:
  T* mRawPtr;
};

namespace mozilla {

#if defined(__GNUC__)
#define MOZ_ALIGNED_DECL(_type, _align) _type __attribute__((aligned(_align)))
#elif defined(_MSC_VER)
#define MOZ_ALIGNED_DECL(_type, _align) __declspec(align(_align)) _type
#else
#warning "We don't know how to align variables on this compiler."
#define MOZ_ALIGNED_DECL(_type, _align) _type
#endif

/*
 * AlignedElem<N> is a structure whose alignment is guaranteed to be at least N
 * bytes.
 *
 * We support 1, 2, 4, 8, and 16-byte alignment.
 */
template <size_t Align>
struct AlignedElem;

/*
 * We have to specialize this template because GCC doesn't like
 * __attribute__((aligned(foo))) where foo is a template parameter.
 */

template <>
struct AlignedElem<1> {
  MOZ_ALIGNED_DECL(uint8_t elem, 1);
};

template <>
struct AlignedElem<2> {
  MOZ_ALIGNED_DECL(uint8_t elem, 2);
};

template <>
struct AlignedElem<4> {
  MOZ_ALIGNED_DECL(uint8_t elem, 4);
};

template <>
struct AlignedElem<8> {
  MOZ_ALIGNED_DECL(uint8_t elem, 8);
};

template <>
struct AlignedElem<16> {
  MOZ_ALIGNED_DECL(uint8_t elem, 16);
};

template <class T>
class Span
{
private:
  T* mRawPtr;
};

struct fallible_t {};

}

namespace mozilla {
namespace gfx {

struct UnknownUnits;

template <class units>
class IntRegionTyped;

typedef IntRegionTyped<UnknownUnits> IntRegion;

}
}

typedef mozilla::gfx::IntRegion nsIntRegion;


namespace JS {
template<class T>
class Heap;
}

class nsRegion;
namespace mozilla {
namespace layers {
struct TileClient;
}
}

namespace mozilla {
struct SerializedStructuredCloneBuffer;
class SourceBufferTask;
}

namespace mozilla {
namespace dom {
namespace ipc {
class StructuredCloneData;
}
}
}

namespace mozilla {
namespace dom {
class ClonedMessageData;
class MessagePortMessage;
namespace indexedDB {
struct StructuredCloneReadInfo;
class SerializedStructuredCloneReadInfo;
class ObjectStoreCursorResponse;
}
}
}

class JSStructuredCloneData;
struct nsTArrayFallibleResult
{

               nsTArrayFallibleResult(bool aResult) : mResult(aResult) {}

               operator bool() { return mResult; }

private:
  bool mResult;
};

struct nsTArrayInfallibleResult
{
};






struct nsTArrayFallibleAllocatorBase
{
  typedef bool ResultType;
  typedef nsTArrayFallibleResult ResultTypeProxy;

  static ResultType Result(ResultTypeProxy aResult) { return aResult; }
  static bool Successful(ResultTypeProxy aResult) { return aResult; }
  static ResultTypeProxy SuccessResult() { return true; }
  static ResultTypeProxy FailureResult() { return false; }
  static ResultType ConvertBoolToResultType(bool aValue) { return aValue; }
};

struct nsTArrayInfallibleAllocatorBase
{
  typedef void ResultType;
  typedef nsTArrayInfallibleResult ResultTypeProxy;

  static ResultType Result(ResultTypeProxy aResult) {}
  static bool Successful(ResultTypeProxy) { return true; }
  static ResultTypeProxy SuccessResult() { return ResultTypeProxy(); }

  static ResultTypeProxy FailureResult()
  {
    do { *((volatile int*) __null) = 180; ::abort(); } while (0);
    return ResultTypeProxy();
  }

  static ResultType ConvertBoolToResultType(bool aValue)
  {
    if (!aValue) {
      do { *((volatile int*) __null) = 187; ::abort(); } while (0);
    }
  }
};

struct nsTArrayFallibleAllocator : nsTArrayFallibleAllocatorBase
{
  static void* Malloc(size_t aSize) { return malloc(aSize); }
  static void* Realloc(void* aPtr, size_t aSize)
  {
    return realloc(aPtr, aSize);
  }

  static void Free(void* aPtr) { free(aPtr); }
  static void SizeTooBig(size_t) {}
};




struct nsTArrayInfallibleAllocator : nsTArrayInfallibleAllocatorBase
{
  static void* Malloc(size_t aSize) { return malloc(aSize); }
  static void* Realloc(void* aPtr, size_t aSize)
  {
    return realloc(aPtr, aSize);
  }

  static void Free(void* aPtr) { free(aPtr); }
  static void SizeTooBig(size_t) { abort(); }
};
struct nsTArrayHeader
{
  static nsTArrayHeader sEmptyHdr;

  uint32_t mLength;
  uint32_t mCapacity : 31;
  uint32_t mIsAutoArray : 1;
};



template<class E, class Derived>
struct nsTArray_SafeElementAtHelper
{
  typedef E* elem_type;
  typedef size_t index_type;




  elem_type& SafeElementAt(index_type aIndex);
  const elem_type& SafeElementAt(index_type aIndex) const;
};

template<class E, class Derived>
struct nsTArray_SafeElementAtHelper<E*, Derived>
{
  typedef E* elem_type;

  typedef size_t index_type;

  elem_type SafeElementAt(index_type aIndex)
  {
    return static_cast<Derived*>(this)->SafeElementAt(aIndex, nullptr);
  }





  elem_type SafeElementAt(index_type aIndex) const
  {
    return static_cast<const Derived*>(this)->SafeElementAt(aIndex, nullptr);
  }
};



template<class E, class Derived>
struct nsTArray_SafeElementAtSmartPtrHelper
{
  typedef E* elem_type;
  typedef const E* const_elem_type;
  typedef size_t index_type;

  elem_type SafeElementAt(index_type aIndex)
  {
    return static_cast<Derived*>(this)->SafeElementAt(aIndex, nullptr);
  }


  elem_type SafeElementAt(index_type aIndex) const
  {
    return static_cast<const Derived*>(this)->SafeElementAt(aIndex, nullptr);
  }
};

template<class T> class nsCOMPtr;

template<class E, class Derived>
struct nsTArray_SafeElementAtHelper<nsCOMPtr<E>, Derived>
  : public nsTArray_SafeElementAtSmartPtrHelper<E, Derived>
{
};

template<class E, class Derived>
struct nsTArray_SafeElementAtHelper<RefPtr<E>, Derived>
  : public nsTArray_SafeElementAtSmartPtrHelper<E, Derived>
{
};

namespace mozilla {
template<class T> class OwningNonNull;
}

template<class E, class Derived>
struct nsTArray_SafeElementAtHelper<mozilla::OwningNonNull<E>, Derived>
{
  typedef E* elem_type;
  typedef const E* const_elem_type;
  typedef size_t index_type;

  elem_type SafeElementAt(index_type aIndex)
  {
    if (aIndex < static_cast<Derived*>(this)->Length()) {
      return static_cast<Derived*>(this)->ElementAt(aIndex);
    }
    return nullptr;
  }


  elem_type SafeElementAt(index_type aIndex) const
  {
    if (aIndex < static_cast<const Derived*>(this)->Length()) {
      return static_cast<const Derived*>(this)->ElementAt(aIndex);
    }
    return nullptr;
  }
};


void AnnotateMozCrashReason(const char*);


template<class Alloc, class Copy>
class nsTArray_base
{



  template<class Allocator, class Copier>
  friend class nsTArray_base;
  friend void Gecko_EnsureTArrayCapacity(void* aArray, size_t aCapacity,
                                         size_t aElemSize);
  friend void Gecko_ClearPODTArray(void* aTArray, size_t aElementSize,
                                   size_t aElementAlign);

protected:
  typedef nsTArrayHeader Header;

public:
  typedef size_t size_type;
  typedef size_t index_type;


  size_type Length() const { return mHdr->mLength; }


  bool IsEmpty() const { return Length() == 0; }




  size_type Capacity() const { return mHdr->mCapacity; }





protected:
  nsTArray_base();

  ~nsTArray_base();





  template<typename ActualAlloc>
  typename ActualAlloc::ResultTypeProxy EnsureCapacity(size_type aCapacity,
                                                       size_type aElemSize);





  void ShrinkCapacity(size_type aElemSize, size_t aElemAlign);
  template<typename ActualAlloc>
  void ShiftData(index_type aStart, size_type aOldLen, size_type aNewLen,
                 size_type aElemSize, size_t aElemAlign);

  bool IsAutoArray() const { return mHdr->mIsAutoArray; }


  Header* GetAutoArrayBuffer(size_t aElemAlign)
  {
    do { } while (0);
    return GetAutoArrayBufferUnsafe(aElemAlign);
  }
  const Header* GetAutoArrayBuffer(size_t aElemAlign) const
  {
    do { } while (0);
    return GetAutoArrayBufferUnsafe(aElemAlign);
  }



  Header* GetAutoArrayBufferUnsafe(size_t aElemAlign)
  {
    return const_cast<Header*>(static_cast<const nsTArray_base<Alloc, Copy>*>(
      this)->GetAutoArrayBufferUnsafe(aElemAlign));
  }
  const Header* GetAutoArrayBufferUnsafe(size_t aElemAlign) const;



  bool UsesAutoArrayBuffer() const;



  Header* mHdr;

  Header* Hdr() const { return mHdr; }
  Header** PtrToHdr() { return &mHdr; }
  static Header* EmptyHdr() { return &Header::sEmptyHdr; }
};





template<class E>
class nsTArrayElementTraits
{
public:

  static inline void Construct(E* aE)
  {
    new (static_cast<void*>(aE)) E;
  }

  template<class A>
  static inline void Construct(E* aE, A&& aArg)
  {
    typedef typename std::remove_cv<E>::Type E_NoCV;
    typedef typename std::remove_cv<A>::Type A_NoCV;
    static_assert(!std::is_same<E_NoCV*, A_NoCV>::value,
                  "For safety, we disallow constructing nsTArray<E> elements "
                  "from E* pointers. See bug 960591.");
    new (static_cast<void*>(aE)) E(std::forward<A>(aArg));
  }

  static inline void Destruct(E* aE) { aE->~E(); }
};


template<class A, class B>
class nsDefaultComparator
{
public:
  bool Equals(const A& aA, const B& aB) const { return aA == aB; }
  bool LessThan(const A& aA, const B& aB) const { return aA < aB; }
};

template<bool IsPod, bool IsSameType>
struct AssignRangeAlgorithm
{
  template<class Item, class ElemType, class IndexType, class SizeType>
  static void implementation(ElemType* aElements, IndexType aStart,
                             SizeType aCount, const Item* aValues)
  {
    ElemType* iter = aElements + aStart;
    ElemType* end = iter + aCount;
    for (; iter != end; ++iter, ++aValues) {
      nsTArrayElementTraits<ElemType>::Construct(iter, *aValues);
    }
  }
};

template<>
struct AssignRangeAlgorithm<true, true>
{
  template<class Item, class ElemType, class IndexType, class SizeType>
  static void implementation(ElemType* aElements, IndexType aStart,
                             SizeType aCount, const Item* aValues)
  {
    memcpy(aElements + aStart, aValues, aCount * sizeof(ElemType));
  }
};
struct nsTArray_CopyWithMemutils
{
  const static bool allowRealloc = true;

  static void MoveNonOverlappingRegionWithHeader(void* aDest, const void* aSrc,
                                                 size_t aCount, size_t aElemSize)
  {
    memcpy(aDest, aSrc, sizeof(nsTArrayHeader) + aCount * aElemSize);
  }

  static void MoveOverlappingRegion(void* aDest, void* aSrc, size_t aCount,
                                    size_t aElemSize)
  {
    memmove(aDest, aSrc, aCount * aElemSize);
  }

  static void MoveNonOverlappingRegion(void* aDest, void* aSrc, size_t aCount,
                                       size_t aElemSize)
  {
    memcpy(aDest, aSrc, aCount * aElemSize);
  }
};



template<class ElemType>
struct nsTArray_CopyWithConstructors
{
  typedef nsTArrayElementTraits<ElemType> traits;

  const static bool allowRealloc = false;

  static void MoveNonOverlappingRegionWithHeader(void* aDest, void* aSrc, size_t aCount,
                                                 size_t aElemSize)
  {
  }
  static void MoveOverlappingRegion(void* aDest, void* aSrc, size_t aCount,
                                    size_t aElemSize)
  {
  }

  static void MoveNonOverlappingRegion(void* aDest, void* aSrc, size_t aCount,
                                       size_t aElemSize)
  {
  }
};




template<class E>
struct nsTArray_CopyChooser
{
  typedef nsTArray_CopyWithMemutils Type;
};
template<class E>
struct nsTArray_CopyChooser<JS::Heap<E>>
{
  typedef nsTArray_CopyWithConstructors<JS::Heap<E>> Type;
};

template<> struct nsTArray_CopyChooser<nsRegion> { typedef nsTArray_CopyWithConstructors<nsRegion> Type; };
template<> struct nsTArray_CopyChooser<nsIntRegion> { typedef nsTArray_CopyWithConstructors<nsIntRegion> Type; };
template<> struct nsTArray_CopyChooser<mozilla::layers::TileClient> { typedef nsTArray_CopyWithConstructors<mozilla::layers::TileClient> Type; };
template<> struct nsTArray_CopyChooser<mozilla::SerializedStructuredCloneBuffer> { typedef nsTArray_CopyWithConstructors<mozilla::SerializedStructuredCloneBuffer> Type; };
template<> struct nsTArray_CopyChooser<mozilla::dom::ipc::StructuredCloneData> { typedef nsTArray_CopyWithConstructors<mozilla::dom::ipc::StructuredCloneData> Type; };
template<> struct nsTArray_CopyChooser<mozilla::dom::ClonedMessageData> { typedef nsTArray_CopyWithConstructors<mozilla::dom::ClonedMessageData> Type; };
template<> struct nsTArray_CopyChooser<mozilla::dom::indexedDB::StructuredCloneReadInfo> { typedef nsTArray_CopyWithConstructors<mozilla::dom::indexedDB::StructuredCloneReadInfo> Type; };;
template<> struct nsTArray_CopyChooser<mozilla::dom::indexedDB::ObjectStoreCursorResponse> { typedef nsTArray_CopyWithConstructors<mozilla::dom::indexedDB::ObjectStoreCursorResponse> Type; };
template<> struct nsTArray_CopyChooser<mozilla::dom::indexedDB::SerializedStructuredCloneReadInfo> { typedef nsTArray_CopyWithConstructors<mozilla::dom::indexedDB::SerializedStructuredCloneReadInfo> Type; };;
template<> struct nsTArray_CopyChooser<JSStructuredCloneData> { typedef nsTArray_CopyWithConstructors<JSStructuredCloneData> Type; };
template<> struct nsTArray_CopyChooser<mozilla::dom::MessagePortMessage> { typedef nsTArray_CopyWithConstructors<mozilla::dom::MessagePortMessage> Type; };
template<> struct nsTArray_CopyChooser<mozilla::SourceBufferTask> { typedef nsTArray_CopyWithConstructors<mozilla::SourceBufferTask> Type; };

template<typename T>
struct nsTArray_CopyChooser<std::function<T>>
{
  typedef nsTArray_CopyWithConstructors<std::function<T>> Type;
};


template<typename T>
class nsTArray;

template<typename T>
class FallibleTArray;

template<class E, class Derived>
struct nsTArray_TypedBase : public nsTArray_SafeElementAtHelper<E, Derived>
{
};
template<class E, class Derived>
struct nsTArray_TypedBase<JS::Heap<E>, Derived>
  : public nsTArray_SafeElementAtHelper<JS::Heap<E>, Derived>
{
  operator const nsTArray<E>&()
  {
    static_assert(sizeof(E) == sizeof(JS::Heap<E>),
                  "JS::Heap<E> must be binary compatible with E.");
    Derived* self = static_cast<Derived*>(this);
    return *reinterpret_cast<nsTArray<E> *>(self);
  }

  operator const FallibleTArray<E>&()
  {
    Derived* self = static_cast<Derived*>(this);
    return *reinterpret_cast<FallibleTArray<E> *>(self);
  }
};

template<class E, class Alloc>
class nsTArray_Impl
  : public nsTArray_base<Alloc, typename nsTArray_CopyChooser<E>::Type>
  , public nsTArray_TypedBase<E, nsTArray_Impl<E, Alloc>>
{
private:
  typedef nsTArrayFallibleAllocator FallibleAlloc;
  typedef nsTArrayInfallibleAllocator InfallibleAlloc;

public:
  typedef typename nsTArray_CopyChooser<E>::Type copy_type;
  typedef nsTArray_base<Alloc, copy_type> base_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::index_type index_type;
  typedef E elem_type;
  typedef nsTArray_Impl<E, Alloc> self_type;
  typedef nsTArrayElementTraits<E> elem_traits;
  typedef nsTArray_SafeElementAtHelper<E, self_type> safeelementat_helper_type;

  using safeelementat_helper_type::SafeElementAt;
  using base_type::EmptyHdr;



  static const index_type NoIndex = index_type(-1);

  using base_type::Length;





  ~nsTArray_Impl() { }





  nsTArray_Impl() {}


  explicit nsTArray_Impl(size_type aCapacity) { SetCapacity(aCapacity); }



  template<typename Allocator>
  explicit nsTArray_Impl(nsTArray_Impl<E, Allocator>&& aOther)
  {
    SwapElements(aOther);
  }
  explicit nsTArray_Impl(const self_type& aOther) { AppendElements(aOther); }

  explicit nsTArray_Impl(std::initializer_list<E> aIL) { AppendElements(aIL.begin(), aIL.size()); }


  template<typename Allocator>
  operator const nsTArray_Impl<E, Allocator>&() const
  {
    return *reinterpret_cast<const nsTArray_Impl<E, Allocator>*>(this);
  }

  operator const nsTArray<E>&() const
  {
    return *reinterpret_cast<const nsTArray<E>*>(this);
  }
  operator const FallibleTArray<E>&() const
  {
    return *reinterpret_cast<const FallibleTArray<E>*>(this);
  }

  bool operator!=(const self_type& aOther) const { return !operator==(aOther); }

  template<typename Allocator>
  self_type& operator=(const nsTArray_Impl<E, Allocator>& aOther)
  {
    ReplaceElementsAt(0, Length(), aOther.Elements(), aOther.Length());
    return *this;
  }

  template<typename Allocator>
  self_type& operator=(nsTArray_Impl<E, Allocator>&& aOther)
  {
    SwapElements(aOther);
    return *this;
  }




  elem_type* Elements() { return reinterpret_cast<elem_type*>(Hdr() + 1); }


  const elem_type* Elements() const
  {
    return reinterpret_cast<const elem_type*>(Hdr() + 1);
  }


protected:
  template<class Item, typename ActualAlloc = Alloc>
  elem_type* AppendElements(const Item* aArray, size_type aArrayLen);

  template<class Item, typename ActualAlloc = Alloc>
  elem_type* AppendElements(mozilla::Span<const Item> aSpan)
  {
    return AppendElements<Item, FallibleAlloc>(aSpan.Elements(),
                                               aSpan.Length());
  }

public:

  template<class Item>

  elem_type* AppendElements(const Item* aArray, size_type aArrayLen,
                            const mozilla::fallible_t&)
  {
    return AppendElements<Item, FallibleAlloc>(aArray, aArrayLen);
  }

  template<class Item>

  elem_type* AppendElements(mozilla::Span<const Item> aSpan,
                            const mozilla::fallible_t&)
  {
    return AppendElements<Item, FallibleAlloc>(aSpan.Elements(),
                                               aSpan.Length());
  }


protected:
  template<class Item, class Allocator, typename ActualAlloc = Alloc>
  elem_type* AppendElements(const nsTArray_Impl<Item, Allocator>& aArray)
  {
    return AppendElements<Item, ActualAlloc>(aArray.Elements(), aArray.Length());
  }
public:

  template<class Item, class Allocator>

  elem_type* AppendElements(const nsTArray_Impl<Item, Allocator>& aArray,
                            const mozilla::fallible_t&)
  {
    return AppendElements<Item, Allocator, FallibleAlloc>(aArray);
  }



protected:
  template<class Item, class Allocator, typename ActualAlloc = Alloc>
  elem_type* AppendElements(nsTArray_Impl<Item, Allocator>&& aArray);

public:

  template<class Item, class Allocator, typename ActualAlloc = Alloc>

  elem_type* AppendElements(nsTArray_Impl<Item, Allocator>&& aArray,
                            const mozilla::fallible_t&)
  {
    return AppendElements<Item, Allocator>(std::move(aArray));
  }

protected:
  template<typename ActualAlloc = Alloc>
  elem_type* AppendElements(size_type aCount) {
		return nullptr;
  }
public:


  elem_type* AppendElements(size_type aCount,
                            const mozilla::fallible_t&)
  {
    return AppendElements<FallibleAlloc>(aCount);
  }


  template<typename ActualAlloc = Alloc>
  elem_type* AppendElement()
  {
    return AppendElements<ActualAlloc>(1);
  }
public:
  using base_type::Hdr;
  using base_type::ShrinkCapacity;
};

template<typename E, class Alloc>
template<class Item, typename ActualAlloc>
auto
nsTArray_Impl<E, Alloc>::AppendElements(const Item* aArray, size_type aArrayLen) -> elem_type*
{
	return nullptr;
}

template<class E>
class nsTArray : public nsTArray_Impl<E, nsTArrayInfallibleAllocator>
{
};

class nsAttrName;
class nsAttrValue;

namespace mozilla {

namespace dom {
class Element;
}


struct ServoAttrSnapshot
{
  nsAttrName* mName;
  nsAttrValue* mValue;
};

class ServoElementSnapshot
{
  explicit ServoElementSnapshot();
  ~ServoElementSnapshot();

private:

  nsTArray<ServoAttrSnapshot> mAttrs;
};

}
