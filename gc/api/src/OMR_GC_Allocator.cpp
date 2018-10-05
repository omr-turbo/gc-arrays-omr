#include <OMR/GC/Allocator.hpp>
#include <OMR/GC/System.hpp>

#include <ObjectAllocationInterface.hpp>


#include <iostream>

namespace OMR
{
namespace GC
{

AllocationResult
allocateRawSlow(Context& cx,
                const std::size_t size,
                const bool canCollect,
                const bool shouldZero) noexcept
{
	AllocationResult result;
	MM_EnvironmentBase* env = cx.env();

	// I don't know what this flag is disabling. Make sure it's true.
	Assert_MM_true(env->_objectAllocationInterface->cachedAllocationsEnabled(env));
	// Assert_MM_true(_allocateDescription.shouldCollectAndClimb() == isGCAllowed());
	Assert_MM_true(size % BASE_OBJECT_ALIGNMENT_FACTOR == 0);

	std::uintptr_t vmState = env->pushVMstate(OMRVMSTATE_GC_ALLOCATE_OBJECT);

	// HACK HACK HACK

	MM_AllocateDescription alloc(size, 0, canCollect, true);

	result.allocation = env->_objectAllocationInterface->allocateObject(
	        env, &alloc, env->getMemorySpace(), canCollect);

	result.tax.subSpace = alloc.getMemorySubSpace();

	if (result.allocation != nullptr) {

#if defined(OMR_VALGRIND_MEMCHECK)
		valgrindMempoolAlloc(env->getExtensions(), (uintptr_t)result.allocation, size);
#endif /* defined(OMR_VALGRIND_MEMCHECK) */

		if (shouldZero) {
			// TODO: If the allocation was carved out of zeroed memory, we don't need memset it again.
			OMRZeroMemory(result.allocation, size);
		}
	}

	env->popVMstate(vmState);
	return result;
}

} // namespace GC
} // namespace OMR
