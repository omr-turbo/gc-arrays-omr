#if !defined(OMR_BYTES_HPP_)
#define OMR_BYTES_HPP_

#include <omrcfg.h>

#include <cstdint>
#include <cstddef>

namespace OMR
{

constexpr std::size_t
bytes(std::size_t n)
{
	return n;
}

constexpr std::size_t
kibibytes(std::size_t n)
{
	return n * bytes(1024);
}

constexpr std::size_t
mebibytes(std::size_t n)
{
	return n * kibibytes(1024);
}

constexpr std::size_t
gibibytes(std::size_t n)
{
	return n * mebibytes(1024);
}

} // namespace OMR

#endif // OMR_BYTES_HPP_
