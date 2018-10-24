
#include <OMR/GC/Handle.hpp>
#include <OMR/GC/PointerOperators.hpp>

#include <unordered_set>

namespace OMR {
namespace GC {

struct DynamicRootNode {
	void* ref;
};

template <typename T>
class DynamicRootHeap {
public:

	DynamicRootNode* newNode(void* ref = nullptr) {
		auto node = new DynamicRootNode(ref);
		_set.insert(node);
		return node;
	}

	deleteNode(DynamicRootNode* node) {
		_set.remove(node);
		delete node;
	}

	template <typename VisitorT>
	visit(VisitorT&& visitor) {
		for (auto nptr : _set) {
			visitor.edge(nullptr, RefSlotHandle(&node->ref));
		}
	}

private:
	std::unordered_set<DynamicRootNode*> _set;
};

template<typename T>
class DynamicRootBase
{
public:

	DynamicRootBase(DynamicRootNode* node, DynamicRootHeap* heap)
		: _node(node)
		, _heap(heap) {}

	T* get() const noexcept { return reinterpret_cast<T*>(_node->ref); }

protected:
	DynamicRootNode* _node;
	DynamicRootHeap* _heap;
};

/// This mixin adds Pointer-style operations to the StackRoot.
template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class DynamicRootPointerOperators : public StackRootBase<T>
{
public:
	using DynamicRootBase<T>::DynamicRootBase;

	T* operator->() const noexcept { return this->get(); }

	T& operator*() const noexcept { return *this->get(); }
};

/// This specialization removes pointer operations for void referents.
/// You cannot dereference a StackRoot<void>.
template<typename T>
class DynamicRootPointerOperators<T, false> : public DynamicRootBase<T>
{
public:
	using DynamicRootBase<T>::DynamicRootBase;
};

template <typename T>
class DynamicRoot : public DynamicRootPointerOperators<T> {
public:
	DynamicRoot(Context& cx, void* ref = nullptr)
		: DynamicRootPointerOperators(&cx.dynamicRootHeap(), cx.dynamicRootHeap().createNode(ref))
	{}

	DynamicRoot(const DynamicRoot& rhs)
		: DynamicRootPointerOperators(rhs.heap, rhs.heap->createNode(rhs.node->ref))
	{}

	DynamicRoot(DynamicRoot&& rhs)
		: DynamicRootPointerOperators(rhs.heap, rhs.node)
	{
		rhs.node = nullptr;
	}

	~DynamicRoot() noexcept {
		if (node) {
			_heap->deleteNode(node);
		}
	}
};

} // namespace GC
} // namespace OMR
