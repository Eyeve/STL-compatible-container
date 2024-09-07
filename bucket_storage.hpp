#ifndef BUCKET_STORAGE_H
#define BUCKET_STORAGE_H

#include <cstring>
#include <iterator>
#include <limits>
#include <utility>

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

  public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using iterator = AbstractIterator< false >;
	using const_iterator = AbstractIterator< true >;
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;

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

	/*
     * @brief
	 */
	friend void swap(BucketStorage< T > &first, BucketStorage< T > &second);

	iterator get_to_distance(iterator it, difference_type distance);

	size_type max_size();

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;
};

// ------------------------------------------
// START OF BUCKET INTERFACE
// ------------------------------------------

template< typename T >
class BucketStorage< T >::Bucket
{
	using size_type = size_t;
};

// ------------------------------------------
// START OF ITERATOR INTERFACE
// ------------------------------------------

template< class T >
template< bool isConst >
class BucketStorage< T >::AbstractIterator
{
  public:
	using value_type = T;
	using reference = typename std::conditional_t< isConst, T const &, T & >;
	using pointer = typename std::conditional_t< isConst, T const *, T * >;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	friend class BucketStorage;

	AbstractIterator();
	AbstractIterator(const AbstractIterator &other);
	~AbstractIterator();

	AbstractIterator &operator=(AbstractIterator&other);

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
};

// ------------------------------------------
// START OF BUCKET STORAGE IMPLEMENTATION
// ------------------------------------------

template<typename T>
BucketStorage<T>::BucketStorage()
{

}
template< typename T >
BucketStorage<T>::BucketStorage(const BucketStorage< T > &other)
{
}
template< typename T >
BucketStorage<T>::BucketStorage(BucketStorage< T > &&other) noexcept
{
}
template< typename T >
BucketStorage<T>::BucketStorage(BucketStorage::size_type block_capacity)
{
}
template< typename T >
BucketStorage<T>::~BucketStorage()
{
}
template< typename T >
BucketStorage<T>& BucketStorage< T >::operator=(const BucketStorage< T > &other)
{
	return *this;
}
template< typename T >
BucketStorage<T>& BucketStorage< T >::operator=(BucketStorage< T > &&other) noexcept
{
	return *this;
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::insert(T &&value)
{
	return BucketStorage::iterator();
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::insert(const T &value)
{
	return BucketStorage::iterator();
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::erase(BucketStorage::const_iterator it)
{
	return BucketStorage::iterator();
}
template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return false;
}
template< typename T >
BucketStorage<T>::size_type BucketStorage< T >::size() const noexcept
{
	return 0;
}
template< typename T >
BucketStorage<T>::size_type BucketStorage< T >::capacity() const noexcept
{
	return 0;
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
	return BucketStorage::iterator();
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::begin() const noexcept
{
	return BucketStorage::const_iterator();
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	return BucketStorage::const_iterator();
}
template< typename T >
BucketStorage<T>::iterator BucketStorage< T >::end() noexcept
{
	return BucketStorage::iterator();
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::end() const noexcept
{
	return BucketStorage::const_iterator();
}
template< typename T >
BucketStorage<T>::const_iterator BucketStorage< T >::cend() const noexcept
{
	return BucketStorage::const_iterator();
}

// ------------------------------------------
// START OF BUCKET IMPLEMENTATION
// ------------------------------------------

// ------------------------------------------
// START OF ITERATOR IMPLEMENTATION
// ------------------------------------------

template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::AbstractIterator()
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::AbstractIterator(const AbstractIterator &other)
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::~AbstractIterator()
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator=(AbstractIterator &other)
{
	return BucketStorage< T >::AbstractIterator();
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> BucketStorage< T >::AbstractIterator<isConst>::operator++(int)
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator++()
{
	return *this;
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> BucketStorage< T >::AbstractIterator<isConst>::operator--(int)
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst> &BucketStorage< T >::AbstractIterator<isConst>::operator--()
{
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator==(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator!=(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator<=(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator<(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator>=(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
bool BucketStorage< T >::AbstractIterator<isConst>::operator>(const AbstractIterator &other) const
{
	return false;
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::operator AbstractIterator< !isConst >() const
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::reference BucketStorage< T >::AbstractIterator<isConst>::operator*() const
{
}
template<class T>
template<bool isConst>
BucketStorage< T >::AbstractIterator<isConst>::pointer BucketStorage< T >::AbstractIterator<isConst>::operator->() const
{
}


#endif /* BUCKET_STORAGE_H */
