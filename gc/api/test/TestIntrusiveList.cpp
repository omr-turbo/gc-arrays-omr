#include <OMR/IntrusiveList.hpp>
#include <gtest/gtest.h>

namespace OMR
{

struct LinkedInt;
struct LinkedIntTraits;

using LinkedIntListNode = IntrusiveListNode<LinkedInt>;

static_assert(std::is_standard_layout<LinkedIntListNode>::value,
              "IntrusiveListNode<T> guarantees that it is a standard layout type.");

/// Forms a linked list of integers.
struct LinkedInt
{
	int value;
	LinkedIntListNode node;
};

/// A custom traits class for pulling the node out of a linked int.
struct LinkedIntTraits
{
	static constexpr LinkedIntListNode& node(LinkedInt& i) noexcept { return i.node; }

	static constexpr const LinkedIntListNode& node(const LinkedInt& i) noexcept
	{
		return i.node;
	}
};

using LinkedIntList = IntrusiveList<LinkedInt, LinkedIntTraits>;

static_assert(std::is_standard_layout<LinkedIntList>::value,
              "IntrusiveList<T> guarantees that it is a standard layout type.");

using LinkedIntListIterator = LinkedIntList::Iterator;

namespace Test
{

TEST(TestIntrusiveList, empty)
{
	LinkedIntList list;
	EXPECT_TRUE(list.empty());
}

TEST(TestIntrusiveList, addOne)
{
	LinkedIntList list;
	LinkedInt a = {1};
	list.add(&a);
	EXPECT_FALSE(list.empty());
}

TEST(TestIntrusiveList, iterateOne)
{
	LinkedIntList list;

	LinkedInt a = {1};
	list.add(&a);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_NE(iter, list.end());
	EXPECT_EQ(iter->value, 1);

	++iter;
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, iterateTwo)
{
	LinkedIntList list;

	LinkedInt a = {1};
	list.add(&a);

	LinkedInt b = {2};
	list.add(&b);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_EQ(iter->value, 2);

	++iter;
	EXPECT_EQ(iter->value, 1);

	++iter;
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddThenRemoveOne)
{
	LinkedIntList list;
	EXPECT_TRUE(list.empty());

	LinkedInt a = {1};
	list.add(&a);
	EXPECT_FALSE(list.empty());

	list.remove(&a);
	EXPECT_TRUE(list.empty());

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddTwoThenRemoveOne)
{
	LinkedIntList list;
	LinkedInt a = {1};
	LinkedInt b = {2};

	list.add(&a);
	list.add(&b);
	list.remove(&a);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_NE(iter, list.end());
	EXPECT_EQ(iter->value, 2);
	++iter;
	EXPECT_EQ(iter, list.end());
}

} // namespace Test
} // namespace OMR
