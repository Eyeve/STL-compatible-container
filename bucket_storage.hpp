#ifndef BUCKET_STORAGE_H
#define BUCKET_STORAGE_H

#include <cstring>
#include <iterator>
#include <limits>
#include <utility>
#include <cstdint>

#define DEFAULT_BLOCK_CAPACITY 64

// ------------------------------------------
// START OF BUCKET STORAGE INTERFACE
// ------------------------------------------

template<typename T>
class BucketStorage
{
	template<bool isConst>
	class AbstractIterator;

	template<bool isConst>
	friend class AbstractIterator;

	class Bucket;
	class GeneralBucketContent;

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
	Bucket* first;
	Bucket* last;
	Bucket* incomplete;

  public:
	/*
     * @brief Creates an empty container
	 */
	BucketStorage();

	/*
     * @brief Creates a copy of other container
	 */
	BucketStorage(const BucketStorage<T>& other);

	/*
     * @brief Moves other container
	 */
	BucketStorage(BucketStorage<T>&& other) noexcept;

	/*
     * @brief Creates a container and gives the block capacity equal to block_capacity
	 */
	explicit BucketStorage(size_type block_capacity);

	~BucketStorage();

	BucketStorage<T>& operator=(const BucketStorage<T>& other);
	BucketStorage<T>& operator=(BucketStorage<T>&& other) noexcept;

	/*
     * @brief Вставляет в контейнер объект (copy-constructor)
	 */
	iterator insert(T &&value);

	/*
     * @brief Вставляет в контейнер объект (move-constructor)
	 */
	iterator insert(const T &value);

	/*
     * @brief Удаляет объект, на который указывает итератор
     *
     * Удаляет объект, на который указывает итератор. Возвращает итератор на следующий
     * (активный) элемент (или end(), если таковых нет)
     *
	 */
	iterator erase(const_iterator it);

	/*
     * @brief Возвращает true, если контейнер пустой
	 */
	[[nodiscard]] bool empty() const noexcept;

	/*
     * @brief Возвращает число элементов, хранящихся в контейнере
	 */
	[[nodiscard]] size_type size() const noexcept;

	/*
     * @brief Возвращает число элементов, которые могут быть сохранены в контейнере без его расширения
	 */
	[[nodiscard]] size_type capacity() const noexcept;

	/*
     * @brief Изменяет ёмкость (capacity) контейнера до минимально необходимого
     *
     * Изменяет ёмкость (capacity) контейнера до минимально необходимого
     * для хранения всех текущих элементов
     *
     * Обратите внимание: этот метод может нарушать свойство стабильности
     * адреса в памяти хранящихся объектов.
     *
	 */
	void shrink_to_fit();

	/*
     * @brief Разрушает все объекты в контейнере и устанавливает размер size() в 0.
	 */
	void clear();

	/*
     * @brief Меняет местами содержимое текущего и переданного контейнеров.
	 */
	void swap(BucketStorage< T > &other) noexcept;

	friend void swap(BucketStorage< T > &first, BucketStorage< T > &second);

	iterator get_to_distance(iterator it, difference_type distance);

	size_type max_size();

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
};

// ------------------------------------------
// START OF GENERAL BUCKET CONTENT INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage<T>::GeneralBucketContent {
	const size_type blockCapacity;
	id_type idCounter;

  public:
	GeneralBucketContent(size_type blockCapacity = DEFAULT_BLOCK_CAPACITY);
	GeneralBucketContent(const BucketStorage<T>::GeneralBucketContent& other);

	size_type getBlockCapacity() const noexcept;
	id_type id() noexcept;
};

