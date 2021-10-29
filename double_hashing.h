#ifndef DOUBLE_PROBING_H
#define DOUBLE_PROBING_H

#include <vector>
#include <algorithm>
#include <functional>
#include "hash_exceptions.h"
#include "prime.h"

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

  HashedObj Get(const HashedObj &in)
  {
    size_t current_pos = FindPos(in);
    if (!IsActive(current_pos))
    {
      throw KeyError();
    }
    return array_[current_pos].element_;
  }

  bool Insert(const HashedObj &x)
  {
    this->total_elements++;
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
    this->total_elements++;
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
    return collisions;
  }

  int TotalElements()
  {
    return this->total_elements;
  }

  int InternalSize()
  {
    return this->array_.size();
  }

	
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

  std::vector<HashEntry> array_;
  size_t current_size_;
  size_t total_elements = 0;
  size_t collisions = 0;

  bool IsActive(size_t current_pos) const
  {
    return array_[current_pos].info_ == ACTIVE;
  }

  size_t FindPos(const HashedObj &x)
  {

    size_t current_pos = InternalHash(x);
    int i = 0;
    while (array_[current_pos].info_ != EMPTY &&
           array_[current_pos].element_ != x)
    {
      collisions++;
      i++;
			current_pos = (current_pos + (i * r_value - (i%r_value))) % array_.size() ;
    }
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
};

#endif // DOUBLE_PROBING_H
