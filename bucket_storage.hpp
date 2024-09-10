#ifndef BUCKET_STORAGE_H
#define BUCKET_STORAGE_H

#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <utility>

#define DEFAULT_BLOCK_CAPACITY 64

// ------------------------------------------
// START OF BUCKET STORAGE INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage
{
	template< bool isConst >
	class AbstractIterator;
	class Bucket;
	class GeneralBucketContent;

	template< bool isConst >
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

  private:
	GeneralBucketContent generalContent;
	size_type dataSize;
	size_type blocksCount;
	Bucket* first;
	Bucket* last;
	Bucket* incomplete;

  public:
	BucketStorage();									/* APPROVED */
	BucketStorage(const BucketStorage< T >& other);		/* APPROVED */
	BucketStorage(BucketStorage< T >&& other) noexcept; /* APPROVED */
	explicit BucketStorage(size_type block_capacity);
	~BucketStorage();

	BucketStorage< T >& operator=(const BucketStorage< T >& other);
	BucketStorage< T >& operator=(BucketStorage< T >&& other) noexcept;

	iterator insert(T&& value);
	iterator insert(const T& value);
	iterator erase(const_iterator it);

	[[nodiscard]] bool empty() const noexcept;		   /* APPROVED */
	[[nodiscard]] size_type size() const noexcept;	   /* APPROVED */
	[[nodiscard]] size_type capacity() const noexcept; /* APPROVED */
	[[nodiscard]] size_type max_size() const noexcept; /* APPROVED */

	void shrink_to_fit();
	void clear();
	void swap(BucketStorage< T >& other) noexcept;
	friend void swap(BucketStorage< T >& first, BucketStorage< T >& second);

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
};

// ------------------------------------------
// START OF GENERAL BUCKET CONTENT INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage< T >::GeneralBucketContent
{
	size_type blockCapacity;
	id_type idCounter;	  // TODO: swap

  public:
	explicit GeneralBucketContent(size_type blockCapacity = DEFAULT_BLOCK_CAPACITY);
	GeneralBucketContent(const BucketStorage< T >::GeneralBucketContent& other);

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
	Bucket();																  /* APPROVED */
	Bucket(GeneralBucketContent* generalContent, Bucket* last);				  /* APPROVED */
	Bucket(GeneralBucketContent* generalContent, Bucket* prev, Bucket* last); /* APPROVED */
	~Bucket();																  /* APPROVED */

	void setNext(Bucket* value) noexcept { next = value; }					   /* APPROVED */
	void setPrev(Bucket* value) noexcept { prev = value; }					   /* APPROVED */
	void setNextIncomplete(Bucket* value) noexcept { nextIncomplete = value; } /* APPROVED */
	void setPrevIncomplete(Bucket* value) noexcept { prevIncomplete = value; } /* APPROVED */

	Bucket* getNext() const noexcept { return next; }					  /* APPROVED */
	Bucket* getPrev() const noexcept { return prev; }					  /* APPROVED */
	Bucket* getNextIncomplete() const noexcept { return nextIncomplete; } /* APPROVED */
	Bucket* getPrevIncomplete() const noexcept { return prevIncomplete; } /* APPROVED */
	[[nodiscard]] size_type getSize() const noexcept { return size; }	  /* APPROVED */
	[[nodiscard]] size_type getFirstIndex() const noexcept;				  /* APPROVED */
	[[nodiscard]] size_type getLastIndex() const noexcept;				  /* APPROVED */
	[[nodiscard]] size_type getNextIndex(size_type index) const;		  /* APPROVED */
	[[nodiscard]] size_type getPrevIndex(size_type index) const;		  /* APPROVED */
	[[nodiscard]] Bucket* getNextBucket() const noexcept;				  /* APPROVED */
	[[nodiscard]] Bucket* getPrevBucket() const noexcept;				  /* APPROVED */

	[[nodiscard]] bool isBegin() const noexcept; /* APPROVED */
	[[nodiscard]] bool isEnd() const noexcept;	 /* APPROVED */
	[[nodiscard]] bool isFull() const noexcept;	 /* APPROVED */
	[[nodiscard]] bool isEmpty() const noexcept; /* APPROVED */

	reference getReference(size_type index);			 /* APPROVED */
	pointer getPointer(size_type index);				 /* APPROVED */
	const_reference getReference(size_type index) const; /* APPROVED */
	const_pointer getPointer(size_type index) const;	 /* APPROVED */

	iterator insert(const T& value); /* APPROVED */
	iterator insert(T&& value);		 /* APPROVED */
	void erase(size_type index);	 /* APPROVED */

  private:
	size_type prepareInsert();
	void completeInsert(size_type index);
	void checkFreeSpace() const;

	template< typename U >
	[[nodiscard]] U* getMemory(size_type count) const; /* APPROVED */
};

