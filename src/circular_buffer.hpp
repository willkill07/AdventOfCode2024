#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(readability-identifier-naming)

/// \brief A basic circular buffer class
///
/// Key supported operations:
/// - insertion from front/back
/// - removal from front/back
/// - access of front/back
/// - traversal
/// - random access
/// - checked (safe) operations as well as traditional (unsafe)
///
/// \tparam T the underlying type
template <typename T, typename Allocator = std::allocator<T>> class CircularBuffer {

  template <template <class> class Wrapped> class Iterator {
    using container_type = Wrapped<CircularBuffer<T>>::type;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = Wrapped<T>::type;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using reference = value_type&;
    using const_reference = value_type const&;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;

    constexpr Iterator() noexcept = default;
    constexpr Iterator(Iterator const&) noexcept = default;
    constexpr Iterator(Iterator&&) noexcept = default;
    constexpr Iterator& operator=(Iterator const&) noexcept = default;
    constexpr Iterator& operator=(Iterator&&) noexcept = default;

    constexpr explicit Iterator(container_type& ref, std::size_t index) noexcept
        : buffer_{std::addressof(ref)}, size_{ref.size()}, index_{index} {
    }

    constexpr reference operator*() const {
      return buffer_->operator[](index_);
    }

    constexpr reference operator[](difference_type offset) const noexcept {
      return *(*this + offset);
    }

    constexpr Iterator& operator++() noexcept {
      ++index_;
      return *this;
    }

    constexpr Iterator operator++(int) noexcept {
      Iterator copy{*this};
      ++*this;
      return copy;
    }

    constexpr Iterator& operator--() noexcept {
      --index_;
      return *this;
    }

    constexpr Iterator operator--(int) noexcept {
      Iterator copy{*this};
      --*this;
      return copy;
    }

    constexpr Iterator& operator+=(difference_type n) noexcept {
      index_ = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(index_) + n);
      return *this;
    }

    constexpr Iterator& operator-=(difference_type n) noexcept {
      index_ = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(index_) - n);
      return *this;
    }

    constexpr Iterator operator+(difference_type n) const noexcept {
      return (Iterator{*this} += n);
    }

    constexpr Iterator operator-(difference_type n) const noexcept {
      return (Iterator{*this} -= n);
    }

    constexpr std::ptrdiff_t operator-(Iterator const& other) const noexcept {
      return static_cast<std::ptrdiff_t>(other.index_) - static_cast<std::ptrdiff_t>(index_);
    }

    friend constexpr Iterator operator+(difference_type n, Iterator const& self) noexcept {
      return self + n;
    }

    constexpr auto operator<=>(Iterator const& other) const noexcept {
      return index_ <=> other.index_;
    }

    constexpr bool operator==(Iterator const& other) const noexcept {
      return index_ == other.index_;
    }

  private:
    container_type* buffer_{nullptr};
    std::size_t size_{0};
    std::size_t index_{0};
  };

