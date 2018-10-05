#if !defined(OMR_GC_LOCALHEAPCACHE_HPP_)
#define OMR_GC_LOCALHEAPCACHE_HPP_

#include "omrcfg.h"

#if !defined(OMR_GC_EXPERIMENTAL_CONTEXT)
#error "The experimental allocators are a part of the experimental context."
#endif

#if !defined(OMR_GC_THREAD_LOCAL_HEAP)
#error "The experimental allocators require OMR_GC_THREAD_LOCAL_HEAP"
#endif

#include <cstddef>
#include <cstdint>
#include <type_traits>

class CollectorInterfaceToLocalHeapCache;
class LocalHeapCacheOffsets;
class CollectorInterfaceToLocalHeapCache;
class MM_TLHAllocationSupport;
class MM_TLHAllocationInterface;

namespace OMR
{
namespace GC
{

constexpr std::size_t DEFAULT_HEAP_CACHE_SIZE = 64 * 64 * 64; // TODO: Pick a number

/// All fields of the heap cache.
/// The fields are poked and prodded at different points in the GC.
/// So we group them up here, just in case..
class LocalHeapCacheData
{
protected:
	friend class LocalHeapCacheOffsets;
	friend class CollectorInterfaceToLocalHeapCache;
	friend class MM_TLHAllocationSupport;
	friend class MM_TLHAllocationInterface;

	/// Pointer to beginning of allocation area. We allocate objects at this address.
	std::uint8_t* _heapAlloc = nullptr;

	/// Pointer "one past the end" of the cache.
	std::uint8_t* _heapTop = nullptr;

	/// ??? Honestly no idea.
	std::uint8_t* _prefetchTLA = nullptr;

	/// Base of the cached area.
	std::uint8_t* _base = nullptr;

	/// "real" pointer to the allocation area.
	/// THe collector will purposefully makes the cache look full to trigger slow path allocations.
	/// This is done by temporarily setting alloc to top.
	/// The slow path will check if the cache is "hidden" by examining the "real" alloc pointer.
	/// This lets us do the extra bookkeeping of the slow allocator, while still serving allocations from a TLH.
	std::uint8_t* _realAlloc = nullptr;

	/// Size of the TLH on refresh.
	std::size_t _refreshSize = DEFAULT_HEAP_CACHE_SIZE;
	void* _memorySubSpace = nullptr;
	void* _memoryPool = nullptr;
};

/// Thread-local heap cache. A pool of memory owned by a single thread.
class LocalHeapCacheBase : public LocalHeapCacheData
{
public:
	friend class LocalHeapCacheOffsets;

	LocalHeapCacheBase() : LocalHeapCacheData() {}

	/// Fast bump allocator for thread-owned region of memory. Will not GC.
	void* tryAllocateRaw(std::size_t nbytes) noexcept
	{
		assert(_heapAlloc <= _heapTop);
		assert(nbytes < PTRDIFF_MAX);
		void* result = nullptr;
		if (nbytes <= std::size_t(_heapTop - _heapAlloc)) {
			result = _heapAlloc;
			_heapAlloc += nbytes;
		}
		return result;
	}

	/// Convert this cache to it's data fields.
	LocalHeapCacheData& data() { return *this; }
};

static_assert(std::is_standard_layout<LocalHeapCacheBase>::value,
              "The LocalHeapCache structure must be a standard layout type.");

// struct LocalHeapCacheOffsets {
// 	static constexpr std::size_t ALLOC = offsetof(LocalHeapCacheBase, _heapAlloc);
// 	static constexpr std::size_T TOP = offsetof(LocalHeapCache);
// };

/// Local heap cache, where memory has *not* been zero-initialized.
class NonZeroLocalHeapCache : public LocalHeapCacheBase
{
private:
	friend class Context;

	NonZeroLocalHeapCache() : LocalHeapCacheBase() {}
};

/// LocalHeapCache, where memory has been zero-initialized.
class LocalHeapCache : public LocalHeapCacheBase
{
private:
	friend class Context;

	LocalHeapCache() : LocalHeapCacheBase() {}
};

/// A set of caches--one for zeroed memory, one for nonzero memory.
class LocalHeapCacheSet
{
public:
private:
	LocalHeapCache _cache;
	NonZeroLocalHeapCache _nonZeroCache;
};

} // namespace GC
} // namespace OMR

using LanguageLocalHeapStruct = OMR::GC::LocalHeapCacheData;

#endif // OMR_GC_LOCALHEAPCACHE_HPP_
