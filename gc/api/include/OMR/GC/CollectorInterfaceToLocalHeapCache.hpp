#if !defined(OMR_GC_CONTEXTHEAPCACHEHELPER_HPP_)
#define OMR_GC_CONTEXTHEAPCACHEHELPER_HPP_

#include <OMR/GC/LocalHeapCache.hpp>
#include <OMR/GC/System.hpp>

namespace OMR
{
namespace GC
{

/// This helper class lets the collector refresh and scan the
/// heeap caches embedded in the context.
class ContextHeapCacheHelper
{
public:
	LocalHeapCacheBase* getLanguageThreadLocalHeapStruct(MM_EnvironmentBase* env, bool zeroTLH)
	{
		if (!zeroTLH) {
			return &getContext(*env).nonZeroCache().data();
		}
		return &getContext(env).heapCache().data();
	}

	std::uint8_t** getPointerToHeapAlloc(MM_EnvironmentBase* env, bool zeroTLH)
	{
		if (!zeroTLH) {
			return &getContext(*env).nonZeroCache()._alloc;
		}
		return &getContext(env).heapCache()._alloc;
	}

	std::uint8_t** getPointerToHeapTop(MM_EnvironmentBase* env, bool zeroTLH)
	{
		if (!zeroTLH) {
			return &getContext(*env).nonZeroCache()._top;
		}
		return &getContext(env).heapCache()._top;
	}

	std::uint8_t* getPointerToTlhPrefetchFTA(MM_EnvironmentBase* env, bool zeroTLH)
	{
		if (!zeroTLH) {
			return &getContext(*env).nonZeroCache()._prefetchTLA;
		}
		return &getContext(env).heapCache()._prefetchTLA;
	}
};

} // namespace GC
} // namespace OMR

// Glue alias.
using MM_LanguageThreadLocalHeap = OMR::GC::ContextHeapCacheHelper;

#endif // OMR_GC_CONTEXTHEAPCACHEHELPER_HPP_