public:
  using value_type = T;
  using reference = T&;
  using pointer = T*;
  using const_reference = T const&;
  using const_pointer = T const*;
  using iterator = Iterator<std::type_identity>;
  using const_iterator = Iterator<std::add_const>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  explicit constexpr CircularBuffer(std::size_t capacity, Allocator const& allocator = Allocator()) noexcept(
      noexcept(allocator_.allocate(capacity)) and std::is_nothrow_copy_constructible_v<Allocator>)
      : allocator_{allocator}, data_{allocator_.allocate(capacity)}, capacity_{capacity} {
  }

  constexpr CircularBuffer(CircularBuffer const& other) noexcept(noexcept(allocator_.allocate(capacity_)) and
                                                                 std::is_nothrow_copy_constructible_v<T>)
      : data_{allocator_.allocate(other.capacity_)}, capacity_{other.capacity_}, size_{other.size_} {
    std::ranges::copy(other, begin());
  }

  constexpr CircularBuffer(CircularBuffer&& other) noexcept
      : data_{std::exchange(other.data_, nullptr)},
        capacity_{other.capacity_},
        head_{other.head_},
        size_{other.size_} {
  }

  constexpr CircularBuffer(CircularBuffer const& other, Allocator const& allocator) noexcept(
      noexcept(allocator_.allocate(other.capacity_)) and std::is_nothrow_copy_constructible_v<T>)
      : allocator_{allocator}, data_{allocator_.allocate(other.capacity_)}, capacity_{other.capacity_} {
    std::ranges::copy(other, std::back_inserter(*this));
  }

  constexpr CircularBuffer(CircularBuffer&& other, Allocator const& allocator) noexcept(
      noexcept(allocator_.allocate(other.capacity_)) and std::is_nothrow_move_constructible_v<T>)
      : allocator_{allocator}, data_{allocator_.allocate(other.capacity_)}, capacity_{other.capacity_} {
    std::ranges::move(other, std::back_inserter(*this));
  }

  constexpr ~CircularBuffer() noexcept(noexcept(allocator_.deallocate(data_, capacity_)) and
                                       std::is_nothrow_destructible_v<T>) {
    if (data_ != nullptr) {
      std::ranges::destroy(*this);
      allocator_.deallocate(data_, capacity_);
    }
  }

  constexpr CircularBuffer&
  operator=(CircularBuffer const& rhs) noexcept(noexcept(allocator_.allocate(capacity_)) and
                                                noexcept(allocator_.deallocate(data_, capacity_)) and
                                                std::is_nothrow_copy_constructible_v<T>) {
    if (&rhs != this) {
      if (capacity_ < rhs.capacity_) {
        allocator_.deallocate(data_, capacity_);
        data_ = allocator_.allocate(rhs.capacity_);
      }
      clear();
      capacity_ = rhs.capacity_;
      head_ = 0;
      std::ranges::copy(rhs, std::back_inserter(*this));
    }
    return *this;
  }

  constexpr CircularBuffer& operator=(CircularBuffer&& rhs) noexcept {
    std::swap(data_, rhs.data_);
    std::swap(capacity_, rhs.capacity_);
    std::swap(head_, rhs.head_);
    std::swap(size_, rhs.size_);
    return *this;
  }

  [[nodiscard]] constexpr iterator begin() noexcept {
    return iterator{*this, 0};
  }

  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator{*this, 0};
  }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return const_iterator{*this, 0};
  }

  [[nodiscard]] constexpr iterator end() noexcept {
    return iterator{*this, size_};
  }

  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return const_iterator{*this, size_};
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept {
    return const_iterator{*this, size_};
  }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator{end()};
  }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{end()};
  }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{cend()};
  }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept {
    return reverse_iterator{begin()};
  }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{begin()};
  }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{cbegin()};
  }

  [[nodiscard]] constexpr decltype(auto) operator[](this auto&& self, std::size_t index) noexcept {
    std::size_t offset{self.head_ + index};
    if (offset >= self.size_) {
      offset -= self.size_;
    }
    return self.data_[offset];
  }

  /// \brief push a value into the circular buffer
  ///
  /// \param[in] value the value to insert
  /// \return true IFF the value was inserted, false if there was not capacity
  [[nodiscard]] constexpr bool
  push_back_checked(T const& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    if (size_ == capacity_) {
      return false;
    }
    ++size_;
    std::construct_at(std::addressof(back()), value);
    return true;
  }

  /// \brief push a value into the circular buffer
  ///
  /// \param[in] value the value to insert
  /// \return true IFF the value was inserted, false if there was not capacity
  [[nodiscard]] constexpr bool
  push_back_checked(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) {
    if (size_ == capacity_) {
      return false;
    }
    ++size_;
    std::construct_at(std::addressof(back()), std::move(value));
    return true;
  }

  /// \brief push a value into the circular buffer
  ///
  /// \param[in] value the value to insert
  /// \return true IFF the value was inserted, false if there was not capacity
  [[nodiscard]] constexpr bool
  push_front_checked(T const& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    if (size_ == capacity_) {
      return false;
    }
    ++size_;
    head_ = (head_ ? head_ : capacity_) - 1;
    std::construct_at(std::addressof(front()), value);
    return true;
  }

  /// \brief push a value into the circular buffer
  ///
  /// \param[in] value the value to insert
  /// \return true IFF the value was inserted, false if there was not capacity
  [[nodiscard]] constexpr bool
  push_front_checked(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) {
    if (size_ == capacity_) {
      return false;
    }
    ++size_;
    head_ = (head_ ? head_ : capacity_) - 1;
    std::construct_at(std::addressof(front()), std::move(value));
    return true;
  }

  /// \brief gets a copy of the first element of the buffer
  ///
  /// \return nullopt if the buffer is empty, else a copy of first element
  [[nodiscard]] constexpr std::optional<T> front_checked() const noexcept(noexcept(std::optional{front()}))
    requires std::copyable<T>
  {
    if (empty()) {
      return std::nullopt;
    }
    return std::optional{front()};
  }

  /// \brief get a copy last element of the buffer
  ///
  /// \return nullopt if the buffer is empty, else a copy of last element
  [[nodiscard]] constexpr std::optional<T> back_checked() const noexcept(noexcept(std::optional{back()}))
    requires std::copyable<T>
  {
    if (empty()) {
      return std::nullopt;
    }
    return std::optional{back()};
  }

  /// \brief push a value into the circular buffer. overwrite the oldest value
  /// if full.
  ///
  /// \param[in] value the value to insert
  constexpr void push_back(T const& value) noexcept(noexcept(pop_front()) and
                                                    noexcept(push_back_checked(value)))
    requires std::copyable<T>
  {
    if (size_ == capacity_) {
      pop_front();
    }
    std::ignore = push_back_checked(value);
  }

  /// \brief push a value into the circular buffer. overwrite the oldest value
  /// if full.
  ///
  /// \param[in] value the value to insert
  constexpr void push_back(T&& value) noexcept(noexcept(pop_front()) and
                                               noexcept(push_back_checked(std::move(value))))
    requires std::movable<T>
  {
    if (size_ == capacity_) {
      pop_front();
    }
    std::ignore = push_back_checked(std::move(value));
  }

  /// \brief push a value into the circular buffer. overwrite the back if full.
  ///
  /// \param[in] value the value to insert
  constexpr void push_front(T const& value) noexcept(noexcept(pop_back()) and
                                                     noexcept(push_front_checked(value)))
    requires std::copyable<T>
  {
    if (size_ == capacity_) {
      pop_back();
    }
    std::ignore = push_front_checked(value);
  }

  /// \brief push a value into the circular buffer. overwrite the back if full.
  ///
  /// \param[in] value the value to insert
  constexpr void push_front(T&& value) noexcept(noexcept(pop_back()) and
                                                noexcept(push_front_checked(std::move(value))))
    requires std::movable<T>
  {
    if (size_ == capacity_) {
      pop_back();
    }
    std::ignore = push_front_checked(std::move(value));
  }

  /// \brief remove the first element of the buffer
  constexpr void pop_front() noexcept(std::is_nothrow_destructible_v<T>) {
    std::destroy_at(std::addressof(front()));
    if ((--size_ == 0) or (++head_ == capacity_)) {
      head_ = 0;
    }
  }

  /// \brief remove the last element of the buffer
  constexpr void pop_back() noexcept(std::is_nothrow_destructible_v<T>) {
    std::destroy_at(std::addressof(back()));
    --size_;
  }

  /// \brief clear the circular buffer
  ///
  constexpr void clear() noexcept(std::is_nothrow_destructible_v<T>) {
    std::ranges::destroy(*this);
    head_ = 0;
    size_ = 0;
  }

  /// \brief change the capacity. Will reallocate if new_capacity != capacity()
  ///
  /// \param[in] new_capacity the new capacity of the circular buffer
  constexpr void change_capacity(std::size_t new_capacity) noexcept(noexcept(CircularBuffer{new_capacity}) and
                                                                    noexcept(this->~CircularBuffer()) and
                                                                    std::is_nothrow_move_constructible_v<T>) {
    if (new_capacity == capacity_) {
      return;
    }
    CircularBuffer new_buf{new_capacity};
    std::ranges::move(*this, std::back_inserter(new_buf));
    swap(new_buf);
  }

  /// \brief query if the circular buffer is empty
  ///
  /// \return true IFF the buffer has no elements
  [[nodiscard]] constexpr bool empty() const noexcept {
    return size() == 0;
  }

  /// \brief query the number of elements in the circular buffer
  ///
  /// \return the number of elements
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

  /// \brief query the capacity of the circular buffer
  ///
  /// \return the capacity
  [[nodiscard]] constexpr std::size_t capacity() const noexcept {
    return capacity_;
  }

  /// \brief get a reference to the last element of the buffer
  [[nodiscard]] constexpr T& back() noexcept {
    std::size_t index{head_ + size_ - 1};
    if (index >= capacity_) {
      index -= capacity_;
    }
    return data_[index];
  }

  /// \brief get a reference to the last element of the buffer
  [[nodiscard]] constexpr T const& back() const noexcept {
    std::size_t index{head_ + size_ - 1};
    if (index >= capacity_) {
      index -= capacity_;
    }
    return data_[index];
  }

  /// \brief get a reference to the first element of the buffer
  [[nodiscard]] constexpr T& front() noexcept {
    return data_[head_];
  }

  /// \brief get a reference to the first element of the buffer
  [[nodiscard]] constexpr T const& front() const noexcept {
    return data_[head_];
  }

  constexpr void swap(CircularBuffer& other) noexcept {
    using std::swap;
    swap(data_, other.data_);
    swap(capacity_, other.capacity_);
    swap(head_, other.head_);
    swap(size_, other.size_);
  }

private:
  [[no_unique_address]] Allocator allocator_;
  T* data_;
  std::size_t capacity_;
  std::size_t head_{0};
  std::size_t size_{0};
};

// NOLINTEND(readability-identifier-naming)