// ------------------------------------------
// START OF ITERATOR INTERFACE
// ------------------------------------------

template< typename T >
template< bool isConst >
class BucketStorage< T >::AbstractIterator
{
  public:
	using value_type = T;
	using reference = typename std::conditional_t< isConst, T const &, T& >;
	using pointer = typename std::conditional_t< isConst, T const *, T* >;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

  private:
	Bucket* bucket;
	size_type index;

  public:
	friend class BucketStorage;

	AbstractIterator() = default;
	AbstractIterator(const AbstractIterator& other);
	~AbstractIterator() = default;

	AbstractIterator& operator=(const AbstractIterator& other);
	AbstractIterator operator++(int);
	AbstractIterator& operator++();
	AbstractIterator operator--(int);
	AbstractIterator& operator--();
	bool operator==(const AbstractIterator& other) const;
	bool operator!=(const AbstractIterator& other) const;
	bool operator<=(const AbstractIterator& other) const;
	bool operator<(const AbstractIterator& other) const;
	bool operator>=(const AbstractIterator& other) const;
	bool operator>(const AbstractIterator& other) const;
	operator AbstractIterator< !isConst >() const;
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
BucketStorage< T >::GeneralBucketContent::GeneralBucketContent(const BucketStorage< T >::GeneralBucketContent& other) :
	blockCapacity(other.blockCapacity), idCounter(other.idCounter)
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
	generalContent(), dataSize(0), blocksCount(0), first(new Bucket()), last(first), incomplete(last)
{
}
template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage< T >& other) :
	generalContent(other.generalContent.getBlockCapacity()), dataSize(other.dataSize), blocksCount(other.blocksCount),
	first(nullptr), last(nullptr), incomplete(nullptr)
{
	// TODO: implementation (to complete)
	//	auto it = other.begin();
	//	first = new Bucket(*it.bucket);
	//	for (; it != other.end(); it.moveNextBucket())
	//	{
	//	}
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
	/* APPROVED */
	if (block_capacity == 0)
		throw 1;	// TODO: replace
}
template< typename T >
BucketStorage< T >::~BucketStorage()
{
	cleanup();
	resetPointers();
}
template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(const BucketStorage< T >& other)
{
	if (this != &other)
	{
		// TODO: clear all data
		generalContent = other.generalContent;
		dataSize = other.dataSize;
	}
	return *this;
}
template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(BucketStorage< T >&& other) noexcept
{
	cleanup();
	first = other.first;
	last = other.last;
	incomplete = other.incomplete;
	other.resetPointers();

	return *this;
}
template< typename T >
void BucketStorage< T >::prepareInsert()
{
	/* APPROVED */
	if (incomplete->isEnd())
	{
		if (empty())
		{
			incomplete = new Bucket(&generalContent, last);
			first = incomplete;
		}
		else
			incomplete = new Bucket(&generalContent, last->getPrev(), last);
		++blocksCount;
	}
}
template< typename T >
void BucketStorage< T >::completeInsert()
{
	/* APPROVED */
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
	/* APPROVED */
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
BucketStorage< T >::iterator BucketStorage< T >::insert(T&& value)
try
{
	/* APPROVED */
	prepareInsert();
	auto it = incomplete->insert(std::move(value));
	completeInsert();
	return it;
} catch (...)
{
	undoInsert();
	throw 1;	// TODO: replace
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::insert(const T& value)
try
{
	/* APPROVED */
	prepareInsert();
	auto it = incomplete->insert(value);
	completeInsert();
	return it;
} catch (...)
{
	undoInsert();
	throw 1;	// TODO: replace
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::erase(BucketStorage::const_iterator it)
{
	/* APPROVED */
	iterator temp = it;
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
		if (it.bucket->getNextIncomplete() != nullptr || it.bucket->getPrevIncomplete() != nullptr)
		{
			throw 1;	// TODO: delete
		}
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
	/* APPROVED */
	return dataSize == 0;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	/* APPROVED */
	return dataSize;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::capacity() const noexcept
{
	/* APPROVED */
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
template< typename T >
void BucketStorage< T >::swap(BucketStorage< T >& other) noexcept
{
}
template< typename T >
void swap(BucketStorage< T >& first, BucketStorage< T >& second)
{
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(BucketStorage::iterator it, BucketStorage::difference_type distance)
{
	iterator temp(it);
	while (distance > 0)
	{
		++temp;	   // TODO: optimization
		--distance;
	}
	while (distance < 0)
	{
		--temp;
		++distance;
	}
	return temp;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::max_size() const noexcept
{
	return 0;	 // TODO: implementation
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	/* APPROVED */
	return iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	/* APPROVED */
	return const_iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	/* APPROVED */
	return const_iterator(first, first->getFirstIndex());
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	/* APPROVED */
	return iterator(last, 0);
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	/* APPROVED */
	return const_iterator(last, 0);
}
template< typename T >
BucketStorage< T >::const_iterator BucketStorage< T >::cend() const noexcept
{
	/* APPROVED */
	return const_iterator(last, 0);
}
template< typename T >
void BucketStorage< T >::resetPointers()
{
	first = nullptr;
	last = nullptr;
	incomplete = nullptr;
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
U* BucketStorage< T >::Bucket::getMemory(size_type count) const
{
	void* memory = malloc(sizeof(U) * count);

	if (memory != nullptr)
		return static_cast< U* >(memory);
	throw 1;	// TODO: replace
}
template< typename T >
BucketStorage< T >::Bucket::Bucket() :
	generalContent(nullptr), id(std::numeric_limits< std::size_t >::max()), next(nullptr), prev(nullptr),
	nextIncomplete(nullptr), prevIncomplete(nullptr), data(nullptr), size(0), firstIndex(0), lastIndex(0),
	nextData(nullptr), prevData(nullptr), idData(nullptr)
{
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(GeneralBucketContent* generalContent, Bucket* last) :
	generalContent(generalContent), id(generalContent->id()), next(last), prev(nullptr), nextIncomplete(last),
	prevIncomplete(nullptr), data(getMemory< T >(generalContent->getBlockCapacity())), size(0), firstIndex(0),
	lastIndex(0), nextData(getMemory< size_type >(generalContent->getBlockCapacity())),
	prevData(getMemory< size_type >(generalContent->getBlockCapacity())),
	idData(getMemory< id_type >(generalContent->getBlockCapacity()))
{
	last->prev = this;
	last->prevIncomplete = this;
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(GeneralBucketContent* generalContent, Bucket* prev, Bucket* last) :
	generalContent(generalContent), id(generalContent->id()), next(last), prev(prev), nextIncomplete(last),
	prevIncomplete(nullptr), data(getMemory< T >(generalContent->getBlockCapacity())), size(0), firstIndex(0),
	lastIndex(0), nextData(getMemory< size_type >(generalContent->getBlockCapacity())),
	prevData(getMemory< size_type >(generalContent->getBlockCapacity())),
	idData(getMemory< id_type >(generalContent->getBlockCapacity()))
{
	last->prev = this;
	last->prevIncomplete = this;
	prev->next = this;
}
template< typename T >
BucketStorage< T >::Bucket::~Bucket()
{
	/* APPROVED */
	if (!isEmpty())
	{
		auto it = const_iterator(this, getFirstIndex());
		while (it.bucket == this)
		{
			data[it.index].~T();
			++it;
		}
	}

	free(data);
	free(nextData);
	free(prevData);
	free(idData);

	data = nullptr;
	nextData = nullptr;
	prevData = nullptr;
	idData = nullptr;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getFirstIndex() const noexcept
{
	/* APPROVED */
	return firstIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getLastIndex() const noexcept
{
	/* APPROVED */
	return lastIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getNextIndex(size_type index) const
{
	/* APPROVED */
	return nextData[index];
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::getPrevIndex(size_type index) const
{
	/* APPROVED */
	return prevData[index];
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getNextBucket() const noexcept
{
	/* APPROVED */
	return next;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::getPrevBucket() const noexcept
{
	/* APPROVED */
	return prev;
}
template< typename T >
bool BucketStorage< T >::Bucket::isBegin() const noexcept
{
	/* APPROVED */
	return prev == nullptr;
}
template< typename T >
bool BucketStorage< T >::Bucket::isEnd() const noexcept
{
	/* APPROVED */
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
void BucketStorage< T >::Bucket::checkFreeSpace() const
{
	if (isFull())
		throw 1;	// TODO: replace
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::prepareInsert()
{
	/* APPROVED */
	/*
	 * If isFull then undefined behavior
	 */
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
void BucketStorage< T >::Bucket::completeInsert(size_type index)
{
	/* APPROVED */
	if (size == index)
	{
		nextData[lastIndex] = index;
		prevData[firstIndex] = index;
		nextData[index] = firstIndex;
		prevData[index] = lastIndex;
	}
	idData[index] = generalContent->id();
	lastIndex = index;
	++size;
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::Bucket::insert(const T& value)
{
	/* APPROVED */
	size_type index = prepareInsert();
	data[index] = value;
	completeInsert(index);
	return iterator(this, index);
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::Bucket::insert(T&& value)
{
	/* APPROVED */
	size_type index = prepareInsert();
	data[index] = std::move(value);
	completeInsert(index);
	return iterator(this, index);
}
template< typename T >
void BucketStorage< T >::Bucket::erase(size_type index)
{
	/* APPROVED */
	data[index].~T();

	if (index == firstIndex)
		firstIndex = nextData[firstIndex];
	else if (index == lastIndex)
		lastIndex = prevData[lastIndex];
	else
	{
		size_type wasPrevIndex = prevData[index];
		size_type wasNextIndex = nextData[index];
		nextData[wasPrevIndex] = wasNextIndex;
		prevData[wasNextIndex] = wasPrevIndex;

		size_type myNextIndex = nextData[lastIndex];
		nextData[lastIndex] = index;
		prevData[myNextIndex] = index;	  // TODO: mb extract in method???
		nextData[index] = myNextIndex;
		prevData[index] = lastIndex;
	}
	--size;
}
template< typename T >
bool BucketStorage< T >::Bucket::isFull() const noexcept
{
	/* APPROVED */
	return size == generalContent->getBlockCapacity();
}
template< typename T >
bool BucketStorage< T >::Bucket::isEmpty() const noexcept
{
	/* APPROVED */
	return size == 0;
}

// ------------------------------------------
// START OF ITERATOR IMPLEMENTATION
// ------------------------------------------

template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >::AbstractIterator(const AbstractIterator& other) :
	bucket(other.bucket), index(other.index)
{
	/* APPROVED */
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst > BucketStorage< T >::AbstractIterator< isConst >::shiftNextBucket()
{
	/* APPROVED */
	/*
	 * If bucket is end then undefined behavior
	 */
	auto temp = *this;
	bucket = bucket->getNextBucket();
	index = bucket->getFirstIndex();
	return temp;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst > BucketStorage< T >::AbstractIterator< isConst >::shiftPrevBucket()
{
	/* APPROVED */
	/*
	 * If bucket is begin then undefined behavior
	 */
	auto temp = *this;
	bucket = bucket->getPrevBucket();
	index = bucket->getLastIndex();
	return temp;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >&
	BucketStorage< T >::AbstractIterator< isConst >::operator=(const AbstractIterator& other)
{
	/* APPROVED */
	if (this != &other)
	{
		bucket = other.bucket;
		index = other.index;
	}
	return *this;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst > BucketStorage< T >::AbstractIterator< isConst >::operator++(int)
{
	/* APPROVED */
	auto temp = *this;
	++(*this);
	return temp;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >& BucketStorage< T >::AbstractIterator< isConst >::operator++()
{
	/* APPROVED */
	/*
	 * If bucket is end and index is last then undefined behavior
	 */
	if (index != bucket->getLastIndex())
		index = bucket->getNextIndex(index);
	else
		shiftNextBucket();
	return *this;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst > BucketStorage< T >::AbstractIterator< isConst >::operator--(int)
{
	auto temp = *this;
	--(*this);
	return temp;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >& BucketStorage< T >::AbstractIterator< isConst >::operator--()
{
	/* APPROVED */
	/*
	 * If bucket is begin and index is first then undefined behavior
	 */
	if (index != bucket->getFirstIndex())
		index = bucket->getPrevIndex(index);
	else
		shiftPrevBucket();
	return *this;
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator==(const AbstractIterator& other) const
{
	/* APPROVED */
	return bucket == other.bucket && index == other.index;
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator!=(const AbstractIterator& other) const
{
	/* APPROVED */
	return !(*this == other);
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator<=(const AbstractIterator& other) const
{
	return false;
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator<(const AbstractIterator& other) const
{
	return false;
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator>=(const AbstractIterator& other) const
{
	return false;
}
template< typename T >
template< bool isConst >
bool BucketStorage< T >::AbstractIterator< isConst >::operator>(const AbstractIterator& other) const
{
	return false;
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >::operator AbstractIterator< !isConst >() const
{
	/* APPROVED */
	return AbstractIterator< !isConst >(bucket, index);
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >::reference BucketStorage< T >::AbstractIterator< isConst >::operator*() const
{
	/* APPROVED */
	return bucket->getReference(index);
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >::pointer BucketStorage< T >::AbstractIterator< isConst >::operator->() const
{
	/* APPROVED */
	return bucket->getPointer(index);
}
template< typename T >
template< bool isConst >
BucketStorage< T >::AbstractIterator< isConst >::AbstractIterator(Bucket* bucket, size_type index) :
	bucket(bucket), index(index)
{
	/* APPROVED */
}

#endif /* BUCKET_STORAGE_H */
