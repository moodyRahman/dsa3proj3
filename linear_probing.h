/**
 * @file linear_probing.h
 * @author Moududur Rahman
 * @brief The header file containing inline implementation for HashTableLinear
 *        (linear probing implementation)
 * @version 0.1
 * @date 2021-11-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef LINEAR_PROBING_H
#define LINEAR_PROBING_H

#include <vector>
#include <algorithm>
#include <functional>
#include <exception>


// Quadratic probing implementation.
template <typename HashedObj>
class HashTableLinear
{
public:
  enum EntryType
  {
    ACTIVE,
    EMPTY,
    DELETED
  };

  /**
   * @brief Construct a new Hash Table Double object
   * 
   * @param size 
   */
  explicit HashTableLinear(size_t size = 101) : array_(NextPrime(size))
  {
    MakeEmpty();
  }

  /**
   * @brief returns if x is in this HashTable object
   * 
   * @param x 
   * @return true 
   * @return false 
   */
  bool Contains(const HashedObj &x) const
  {
    return IsActive(FindPos(x));
  }

  /**
   * @brief deletes all entries in this HashTable
   * 
   */
  void MakeEmpty()
  {
    current_size_ = 0;
    for (auto &entry : array_)
      entry.info_ = EMPTY;
  }

  /**
   * @brief return how many probes it took to find in, otherwise throw an exception
   * 
   * @param in 
   * @return int 
   */
  int Get(const HashedObj &in)
  {
    size_t current_pos = FindPos(in);
    if (!IsActive(current_pos))
    {
      throw KeyError();
    }
    return temp_collisions_;
  }

  /**
   * @brief insert new element into HashTable
   * 
   * @param x 
   * @return true 
   * @return false 
   */
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

  /**
   * @brief r value insert function for rehashing
   * 
   * @param x 
   * @return true 
   * @return false 
   */
  bool Insert(HashedObj &&x)
  {
    // we don't increment total_elements_ because this version of insert gets called
    // on rehash only
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

  /**
   * @brief find and remove element x
   * 
   * @param x 
   * @return true 
   * @return false 
   */
  bool Remove(const HashedObj &x)
  {
    size_t current_pos = FindPos(x);
    if (!IsActive(current_pos))
      return false;

    array_[current_pos].info_ = DELETED;
    return true;
  }

  /**
   * @brief return total collisions that occured while probing
   * 
   * @return int 
   */
  int TotalCollisions()
  {
    return collisions_;
  }

  /**
   * @brief returns total elements in HashTable
   * 
   * @return int 
   */
  int TotalElements()
  {
    return this->total_elements_;
  }

  /**
   * @brief returns internal HashTable allocated size
   * 
   * @return int 
   */
  int InternalSize()
  {
    return this->array_.size();
  }
  int temp_collisions_ = 0;
private:
  /**
   * @brief metadata associates with the object being hashed
   * 
   */
  struct HashEntry
  {
    HashedObj element_;
    EntryType info_;

    HashEntry(const HashedObj &e = HashedObj{}, EntryType i = EMPTY)
        : element_{e}, info_{i} {}

    HashEntry(HashedObj &&e, EntryType i = EMPTY)
        : element_{std::move(e)}, info_{i} {}
  };

  /**
   * @brief error object if a key is not found
   * 
   */
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

  /**
   * @brief returns the active metadata from a HashMap slot
   * 
   * @param current_pos 
   * @return true 
   * @return false 
   */
  bool IsActive(size_t current_pos) const
  {
    return array_[current_pos].info_ == ACTIVE;
  }

  /**
   * @brief returns the position of the object x in the HashMap
   * 
   * @param x 
   * @return size_t 
   */
  size_t FindPos(const HashedObj &x)
  {
    size_t current_pos = InternalHash(x);
    temp_collisions_ = 1;
    while (array_[current_pos].info_ != EMPTY &&
           array_[current_pos].element_ != x)
    {
      temp_collisions_++;
      current_pos++;
      if (current_pos >= array_.size())
        current_pos -= array_.size();
    }
    collisions_ += (temp_collisions_ - 1);
    return current_pos;
  }

  /**
   * @brief when the internal vector reaches load capacity, resize the vector and 
   *        reinsert all elements
   * 
   */
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

  /**
   * @brief returns the hash of object x
   * 
   * @param x 
   * @return size_t 
   */
  size_t InternalHash(const HashedObj &x) const
  {
    static std::hash<HashedObj> hf;
    return hf(x) % array_.size();
  }

  /**
   * @brief returns if an int n is prime
   * 
   * @param n 
   * @return true 
   * @return false 
   */
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

#endif // LINEAR_PROBING_H
