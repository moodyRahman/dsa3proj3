/**
 * @file double_hashing.h
 * @author Moududur Rahman
 * @brief The header file containing inline implementation for HashTableDouble
 *        (double hashing implementation)
 * @version 0.1
 * @date 2021-11-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DOUBLE_PROBING_H
#define DOUBLE_PROBING_H

#include <vector>
#include <algorithm>
#include <functional>
#include <exception>

template <typename HashedObj>
class HashTableDouble
{
public:
  enum EntryType
  {
    ACTIVE,
    EMPTY,
    DELETED
  };

  explicit HashTableDouble(size_t size = 101) : array_(NextPrime(size)), r_value(87)
  {
    MakeEmpty();
  }

  HashTableDouble(int r_value_in, size_t size) : r_value{r_value_in}, array_(NextPrime(size))
  {
    MakeEmpty();
  }

  bool Contains(const HashedObj &x) const
  {
    return IsActive(FindPos(x));
  }

  void MakeEmpty()
  {
    current_size_ = 0;
    for (auto &entry : array_)
      entry.info_ = EMPTY;
  }

  int Get(const HashedObj &in)
  {
    size_t current_pos = FindPos(in);
    if (!IsActive(current_pos))
    {
      throw KeyError();
    }
    return temp_collisions_;
  }

  bool Insert(const HashedObj &x)
  {
    this->total_elements_++;
    // Insert x as active
    size_t current_pos = FindPos(x);
    if (IsActive(current_pos))
      return false;

    array_[current_pos].element_ = x;
    array_[current_pos].info_ = ACTIVE;

    // Rehash; see Section 5.5
    if (++current_size_ > array_.size() / 2)
    {
      Rehash();
    }
    return true;
  }

  bool Insert(HashedObj &&x)
  {
    // Insert x as active
    size_t current_pos = FindPos(x);
    if (IsActive(current_pos))
      return false;

    array_[current_pos] = std::move(x);
    array_[current_pos].info_ = ACTIVE;

    // Rehash; see Section 5.5
    if (++current_size_ > array_.size() / 2)
    {
      Rehash();
    }

    return true;
  }

  bool Remove(const HashedObj &x)
  {
    size_t current_pos = FindPos(x);
    if (!IsActive(current_pos))
      return false;

    array_[current_pos].info_ = DELETED;
    return true;
  }

  int TotalCollisions()
  {
    return collisions_;
  }

  int TotalElements()
  {
    return this->total_elements_;
  }

  int InternalSize()
  {
    return this->array_.size();
  }
  int temp_collisions_ = 0;

private:
  const int r_value;
  struct HashEntry
  {
    HashedObj element_;
    EntryType info_;

    HashEntry(const HashedObj &e = HashedObj{}, EntryType i = EMPTY)
        : element_{e}, info_{i} {}

    HashEntry(HashedObj &&e, EntryType i = EMPTY)
        : element_{std::move(e)}, info_{i} {}
  };

  struct KeyError : public std::exception
  {
    const char *what() const throw()
    {
      return "Key Not Found";
    }
  };

  std::vector<HashEntry> array_;
  size_t current_size_;
  size_t total_elements_ = 0;
  size_t collisions_ = 0;

  bool IsActive(size_t current_pos) const
  {
    return array_[current_pos].info_ == ACTIVE;
  }

  size_t FindPos(const HashedObj &x)
  {
    std::hash<HashedObj> hf;
    size_t current_pos = InternalHash(x);
    int i = 0;
    temp_collisions_ = 0;
    while (array_[current_pos].info_ != EMPTY &&
           array_[current_pos].element_ != x)
    {
      temp_collisions_++;
      i++;
      current_pos += (r_value - (hf(x) % r_value));
      current_pos = (current_pos % array_.size());
    }
    collisions_ += (temp_collisions_);
    temp_collisions_++;
    return current_pos;
  }

  void Rehash()
  {

    std::vector<HashEntry> old_array = array_;

    // Create new double-sized, empty table.
    array_.resize(NextPrime(2 * old_array.size()));
    for (auto &entry : array_)
      entry.info_ = EMPTY;

    // Copy table over.
    current_size_ = 0;
    for (auto &entry : old_array)
      if (entry.info_ == ACTIVE)
        Insert(std::move(entry.element_));
  }

  // returns the
  size_t InternalHash(const HashedObj &x) const
  {
    static std::hash<HashedObj> hf;
    return hf(x) % array_.size();
  }

  bool IsPrime(size_t n)
  {
    if (n == 2 || n == 3)
      return true;

    if (n == 1 || n % 2 == 0)
      return false;

    for (int i = 3; i * i <= n; i += 2)
      if (n % i == 0)
        return false;

    return true;
  }

  // Internal method to return a prime number at least as large as n.
  int NextPrime(size_t n)
  {
    if (n % 2 == 0)
      ++n;
    while (!IsPrime(n))
      n += 2;
    return n;
  }
};

#endif // DOUBLE_PROBING_H
