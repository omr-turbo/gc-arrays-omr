#if !defined(OMR_GC_ALLOCATOR_HPP_)
#define OMR_GC_ALLOCATOR_HPP_

#include "omrcfg.h"

#if !defined(OMR_GC_EXPERIMENTAL_ALLOCATOR)
#error "OMR/GC/Allocator.hpp: Requires OMR_GC_EXPERIMENTAL_ALLOCATOR"
#endif

#if defined(OMRGC_SEGREGATED_HEAP)
#error "OMR/GC/Allocator.hpp: not compatible with the segregated heap."
#endif

#if !defined(OMR_GC_NON_ZERO_TLH)
#error "OMR/GC/Allocator.hpp: OMR_GC_NON_ZERO_TLH required to implement uncleared object allocations."
#endif

// #include "LanguageThreadLocalHeap.hpp"
#include <AllocateDescription.hpp>
#include <MemorySubSpace.hpp>
#include <GCExtensionsBase.hpp>

#include <OMR/GC/AllocationResult.hpp>
#include <OMR/GC/System.hpp>
#include <OMR/GC/StackRoot.hpp>

class MM_MemorySpace;

namespace OMR
{
namespace GC
{

/// Every object size must be a multiple of the BASE_OBJECT_ALIGNMENT_FACTOR
constexpr std::size_t BASE_OBJECT_ALIGNMENT_FACTOR = 8;

/// Every object size must be at least this large.
constexpr std::size_t MINIMUM_OBJECT_SIZE = 32;

/// Round size up to a multiple of alignment.
/// @returns aligned size
constexpr std::size_t
align(std::size_t size, std::size_t alignment)
{
	return (size + alignment - 1) & ~(alignment - 1);
}

/// Slow path GC allocator. Avoid if possible.
AllocationResult
allocateRawSlow(Context& cx, const std::size_t size, const bool canCollect, const bool shouldZero) noexcept;

/// @group Raw Allocators
/// Allocate bytes without initializing. Will not pay allocator tax.
/// @{

/// Memory has been zeroed. Will trigger a GC on allocation failure.
inline void*
allocateRawFast(Context& cx, std::size_t size) noexcept
{
	return cx.heapCache().tryAllocateRaw(size);
}

/// Memory has not been zeroed. Will trigger a GC on allocation failure.
inline void*
allocateRawNonZeroFast(Context& cx, std::size_t size) noexcept
{
	return cx.nonZeroHeapCache().tryAllocateRaw(size);
}

/// @}

inline bool
allocationTaxRequired(Context& cx, const AllocationTax& tax)
{
	// TODO: There are other checks than can affect if the tax is payable or not.
	return tax.size != 0;
}

inline bool
allocationTaxRequired(Context& cx, const AllocationResult& result)
{
	return allocationTaxRequired(cx, result.tax);
}

/// Pay allocation tax. The collector requires that GC processing is performed as allocations are made.
/// The tax is part of the result of a slow-path allocation.
/// It's the mutator thread's responsibility to
inline void
payAllocationTax(Context& cx, const AllocationTax& tax) noexcept
{
	if (0 != tax.size) {
		Assert_MM_true(NULL != tax.subSpace);

		omrthread_t mutator = omrthread_self();
		uintptr_t category = omrthread_get_category(mutator);
		MM_GCExtensionsBase* extensions = cx.env()->getExtensions();

		if (extensions->trackMutatorThreadCategory) {
			/* this thread is doing concurrent GC work, charge time spent against the GC category */
			omrthread_set_category(mutator, J9THREAD_CATEGORY_SYSTEM_GC_THREAD,
			                       J9THREAD_TYPE_SET_GC);
		}

		/// HACK HACK HACK
		MM_AllocateDescription alloc(0, 0, true, true);
		alloc.setMemorySubSpace(tax.subSpace);
		alloc.setAllocationTaxSize(tax.size);

		tax.subSpace->payAllocationTax(cx.env(), &alloc);

		if (extensions->trackMutatorThreadCategory) {
			/* done doing concurrent GC work, restore the thread category */
			omrthread_set_category(mutator, category, J9THREAD_TYPE_SET_GC);
		}
	}
}

inline void
payAllocationTax(Context& cx, const AllocationResult& result)
{
	payAllocationTax(cx, result.tax);
}

/// Slowpath allocate. Only use this call if fast allocation fails.
template<typename T = void, typename Init>
inline T*
allocateSlow(Context& cx, std::size_t size, Init&& init, bool canCollect = true, bool shouldZero = true)
{
	AllocationResult result = allocateRawSlow(cx, size, canCollect, shouldZero);
	T* object = reinterpret_cast<T*>(result.allocation);
	if (object != nullptr) {
		init(object);
		if (allocationTaxRequired(cx, result)) {
			StackRoot<T> root(cx, object);
			MM_AtomicOperations::writeBarrier();
			payAllocationTax(cx, result.tax);
			object = root.get();
		}
	}
	return object;
}

/// @group Object-oriented fast-allocators
/// These allocators will allocate and initialize an object. It's important to understand that this is primitive
/// initialization, the bare minimum amount of work needed to walk the object. The Initializer may not allocate.
/// @{

/// Allocate an object from zero'd memory. If the allocation fails, this call will trigger a GC.
/// Init may not throw or allocate.
template<typename T = void, typename Init>
T*
allocate(Context& cx, std::size_t size, Init&& init) noexcept
{
	T* object = reinterpret_cast<T*>(allocateRawFast(cx, size));
	if (object != nullptr) {
		init(object);
	} else {
		object = allocateSlow<T>(cx, size, std::forward<Init>(init), true, true);
	}
	return object;
}

/// Allocate an object from zero-initialized memory. If the allocation fails, this call will _not_ trigger a GC.
template<typename T = void, typename Init>
T*
allocateNoCollect(Context& cx, std::size_t size, Init&& init) noexcept
{
	T* object = reinterpret_cast<T*>(allocateRawFast(cx, size));
	if (object != nullptr) {
		init(object);
	} else {
		object = allocateSlow<T>(cx, size, std::forward<Init>(init), false, true);
	}
	return object;
}

/// Allocate an object from memory that hasn't been zero-initialized.
template<typename T = void, typename Init>
T*
allocateNonZero(Context& cx, std::size_t size, Init&& init)
{
	T* object = reinterpret_cast<T*>(allocateRawNonZeroFast(cx, size));
	if (object != nullptr) {
		init(object);
	} else {
		object = allocateSlow<T>(cx, size, std::forward<Init>(init), true, false);
	}
	return object;
}

/// Allocate an object from memory that hasn't been zero-initialized.
template<typename T = void, typename Init>
T*
allocateNonZeroNoCollect(Context& cx, std::size_t size, Init&& init) noexcept
{
	T* object = reinterpret_cast<T*>(allocateRawNonZeroFast(cx, size));
	if (object != nullptr) {
		init(object);
	} else {
		object = allocateSlow<T>(cx, size, std::forward<Init>(init), false, false);
	}
	return object;
}

/// @}

} // namespace GC
} // namespace OMR

#endif // OMR_GC_ALLOCATOR_HPP_