// ------------------------------------------
// START OF BUCKET INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage< T >::Bucket
{
	constexpr static size_type nullIndex = std::numeric_limits<size_type>::max();

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
	Bucket(GeneralBucketContent* generalContent, Bucket* last);
	Bucket(GeneralBucketContent* generalContent, Bucket* prev, Bucket* last);
	~Bucket();

	void setNext(Bucket* value) { next = value; }
	void setPrev(Bucket* value) { prev = value; }
	void setNextIncomplete(Bucket* value) { nextIncomplete = value; }
	void setPrevIncomplete(Bucket* value) { prevIncomplete = value; }
	Bucket* getNext() const { return next; }
	Bucket* getPrev() const { return prev; }
	Bucket* getNextIncomplete() const { return nextIncomplete; }
	Bucket* getPrevIncomplete() const { return prevIncomplete; }
	[[nodiscard]] size_type beginIndex() const;
	[[nodiscard]] size_type endIndex() const;
	[[nodiscard]] size_type nextIndex(size_type index) const;
	[[nodiscard]] size_type prevIndex(size_type index) const;
	[[nodiscard]] Bucket* nextBucket() const;
	[[nodiscard]] Bucket* prevBucket() const;
	[[nodiscard]] bool isEnd() const;
	reference getReference(size_type index);
	pointer getPointer(size_type index);
	const_reference getReference(size_type index) const;
	const_pointer getPointer(size_type index) const;
	iterator insert(const T& value);
	iterator insert(T&& value);
	[[nodiscard]] bool isFull() const;
	[[nodiscard]] bool isEmpty() const;

  private:
	void checkFreeSpace() const;
	size_type prepareInsert();
	void completeInsert(size_type index);
	template< typename U >
	[[nodiscard]] U* getMemory(size_type count) const;
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
	using reference = typename std::conditional_t< isConst, T const &, T & >;
	using pointer = typename std::conditional_t< isConst, T const *, T * >;
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

	AbstractIterator &operator=(const AbstractIterator& other);

	AbstractIterator operator++(int);
	AbstractIterator &operator++();

	AbstractIterator operator--(int);
	AbstractIterator &operator--();

	bool operator==(const AbstractIterator &other) const;
	bool operator!=(const AbstractIterator &other) const;
	bool operator<=(const AbstractIterator &other) const;
	bool operator<(const AbstractIterator &other) const;
	bool operator>=(const AbstractIterator &other) const;
	bool operator>(const AbstractIterator &other) const;
	operator AbstractIterator< !isConst >() const;

	reference operator*() const;
	pointer operator->() const;

  private:
	AbstractIterator(Bucket* bucket, size_type index);
};

// ------------------------------------------
// START OF GENERAL BUCKET CONTENT IMPLEMENTATION
// ------------------------------------------

template< typename T >
BucketStorage<T>::GeneralBucketContent::GeneralBucketContent(size_type blockCapacity):
	blockCapacity(blockCapacity),
	idCounter(0)
{
}
template< typename T >
BucketStorage<T>::GeneralBucketContent::GeneralBucketContent(const BucketStorage<T>::GeneralBucketContent& other):
	blockCapacity(other.blockCapacity),
	idCounter(other.idCounter)
{
}
template< typename T >
BucketStorage<T>::size_type BucketStorage<T>::GeneralBucketContent::getBlockCapacity() const noexcept {
	return blockCapacity;
}
template< typename T >
BucketStorage<T>::id_type BucketStorage<T>::GeneralBucketContent::id() noexcept {
	return idCounter++;
}

// ------------------------------------------
// START OF BUCKET STORAGE IMPLEMENTATION
// ------------------------------------------

