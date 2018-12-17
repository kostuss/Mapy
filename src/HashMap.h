#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional>//dla funkcji haszujacej

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:

    class Node {

    public:
        value_type element;
        size_type hash;
        Node* next;
        Node* previous;
        Node(value_type element, size_type hash, Node* next= nullptr, Node* previous= nullptr):
        element(element), hash(hash), next(next), previous(previous)
        {}

    };
    Node** buffer;
    size_type map_size;
    //size_type biggest_hash;
   // size_type smallest_hash;
    //size_type max_hash;
    std::hash<key_type> GetNumberFromKey;
    static const size_type MAX_HASH= 20470;

    size_type get_hash_index(const key_type& key) const
    {
        return (GetNumberFromKey(key)) % MAX_HASH;
    }

    void clear()
    {
      if (buffer== nullptr) return;
      for (size_type i=0; i<MAX_HASH; i++)
      {
        Node* temporary = buffer[i];
        while(temporary!= nullptr)
        {
          Node* remove = temporary;
          temporary = temporary->next;
          delete remove;
        }
        buffer[i]=nullptr;
      }
      map_size = 0;
      //biggest_hash = 0; // nie podoba mi sie
      //smallest_hash = MAX_HASH-1 ;
    }

public:
  HashMap()
  {
      map_size=0;
      //biggest_hash=0;
      //smallest_hash = MAX_HASH-1; // wtf
     // max_hash=MAX_HASH;
      buffer = new Node*[MAX_HASH+1];  //we need one free space for end
      for(size_type i=0; i<MAX_HASH+1;i++)
        buffer[i]=nullptr;
  }

  ~HashMap()
  {
      clear();
      delete[] buffer;
  }

  HashMap(std::initializer_list<value_type> list) : map_size(0) // biggest_hash(0),smallest_hash(MAX_HASH-1), max_hash(MAX_HASH)
  {
    buffer = new Node*[MAX_HASH+1];
    for(size_type i =0; i<MAX_HASH+1 ;i++) {
        buffer[i] = nullptr;
    }

    for(auto &element :list)
    {
        this->operator[](element.first) = element.second;
    }

  }

  HashMap(const HashMap& other)
  {

      buffer = new Node*[MAX_HASH+1];
      for(size_type i =0; i<MAX_HASH+1 ;i++)
          buffer[i] = nullptr;

      map_size=0;

      for(auto &element :other)
      {
          this->operator[](element.first) = element.second;
      }
  }

  HashMap(HashMap&& other)
  {
      buffer = other.buffer;
      other.buffer = nullptr;
      map_size = other.map_size;
      other.map_size = 0;
  }

  HashMap& operator=(const HashMap& other)
  {
      if(this ==&other) return *this ;
      clear();
      for(auto &element : other)
          this->operator[](element.first) = element.second;
      return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
      if(this ==&other) return *this ;
      clear();
      std::swap(buffer, other.buffer);
      std::swap(map_size, other.map_size);
      return *this;
  }

  bool isEmpty() const
  {
    return map_size==0;
  }

  mapped_type& operator[](const key_type& key)
  {
      size_type index = get_hash_index(key);
      if(buffer[index]!=nullptr) // list is not empty
      {
        Node* temporary=buffer[index];
        while(temporary!= nullptr) {
          if (temporary->element.first == key) return temporary->element.second;
          temporary=temporary->next;
        }
        //we need to add to the list
        Node* helper = buffer[index];
        temporary = new Node(value_type(key, mapped_type{}),index,helper, nullptr );
        helper->previous=temporary;
        buffer[index]=temporary;
        map_size++;
        return temporary->element.second;
      } else // list is empty
      {
        Node* temporary= new Node(value_type(key, mapped_type{}),index, nullptr, nullptr);
        buffer[index]=temporary;
        map_size++;
        return temporary->element.second;
      }
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    auto temporary = find(key);
    if(temporary == cend())//nie ma takiego elementu
      throw std::out_of_range("no such key");
    return (*temporary).second;//zwracamy wartosc pary
  }

  mapped_type& valueOf(const key_type& key)
  {
    auto temporary = find(key);
    if(temporary == end())//nie ma takiego elementu
      throw std::out_of_range("no such key");
    return (*temporary).second;//zwracamy wartosc pary
  }

  const_iterator find(const key_type& key) const
  {
    size_type index = get_hash_index(key);
    if(buffer[index]== nullptr)//list is empty
      return cend();
    else//list is not empty
    {
      Node* temporary= buffer[index];
      while(temporary!= nullptr)//przeszukujemy liste
      {
        if(temporary->element.first == key)// there is such element
          return ConstIterator(temporary, buffer);//  dopisac iterator
      }
      return cend();// there is not such element in list
    }
  }

  iterator find(const key_type& key)
  {
    size_type index = get_hash_index(key);
    if(buffer[index]== nullptr)//list is empty
      return cend();
    else//list is not empty
    {
      Node* temporary= buffer[index];
      while(temporary!= nullptr)//przeszukujemy liste
      {
        if(temporary->element.first == key)// there is such element
          return Iterator(temporary, buffer);//  dopisac iterator
      }
      return cend();// there is not such element in list
    }
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(it==end()) throw std::out_of_range ("removing element that not exist");

      Node* to_remove = it.node;
      size_type hash = get_hash_index(to_remove->element.first);

      Node* temporary = buffer[hash];
      while(temporary->element.first!=to_remove->element.first)
      {
          temporary=temporary->next;
      }
      if (temporary== nullptr) {throw std::out_of_range ("removing element that not exist");}
      if (temporary->next== nullptr && temporary->previous== nullptr) // we remove whole list
      {
          buffer[hash]= nullptr;
          delete temporary;
      } else if(temporary->next!= nullptr && temporary->previous == nullptr) // we remove first element
      {
            temporary->next->previous= nullptr;
            buffer[hash]= temporary->next;
            delete temporary;
      } else if(temporary->next== nullptr && temporary->previous!= nullptr)  //we remove last element
      {
          temporary->previous->next= nullptr;
          delete temporary;
      } else // we remove middle
      {
          temporary->next->previous=temporary->previous;
          temporary->previous->next=temporary->next;
          delete temporary;
      }
      map_size--;
  }

  size_type getSize() const
  {
    return map_size;
  }

  bool operator==(const HashMap& other) const
  {
      if(map_size != other.map_size)
          return false;
      auto iterator1 = cbegin();
      auto iterator2 = other.cbegin();
      while(iterator1 != cend() && iterator2 != other.cend())
      {
          if((*iterator1).first != (*iterator2).first || (*iterator1).second != (*iterator2).second)
              return false;
          ++iterator1;
          ++iterator2;
      }
      return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
        if (isEmpty()) return end();

    size_type hash=0;
    while(buffer[hash]== nullptr)
        hash++;
    return Iterator(buffer[hash], buffer);
  }

  iterator end()
  {
      return Iterator(buffer[MAX_HASH], buffer);
  }

  const_iterator cbegin() const
  {
      if (isEmpty()) return cend();

      size_type hash=0;
      while(buffer[hash]== nullptr)
          hash++;
      return ConstIterator(buffer[hash], buffer);
  }

  const_iterator cend() const
  {
      return ConstIterator(buffer[MAX_HASH], buffer);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;


private:
    Node* node;
    Node** buffer;

    friend class HashMap<KeyType, ValueType>;
    friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);

