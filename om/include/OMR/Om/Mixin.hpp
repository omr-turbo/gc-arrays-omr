#if !defined OMR_OM_MIXIN_HPP_
#define OMR_OM_MIXIN_HPP_

namespace OMR {
namespace Om {

template<typename UnderlyingT>
class Underlayed {
protected:
	UnderlyingT* underlying() { return static_cast<UnderlyingT*>(this); }

	const T* underlying() const { return static_cast<const UnderlyingT*>(this); }
};

template<>
class Mixin<void> {};

template<>
class Underlayed {};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_MIXIN_HPP_
