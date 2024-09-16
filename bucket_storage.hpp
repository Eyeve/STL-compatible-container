#ifndef BUCKET_STORAGE_H
#define BUCKET_STORAGE_H

#include <cstdint>
#include <iterator>
#include <limits>

// ------------------------------------------
// START OF BUCKET STORAGE INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage
{
	template< bool IsConst >
	class AbstractIterator;
	class Bucket;
	class GeneralBucketContent;

	template< bool IsConst >
	friend class AbstractIterator;

  public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using iterator = AbstractIterator< false >;
	using const_iterator = AbstractIterator< true >;
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;
	using id_type = uint64_t;

	static constexpr size_type DEFAULT_BLOCK_CAPACITY = 64;

  private:
	GeneralBucketContent generalContent;
	size_type dataSize;
	size_type blocksCount;
	Bucket* first;
	Bucket* last;
	Bucket* incomplete;

  public:
	BucketStorage();
	BucketStorage(const BucketStorage< T >& other);
	BucketStorage(BucketStorage< T >&& other) noexcept;
	explicit BucketStorage(size_type block_capacity);
	~BucketStorage() noexcept;

	BucketStorage< T >& operator=(const BucketStorage< T >& other);
	BucketStorage< T >& operator=(BucketStorage< T >&& other) noexcept;

	template< typename U >
	iterator insert(U&& value);
	iterator erase(const_iterator it);

	[[nodiscard]] bool empty() const noexcept;
	[[nodiscard]] size_type size() const noexcept;
	[[nodiscard]] size_type capacity() const noexcept;
	[[nodiscard]] size_type max_size() const noexcept;

	void shrink_to_fit();
	void clear();
	void swap(BucketStorage< T >& other) noexcept;
	friend void swap(BucketStorage< T >& first, BucketStorage< T >& second) noexcept;

	iterator get_to_distance(iterator it, difference_type distance);

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

  private:
	void prepareInsert();
	void completeInsert();
	void undoInsert();
	void resetPointers();
	void cleanup();
	void deepCopy(const BucketStorage< T >& other);
};

// ------------------------------------------
// START OF GENERAL BUCKET CONTENT INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage< T >::GeneralBucketContent
{
	size_type blockCapacity;
	id_type idCounter;

  public:
	explicit GeneralBucketContent(size_type blockCapacity = DEFAULT_BLOCK_CAPACITY);

	void setBlockCapacity(size_type value) noexcept;
	[[nodiscard]] size_type getBlockCapacity() const noexcept;
	[[nodiscard]] id_type id() noexcept;
};

// ------------------------------------------
// START OF BUCKET INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage< T >::Bucket
{
  public:
	using size_type = size_t;
	using reference = T&;
	using pointer = T*;
	using const_reference = const T&;
	using const_pointer = const T*;

  private:
	GeneralBucketContent* const generalContent;
	const id_type id;
	Bucket* next;
	Bucket* prev;
	Bucket* nextIncomplete;
	Bucket* prevIncomplete;
	T* data;
	size_type size;
	size_type firstIndex;
	size_type lastIndex;
	size_type* nextData;
	size_type* prevData;
	id_type* idData;

  public:
	Bucket();
	Bucket(GeneralBucketContent* generalContent, Bucket* next, Bucket* prev, Bucket* incomplete);
	Bucket(const Bucket& other, GeneralBucketContent* generalContent, Bucket* next, Bucket* prev);
	~Bucket();

	void setNext(Bucket* value) noexcept;
	void setPrev(Bucket* value) noexcept;
	void setNextIncomplete(Bucket* value) noexcept;
	void setPrevIncomplete(Bucket* value) noexcept;

	[[nodiscard]] Bucket* getNext() const noexcept;
	[[nodiscard]] Bucket* getPrev() const noexcept;
	[[nodiscard]] Bucket* getNextIncomplete() const noexcept;
	[[nodiscard]] Bucket* getPrevIncomplete() const noexcept;
	[[nodiscard]] id_type getId() const noexcept;
	[[nodiscard]] id_type getDataId(size_type index);
	[[nodiscard]] size_type getSize() const noexcept;
	[[nodiscard]] size_type getFirstIndex() const noexcept;
	[[nodiscard]] size_type getLastIndex() const noexcept;
	[[nodiscard]] size_type getNextIndex(size_type index) const noexcept;
	[[nodiscard]] size_type getPrevIndex(size_type index) const noexcept;

	[[nodiscard]] bool isBegin() const noexcept;
	[[nodiscard]] bool isEnd() const noexcept;
	[[nodiscard]] bool isFull() const noexcept;
	[[nodiscard]] bool isEmpty() const noexcept;

	reference getReference(size_type index);
	pointer getPointer(size_type index);
	const_reference getReference(size_type index) const;
	const_pointer getPointer(size_type index) const;

	template< typename U >
	iterator insert(U&& value);
	void erase(size_type index);

  private:
	size_type prepareInsert() noexcept;
	void completeInsert(size_type index) noexcept;

	void reconnectData(size_type nextIndex, size_type prevIndex, size_type nextValue, size_type prevValue) noexcept;

	template< typename U >
	[[nodiscard]] U* allocateMemory(size_type count) const;
};