template<typename T>
BucketStorage<T>::BucketStorage():
	generalContent(),
	dataSize(0),
	first(nullptr),
	last(new Bucket()),
	incomplete(last)
{
}
template< typename T >
BucketStorage<T>::BucketStorage(const BucketStorage< T > &other)
{
	// TODO: implementation
}
template< typename T >
BucketStorage<T>::BucketStorage(BucketStorage< T > &&other) noexcept:
	generalContent(other.generalContent),
	dataSize(other.dataSize),
	first(other.first),
	last(other.last),
	incomplete(other.last)
{
	// TODO: replace other data with valid data
}
template< typename T >
BucketStorage<T>::BucketStorage(size_type block_capacity):
	generalContent(block_capacity),
	dataSize(0),
	first(nullptr),
	last(new Bucket()),
	incomplete(last)
{
	if (block_capacity == 0) {
		throw 2; // TODO: replace
	}
}
template< typename T >
BucketStorage<T>::~BucketStorage()
{
}
template< typename T >
BucketStorage<T>& BucketStorage< T >::operator=(const BucketStorage< T > &other)
{
	if (this == &other) {
		return *this;
	}
	return *this;
}
template< typename T >
BucketStorage<T>& BucketStorage< T >::operator=(BucketStorage< T > &&other) noexcept
{
	return *this;
}
template< typename T >
void BucketStorage< T >::prepareInsert()
{
	if (incomplete->isEnd()) {
		if (last->getPrev() == nullptr)  {
			incomplete = new Bucket(&generalContent, last);
			first = incomplete;
		} else {
			incomplete = new Bucket(&generalContent, last->getPrev(), last);
		}
	}
}
template< typename T >
void BucketStorage< T >::completeInsert()
{
	if (incomplete->isFull()) {
		incomplete = incomplete->getNextIncomplete();
		incomplete->getPrevIncomplete()->setNextIncomplete(nullptr);
		incomplete->setPrevIncomplete(nullptr);
	}
	++dataSize;
}
template< typename T >
void BucketStorage< T >::undoInsert()
{
	if (incomplete->isEmpty()) {
		Bucket* temp = incomplete->getPrev();
		last->setPrevIncomplete(nullptr);
		last->setPrev(temp);
		if (temp != nullptr) {
			temp->setNext(last);
		}
		incomplete = last;
	}
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::insert(T &&value) try
{
	prepareInsert();
	auto temp =  incomplete->insert(std::move(value));
	completeInsert();
	return temp;
} catch (...) {
	undoInsert();
	throw 3; // TODO: replace
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::insert(const T &value) try
{
	prepareInsert();
	auto temp =  incomplete->insert(value);
	completeInsert();
	return temp;
} catch (...) {
	undoInsert();
	throw 3; // TODO: replace
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::erase(BucketStorage::const_iterator it)
{
	return BucketStorage::iterator();
}
template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return dataSize == 0;
}
template< typename T >
BucketStorage<T>::size_type BucketStorage< T >::size() const noexcept
{
	return dataSize;
}
template< typename T >
BucketStorage<T>::size_type BucketStorage< T >::capacity() const noexcept
{
	return generalContent.getBlockCapacity();
}
template< typename T >
void BucketStorage< T >::shrink_to_fit()
{
}
template< typename T >
void BucketStorage< T >::clear()
{
}
template< typename T >
void BucketStorage<T>::swap(BucketStorage< T > &other) noexcept
{
}
template < typename T >
void swap(BucketStorage<T>& first, BucketStorage<T>& second)
{
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::get_to_distance(BucketStorage::iterator it, BucketStorage::difference_type distance)
{
	return BucketStorage::iterator();
}
template< typename T >
BucketStorage<T>::size_type BucketStorage< T >::max_size()
{
	return 0;
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::begin() noexcept
{
	return iterator(first, first->beginIndex());
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::begin() const noexcept
{
	return const_iterator(first, first->beginIndex());
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	return const_iterator(first, first->beginIndex());
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::end() noexcept
{
	return iterator(last, 0);
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(last, 0);
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::cend() const noexcept
{
	return const_iterator(last, 0);
}

// ------------------------------------------
// START OF BUCKET IMPLEMENTATION
// ------------------------------------------

template< typename T >
template< typename U >
U* BucketStorage< T >::Bucket::getMemory(size_type count) const {
	void* memory = malloc(sizeof(U) * count);

	if (memory != nullptr) {
		return static_cast<U*>(memory);
	}
	// TODO: replace
	throw 1;
}

//try
//{
//	return static_cast<U*>(::operator new[](count * sizeof(U)));
//} catch (...) {
//	// TODO: implementation
//	throw 1;
//}
template< typename T >
BucketStorage< T >::Bucket::Bucket():
	generalContent(nullptr),
	id(std::numeric_limits<std::size_t>::max()),
	next(nullptr),
	prev(nullptr),
	nextIncomplete(nullptr),
	prevIncomplete(nullptr),
	data(nullptr),
	size(0),
	firstIndex(0),
	lastIndex(0),
	nextData(nullptr),
	prevData(nullptr),
	idData(nullptr)
{
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(GeneralBucketContent* generalContent, Bucket* last):
	generalContent(generalContent),
	id(generalContent->id()),
	next(last),
	prev(nullptr),
	nextIncomplete(last),
	prevIncomplete(nullptr),
	data(getMemory<T>(generalContent->getBlockCapacity())),
	size(0),
	firstIndex(0),
	lastIndex(0),
	nextData(getMemory<size_type>(generalContent->getBlockCapacity())),
	prevData(getMemory<size_type>(generalContent->getBlockCapacity())),
	idData(getMemory<id_type>(generalContent->getBlockCapacity()))
{
	last->prev = this;
	last->prevIncomplete = this;
}
template< typename T >
BucketStorage< T >::Bucket::Bucket(GeneralBucketContent* generalContent, Bucket* prev, Bucket* last):
	generalContent(generalContent),
	id(generalContent->id()),
	next(last),
	prev(prev),
	nextIncomplete(last),
	prevIncomplete(nullptr),
	data(getMemory<T>(generalContent->getBlockCapacity())),
	size(0),
	firstIndex(0),
	lastIndex(0),
	nextData(getMemory<size_type>(generalContent->getBlockCapacity())),
	prevData(getMemory<size_type>(generalContent->getBlockCapacity())),
	idData(getMemory<id_type>(generalContent->getBlockCapacity()))
{
	last->prev = this;
	last->prevIncomplete = this;
	prev->next = this;
}
template< typename T >
BucketStorage<T>::Bucket::~Bucket() {
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
BucketStorage< T >::size_type BucketStorage< T >::Bucket::beginIndex() const
{
	return firstIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::endIndex() const
{
	return lastIndex;
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::nextIndex(size_type index) const
{
	return nextData[index];
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::prevIndex(size_type index) const
{
	return prevData[index];
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::nextBucket() const
{
	return next;
}
template< typename T >
BucketStorage< T >::Bucket* BucketStorage< T >::Bucket::prevBucket() const
{
	return prev;
}
template< typename T >
bool BucketStorage< T >::Bucket::isEnd() const
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
void BucketStorage< T >::Bucket::checkFreeSpace() const
{
	if (isFull()) {
		throw 1; // TODO: replace
	}
}
template< typename T >
BucketStorage< T >::size_type BucketStorage< T >::Bucket::prepareInsert() {
	checkFreeSpace(); // TODO: comment out

	if (isEmpty()) {
		// Заполняем первый элемент
		firstIndex = 0;
		lastIndex = 0;
		return 0;
	}
	if (nextData[lastIndex] == firstIndex) {
		// Новый слой
		return size;
	}
	// Дозаполняем слои
	return nextData[lastIndex];
}
template< typename T >
void BucketStorage< T >::Bucket::completeInsert(size_type index)
{
	nextData[index] = firstIndex;
	prevData[index] = lastIndex;
	idData[index] = generalContent->id();
	nextData[lastIndex] = index;
	prevData[firstIndex] = index;
	lastIndex = index;
	++size;
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::Bucket::insert(const T& value)
{
	size_type index = prepareInsert();
	data[index] = value;
	completeInsert(index);
	return iterator(this, index);
}
template< typename T >
BucketStorage< T >::iterator BucketStorage< T >::Bucket::insert(T&& value)
{
	size_type index = prepareInsert();
	data[index] = std::move(value);
	completeInsert(index);
	return iterator(this, index);
}
template< typename T >
bool BucketStorage< T >::Bucket::isFull() const
{
	return size == generalContent->getBlockCapacity();
}
template< typename T >
bool BucketStorage< T >::Bucket::isEmpty() const
{
	return size == 0;
}

// ------------------------------------------
// START OF ITERATOR IMPLEMENTATION
// ------------------------------------------

template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::AbstractIterator(const AbstractIterator &other):
	bucket(other.bucket),
	index(other.index)
{
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator=(const AbstractIterator &other)
{
	if (*this == other) {
		return *this;
	}
	return *this;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> BucketStorage< T >::AbstractIterator<isConst>::operator++(int)
{
	auto temp = *this;
	++(*this);
	return temp;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator++()
{
	// TODO: handle out of bounds
	if (index != bucket->endIndex()) {
		index = bucket->nextIndex(index);
	} else {
		bucket = bucket->nextBucket();
		index = bucket->beginIndex();
	}
	return *this;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> BucketStorage< T >::AbstractIterator<isConst>::operator--(int)
{
	auto temp = *this;
	--(*this);
	return temp;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator--()
{
	// TODO: handle out of bounds
	if (index != bucket->beginIndex()) {
		index = bucket->prevIndex(index);
	} else {
		bucket = bucket->prevBucket();
		index = bucket->endIndex();
	}
	return *this;
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator==(const AbstractIterator &other) const
{
	return bucket == other.bucket && index == other.index;
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator!=(const AbstractIterator &other) const
{
	return !(*this == other);
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator<=(const AbstractIterator &other) const
{
	return false;
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator<(const AbstractIterator &other) const
{
	return false;
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator>=(const AbstractIterator &other) const
{
	return false;
}
template<typename T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator>(const AbstractIterator &other) const
{
	return false;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::operator AbstractIterator< !isConst >() const
{
	return *this;
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::reference BucketStorage< T >::AbstractIterator<isConst>::operator*() const
{
	return bucket->getReference(index);
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::pointer BucketStorage< T >::AbstractIterator<isConst>::operator->() const
{
	return bucket->getPointer(index);
}
template<typename T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::AbstractIterator(Bucket* bucket, size_type index):
	bucket(bucket),
	index(index)
{
}


#endif /* BUCKET_STORAGE_H */