public:

  explicit ConstIterator(Node* node= nullptr, Node** buffer= nullptr): node(node), buffer(buffer)
  {}

  ConstIterator(const ConstIterator& other)
  {
   node=other.node;
   buffer=other.buffer;
  }

  ConstIterator& operator++()
  {
    if(node == nullptr)
      throw std::out_of_range("incrementing end");

    if(node->next == nullptr)
    {
      size_type hash = node->hash + 1;
      while(buffer[hash]== nullptr)
      {
        if(hash >= MAX_HASH)
        {
          node = nullptr;
          return *this;
        }
        //if(buffer[hash] != nullptr) break;
        hash++;
      }
      node = buffer[hash];
    }
    else node = node->next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator old = ConstIterator(node, buffer);
    operator++();
    return old;
  }

  ConstIterator& operator--()
  {
      if(node == nullptr)
      {
          size_t iter = MAX_HASH;
          while(iter-- > 0)
          {
              if(buffer[iter] == nullptr)
                  continue;
              auto temporary = buffer[iter];
              while(temporary->next != nullptr)
                  temporary = temporary->next;
              node = temporary;
              return *this;
          }
          throw std::out_of_range("out");
      }

      if(node->previous == nullptr)
      {
          auto hash = node->hash - 1;
          while(buffer[hash]==nullptr)
          {
              if(hash<=0)
              {
                  //hash = 0;
                  throw std::out_of_range ("out");
              }
              //if(buffer[hash] != nullptr) break;
              hash--;
          }
          node = buffer[hash];
      }
      else node = node->previous;
      return *this;
  }

  ConstIterator operator--(int)
  {
    auto old = ConstIterator(node, buffer);
    operator--();
    return old;
  }

  reference operator*() const
  {
    if(node == nullptr)
      throw std::out_of_range("out_of_range");
    return node->element;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const   // podjebane sprobowac samemu jak bedzie czas
  {
    bool iif = 0;
    if(node == nullptr || other.node == nullptr)
    {
      if(node == nullptr && other.node == nullptr)
        iif = 1;
    }
    else
      iif = node->element == other.node->element;

    return iif;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(Node* node=nullptr, Node** buffer= nullptr): ConstIterator(node,buffer)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