// ------------------------------------------
// START OF ITERATOR INTERFACE
// ------------------------------------------

template< typename T >
template< bool IsConst >
class BucketStorage< T >::AbstractIterator
{
	friend class BucketStorage;
	using bucket_pointer = typename std::conditional_t< IsConst, const Bucket*, Bucket* >;

  public:
	using value_type = T;
	using reference = typename std::conditional_t< IsConst, T const &, T& >;
	using pointer = typename std::conditional_t< IsConst, T const *, T* >;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

  private:
	Bucket* bucket;
	size_type index;

  public:
	AbstractIterator() = default;
	AbstractIterator(const AbstractIterator& other);
	~AbstractIterator() = default;

	AbstractIterator& operator=(const AbstractIterator& other);
	AbstractIterator operator++(int);
	AbstractIterator& operator++();
	AbstractIterator operator--(int);
	AbstractIterator& operator--();
	bool operator==(const AbstractIterator< true >& other) const noexcept;
	bool operator!=(const AbstractIterator< true >& other) const noexcept;
	bool operator<=(const AbstractIterator< true >& other) const noexcept;
	bool operator<(const AbstractIterator< true >& other) const noexcept;
	bool operator>=(const AbstractIterator< true >& other) const noexcept;
	bool operator>(const AbstractIterator< true >& other) const noexcept;
	operator AbstractIterator< !IsConst >() const noexcept;
	reference operator*() const;
	pointer operator->() const;

	AbstractIterator shiftNextBucket();
	AbstractIterator shiftPrevBucket();

  private:
	AbstractIterator(Bucket* bucket, size_type index);
};

// ------------------------------------------
// START OF GENERAL BUCKET CONTENT IMPLEMENTATION
// ------------------------------------------

