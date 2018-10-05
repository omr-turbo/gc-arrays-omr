#if !defined(OMR_GC_ALLOCATIONRESULT_HPP_)
#define OMR_GC_ALLOCATIONRESULT_HPP_

#include <omrcfg.h>

#if !defined(OMR_GC_EXPERIMENTAL_ALLOCATOR)
#error "Requires OMR_GC_EXPERIMENTAL_ALLOCATOR"
#endif

#include <cstdint>
#include <cstdlib>

class MM_MemorySubSpace;

namespace OMR {
namespace GC {

enum class AllocationTaxKind {
	LARGE_OBJECT_AREA,
	THREAD_LOCAL_HEAP_REFRESH,
	NORMAL,
	NONE
};

/// The collector will sometimes require that mutator threads
/// perform GC work as part of allocating.
/// The tax request is part of the result of the allocation.
struct AllocationTax {
public:

	/// The amount of bytes we are paying tax for. This is typically the size of the allocation we are paying tax on.
	std::size_t size = 0;

	/// The memory subspace we owe tax to. This is typically the subspace that satisified the allocation.
	MM_MemorySubSpace* subSpace = nullptr;

	/// The kind of allocation we're paying tax on.
	AllocationTaxKind kind = AllocationTaxKind::NONE;
};

/// Result of a slowpath allocation.
struct AllocationResult {
	void* allocation = nullptr;
	AllocationTax tax;
};

} // namespace GC
} // namespace OMR

#endif // OMR_GC_ALLOCATIONRESULT_HPP_