template< typename T >
BucketStorage< T >::GeneralBucketContent::GeneralBucketContent(size_type blockCapacity) :
	blockCapacity(blockCapacity), idCounter(0)
{
}
template< typename T >
void BucketStorage< T >::GeneralBucketContent::setBlockCapacity(size_type value) noexcept
{
	blockCapacity = value;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::GeneralBucketContent::getBlockCapacity() const noexcept
{
	return blockCapacity;
}
template< typename T >
BucketStorage< T >::id_type BucketStorage< T >::GeneralBucketContent::id() noexcept
{
	return idCounter++;
}

// ------------------------------------------
// START OF BUCKET STORAGE IMPLEMENTATION
// ------------------------------------------

template< typename T >
BucketStorage< T >::BucketStorage() :
	generalContent(GeneralBucketContent()), dataSize(0), blocksCount(0), first(new Bucket()), last(first), incomplete(last)
{
}
template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage< T >& other) :
	generalContent(other.generalContent), dataSize(other.dataSize), blocksCount(other.blocksCount), first(new Bucket()),
	last(first), incomplete(first)
{
	if (!other.empty())
		deepCopy(other);
}
template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage< T >&& other) noexcept :
	generalContent(other.generalContent), dataSize(other.dataSize), blocksCount(other.blocksCount), first(other.first),
	last(other.last), incomplete(other.incomplete)
{
	other.resetPointers();
}
template< typename T >
BucketStorage< T >::BucketStorage(size_type block_capacity) :
	generalContent(block_capacity), dataSize(0), blocksCount(0), first(new Bucket()), last(first), incomplete(first)
{
	if (block_capacity == 0)
	{
		generalContent.setBlockCapacity(DEFAULT_BLOCK_CAPACITY);
		throw std::invalid_argument("block_capacity cannot be zero");
	}
}
template< typename T >
BucketStorage< T >::~BucketStorage() noexcept
{
	cleanup();
	resetPointers();
}
template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(const BucketStorage< T >& other)
{
	if (this == &other)
		return *this;

	BucketStorage temp(other);
	(*this).swap(temp);
	return *this;
}
template< typename T >
void BucketStorage< T >::deepCopy(const BucketStorage< T >& other)
{
	for (auto it = --other.end(); it != other.begin(); it.shiftPrevBucket())
	{
		first = new Bucket(*it.bucket, &generalContent, first, nullptr);
		if (!first->isFull())
		{
			incomplete->setPrevIncomplete(first);
			first->setNextIncomplete(incomplete);
			incomplete = first;
		}
	}
}
template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(BucketStorage< T >&& other) noexcept
{
	if (this == &other)
		return *this;

	cleanup();
	resetPointers();
	other.swap(*this);
	return *this;
}
template< typename T >
void BucketStorage< T >::prepareInsert()
{
	if (incomplete->isEnd())
	{
		incomplete = new Bucket(&generalContent, last, last->getPrev(), last);
		if (empty())
			first = incomplete;
		++blocksCount;
	}
}
template< typename T >
void BucketStorage< T >::completeInsert()
{
	if (incomplete->isFull())
	{
		incomplete = incomplete->getNextIncomplete();
		incomplete->getPrevIncomplete()->setNextIncomplete(nullptr);
		incomplete->setPrevIncomplete(nullptr);
	}
	++dataSize;
}
template< typename T >
void BucketStorage< T >::undoInsert()
{
	if (incomplete->isEmpty())
	{
		Bucket* temp = incomplete->getPrev();
		last->setPrevIncomplete(nullptr);
		last->setPrev(temp);
		if (temp != nullptr)
			temp->setNext(last);
		delete incomplete;
		incomplete = last;
		--blocksCount;
	}
}
template< typename T >
template< typename U >
BucketStorage< T >::iterator BucketStorage< T >::insert(U&& value)
{
	try
	{
		prepareInsert();
		auto it = incomplete->insert(std::forward< U >(value));
		completeInsert();
		return it;
	} catch (...)
	{
		undoInsert();
		throw;
	}
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::erase(BucketStorage::const_iterator it)
{
	iterator temp(it);
	++temp;

	it.bucket->erase(it.index);
	if (it.bucket->isEmpty())
	{
		Bucket* next = it.bucket->getNext();
		Bucket* prev = it.bucket->getPrev();
		Bucket* nextIncomplete = it.bucket->getNextIncomplete();
		Bucket* prevIncomplete = it.bucket->getPrevIncomplete();

		next->setPrev(prev);
		if (prev != nullptr)
			prev->setNext(next);
		else
			first = next;

		if (nextIncomplete != nullptr)
			nextIncomplete->setPrevIncomplete(prevIncomplete);
		if (prevIncomplete != nullptr)
			prevIncomplete->setNextIncomplete(nextIncomplete);
		else if (incomplete == it.bucket)
			incomplete = nextIncomplete;

		delete it.bucket;
		--blocksCount;
	}
	else if (it.bucket->getSize() == generalContent.getBlockCapacity() - 1)
	{
		incomplete->setPrevIncomplete(it.bucket);
		it.bucket->setNextIncomplete(incomplete);
		incomplete = it.bucket;
	}
	--dataSize;
	return temp;
}
template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return dataSize == 0;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	return dataSize;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::capacity() const noexcept
{
	return generalContent.getBlockCapacity() * blocksCount;
}
template< typename T >
void BucketStorage< T >::shrink_to_fit()
{
	BucketStorage< T > temp(generalContent.getBlockCapacity());

	for (auto it = begin(); it != end(); ++it)
		temp.insert(std::move(*it));

	*this = std::move(temp);
}
template< typename T >
void BucketStorage< T >::clear()
{
	if (!empty())
	{
		auto it = begin();
		while (it != end())
			delete it.shiftNextBucket().bucket;

		first = last;
		incomplete = last;

		last->setPrev(nullptr);
		last->setPrevIncomplete(nullptr);

		dataSize = 0;
		blocksCount = 0;
	}
}
template< typename T >
void BucketStorage< T >::swap(BucketStorage< T >& other) noexcept
{
	using std::swap;

	swap(generalContent, other.generalContent);
	swap(dataSize, other.dataSize);
	swap(blocksCount, other.blocksCount);
	swap(first, other.first);
	swap(last, other.last);
	swap(incomplete, other.incomplete);
}
template< typename T >
void swap(BucketStorage< T >& first, BucketStorage< T >& second) noexcept
{
	first.swap(second);
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(BucketStorage::iterator it, BucketStorage::difference_type distance)
{
	while (distance > 0)
	{
		if (distance > it.bucket->getSize() && it.bucket->getFirstIndex() == it.index)
			distance -= it.shiftNextBucket().bucket->getSize();
		else
		{
			++it;
			--distance;
		}
	}
	while (distance < 0)
	{
		if (distance < -it.bucket->getSize() && it.bucket->getLastIndex() == it.index)
			distance += it.shiftPrevBucket().bucket->getSize();
		else
		{
			--it;
			++distance;
		}
	}
	return it;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::max_size() const noexcept
{
	return std::numeric_limits< size_type >::max() / sizeof(T);
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	return iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	return const_iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	return const_iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	return iterator(last, 0);
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(last, 0);
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::cend() const noexcept
{
	return const_iterator(last, 0);
}
template< typename T >
void BucketStorage< T >::resetPointers()
{
	first = nullptr;
	last = nullptr;
	incomplete = nullptr;
	dataSize = 0;
}
template< typename T >
void BucketStorage< T >::cleanup()
{
	clear();
	delete last;
}

// ------------------------------------------
// START OF BUCKET IMPLEMENTATION
// ------------------------------------------

template< typename T >
template< typename U >
U* BucketStorage< T >::Bucket::allocateMemory(size_type count) const
{
	return static_cast< U* >(::operator new(sizeof(U) * count));
}
template< typename T >
BucketStorage< T >::Bucket::Bucket() :
	generalContent(nullptr), id(std::numeric_limits< id_type >::max()), next(nullptr), prev(nullptr),
	nextIncomplete(nullptr), prevIncomplete(nullptr), data(nullptr), size(0), firstIndex(0), lastIndex(0),
	nextData(nullptr), prevData(nullptr), idData(nullptr)
{
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(GeneralBucketContent* generalContent, Bucket* next, Bucket* prev, Bucket* incomplete) :
	generalContent(generalContent), id(generalContent->id()), next(next), prev(prev), nextIncomplete(incomplete),
	prevIncomplete(nullptr), data(allocateMemory< T >(generalContent->getBlockCapacity())), size(0), firstIndex(0),
	lastIndex(0), nextData(allocateMemory< size_type >(generalContent->getBlockCapacity())),
	prevData(allocateMemory< size_type >(generalContent->getBlockCapacity())),
	idData(allocateMemory< id_type >(generalContent->getBlockCapacity()))
{
	if (next != nullptr)
		next->prev = this;
	if (prev != nullptr)
		prev->next = this;
	if (incomplete != nullptr)
		incomplete->prevIncomplete = this;
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(const Bucket& other, GeneralBucketContent* generalContent, Bucket* next, Bucket* prev) :
	generalContent(generalContent), id(other.id), next(next), prev(prev), nextIncomplete(nullptr), prevIncomplete(nullptr),
	data(allocateMemory< T >(generalContent->getBlockCapacity())), size(other.size), firstIndex(other.firstIndex),
	lastIndex(other.lastIndex), nextData(allocateMemory< size_type >(generalContent->getBlockCapacity())),
	prevData(allocateMemory< size_type >(generalContent->getBlockCapacity())),
	idData(allocateMemory< id_type >(generalContent->getBlockCapacity()))
{
	if (next != nullptr)
		next->prev = this;
	if (prev != nullptr)
		prev->next = this;

	for (auto it = const_iterator(const_cast< Bucket* >(&other), other.getFirstIndex()); it.bucket == &other; ++it)
	{
		new (&data[it.index]) T(*it);
		nextData[it.index] = other.nextData[it.index];
		prevData[it.index] = other.prevData[it.index];
		idData[it.index] = other.idData[it.index];
	}
}
template< typename T >
BucketStorage< T >::Bucket::~Bucket()
{
	if (!isEmpty())
	{
		auto it = const_iterator(this, getFirstIndex());
		while (it.bucket == this)
		{
			data[it.index].~T();
			++it;
		}
	}

	::operator delete(data);
	::operator delete(nextData);
	::operator delete(prevData);
	::operator delete(idData);

	data = nullptr;
	nextData = nullptr;
	prevData = nullptr;
	idData = nullptr;
}
template< typename T >
void BucketStorage< T >::Bucket::setNext(BucketStorage< T >::Bucket* value) noexcept
{
	next = value;
}
template< typename T >
void BucketStorage< T >::Bucket::setPrev(BucketStorage< T >::Bucket* value) noexcept
{
	prev = value;
}
template< typename T >
void BucketStorage< T >::Bucket::setNextIncomplete(BucketStorage< T >::Bucket* value) noexcept
{
	nextIncomplete = value;
}
template< typename T >
void BucketStorage< T >::Bucket::setPrevIncomplete(BucketStorage< T >::Bucket* value) noexcept
{
	prevIncomplete = value;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getNext() const noexcept
{
	return next;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getPrev() const noexcept
{
	return prev;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getNextIncomplete() const noexcept
{
	return nextIncomplete;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getPrevIncomplete() const noexcept
{
	return prevIncomplete;
}
template< typename T >
BucketStorage< T >::id_type BucketStorage< T >::Bucket::getId() const noexcept
{
	return id;
}
template< typename T >
BucketStorage< T >::id_type BucketStorage< T >::Bucket::getDataId(unsigned long long int index)
{
	return idData[index];
}
template< typename T >
unsigned long long int BucketStorage< T >::Bucket::getSize() const noexcept
{
	return size;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getFirstIndex() const noexcept
{
	return firstIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getLastIndex() const noexcept
{
	return lastIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getNextIndex(size_type index) const noexcept
{
	return nextData[index];
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getPrevIndex(size_type index) const noexcept
{
	return prevData[index];
}
template< typename T >
bool BucketStorage< T >::Bucket::isBegin() const noexcept
{
	return prev == nullptr;
}
template< typename T >
bool BucketStorage< T >::Bucket::isEnd() const noexcept
{
	return next == nullptr;
}
template< typename T >
BucketStorage< T >::Bucket::reference BucketStorage< T >::Bucket::getReference(size_type index)
{
	return data[index];
}
template< typename T >
BucketStorage< T >::Bucket::pointer BucketStorage< T >::Bucket::getPointer(size_type index)
{
	return &data[index];
}
template< typename T >
BucketStorage< T >::Bucket::const_reference BucketStorage< T >::Bucket::getReference(size_type index) const
{
	return data[index];
}
template< typename T >
BucketStorage< T >::Bucket::const_pointer BucketStorage< T >::Bucket::getPointer(size_type index) const
{
	return &data[index];
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::prepareInsert() noexcept
{
	if (isEmpty())
	{
		firstIndex = 0;
		lastIndex = 0;
		return 0;
	}
	if (nextData[lastIndex] == firstIndex)
		return size;
	return nextData[lastIndex];
}
template< typename T >
void BucketStorage< T >::Bucket::completeInsert(size_type index) noexcept
{
	if (size == index)
	{
		reconnectData(lastIndex, firstIndex, index, index);
		reconnectData(index, index, firstIndex, lastIndex);
	}
	idData[index] = generalContent->id();
	lastIndex = index;
	++size;
}
template< typename T >
void BucketStorage< T >::Bucket::reconnectData(size_type nextIndex, size_type prevIndex, size_type nextValue, size_type prevValue) noexcept
{
	nextData[nextIndex] = nextValue;
	prevData[prevIndex] = prevValue;
}
template< typename T >
template< typename U >
BucketStorage< T >::iterator BucketStorage< T >::Bucket::insert(U&& value)
{
	size_type index = prepareInsert();
	new (&data[index]) T(std::forward< U >(value));
	completeInsert(index);
	return iterator(this, index);
}
template< typename T >
void BucketStorage< T >::Bucket::erase(size_type index)
{
	data[index].~T();

	if (index == firstIndex)
		firstIndex = nextData[firstIndex];
	else if (index == lastIndex)
		lastIndex = prevData[lastIndex];
	else
	{
		size_type wasPrevIndex = prevData[index];
		size_type wasNextIndex = nextData[index];
		reconnectData(wasPrevIndex, wasNextIndex, wasNextIndex, wasPrevIndex);

		size_type myNextIndex = nextData[lastIndex];
		reconnectData(lastIndex, myNextIndex, index, index);
		reconnectData(index, index, myNextIndex, lastIndex);
	}
	--size;
}
template< typename T >
bool BucketStorage< T >::Bucket::isFull() const noexcept
{
	return size == generalContent->getBlockCapacity();
}
template< typename T >
bool BucketStorage< T >::Bucket::isEmpty() const noexcept
{
	return size == 0;
}

// ------------------------------------------
// START OF ITERATOR IMPLEMENTATION
// ------------------------------------------

template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >::AbstractIterator(const AbstractIterator& other) :
	bucket(other.bucket), index(other.index)
{
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst > BucketStorage< T >::AbstractIterator< IsConst >::shiftNextBucket()
{
	if (bucket->isEnd())
		return *this;

	auto temp = *this;
	bucket = bucket->getNext();
	index = bucket->getFirstIndex();
	return temp;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst > BucketStorage< T >::AbstractIterator< IsConst >::shiftPrevBucket()
{
	auto temp = *this;
	if (bucket->isBegin())
		index = bucket->getFirstIndex();
	else
	{
		bucket = bucket->getPrev();
		index = bucket->getLastIndex();
	}
	return temp;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >&
	BucketStorage< T >::AbstractIterator< IsConst >::operator=(const AbstractIterator& other)
{
	if (this != &other)
	{
		bucket = other.bucket;
		index = other.index;
	}
	return *this;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst > BucketStorage< T >::AbstractIterator< IsConst >::operator++(int)
{
	auto temp = *this;
	++(*this);
	return temp;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >& BucketStorage< T >::AbstractIterator< IsConst >::operator++()
{
	if (index != bucket->getLastIndex())
		index = bucket->getNextIndex(index);
	else
		shiftNextBucket();
	return *this;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst > BucketStorage< T >::AbstractIterator< IsConst >::operator--(int)
{
	auto temp = *this;
	--(*this);
	return temp;
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >& BucketStorage< T >::AbstractIterator< IsConst >::operator--()
{
	if (index != bucket->getFirstIndex())
		index = bucket->getPrevIndex(index);
	else
		shiftPrevBucket();
	return *this;
}
template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator==(const AbstractIterator< true >& other) const noexcept
{
	return bucket == other.bucket && index == other.index;
}

template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator!=(const AbstractIterator< true >& other) const noexcept
{
	return !(*this == other);
}
template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator<=(const AbstractIterator< true >& other) const noexcept
{
	return *this < other || *this == other;
}
template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator<(const AbstractIterator< true >& other) const noexcept
{
	return !(*this >= other);
}
template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator>=(const AbstractIterator< true >& other) const noexcept
{
	return *this > other || *this == other;
}
template< typename T >
template< bool IsConst >
bool BucketStorage< T >::AbstractIterator< IsConst >::operator>(const AbstractIterator< true >& other) const noexcept
{
	if (bucket->getId() == other.bucket->getId())
		return !bucket->isEnd() && bucket->getDataId(index) > other.bucket->getDataId(other.index);
	return bucket->getId() > other.bucket->getId();
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >::operator AbstractIterator< !IsConst >() const noexcept
{
	return AbstractIterator< !IsConst >(bucket, index);
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >::reference BucketStorage< T >::AbstractIterator< IsConst >::operator*() const
{
	return bucket->getReference(index);
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >::pointer BucketStorage< T >::AbstractIterator< IsConst >::operator->() const
{
	return bucket->getPointer(index);
}
template< typename T >
template< bool IsConst >
BucketStorage< T >::AbstractIterator< IsConst >::AbstractIterator(Bucket* bucket, size_type index) :
	bucket(bucket), index(index)
{
}

#endif /* BUCKET_STORAGE_H */
