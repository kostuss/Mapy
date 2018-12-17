#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
        Node* left;
        Node* right;
        Node* parent;
        size_t  balance;

        Node(key_type key, mapped_type value, Node* left= nullptr, Node* right= nullptr, Node* parent= nullptr):
            element(key, value), left(left), right(right), parent(parent), balance(0)
        {}
    };

    Node* root;
    size_type tree_size;
// napisac funckje do wywazania drzewa

    Node* find1 (const key_type key ) const
    {
      Node* temporary = root;

      while(temporary != nullptr)
      {
        if(temporary->element.first == key) break;
        if(temporary->element.first > key)
          temporary = temporary->left;
        else
          temporary = temporary->right;
      }
      return temporary;
    }


    Node* maximum(Node* other) const
    {
      if(isEmpty()) return nullptr;
      while(other->right!= nullptr)
      {
        other=other->right;
      }
      return other;
    }

    Node* minimum(Node* other) const
    {
      if(isEmpty()) return nullptr;
      while(other->left!= nullptr)
      {
        other=other->left;
      }
      return other;
    }

    void clear(Node* temporary)
    {
        if(temporary==nullptr) return; // we deleted all tree
        clear(temporary->left);
        clear(temporary->right);
        delete temporary;
    }

public:

  TreeMap(): root(nullptr), tree_size(0)
  {}
  ~TreeMap()
  {
      clear(root);
      root= nullptr;
      tree_size=0;
  }

  TreeMap(std::initializer_list<value_type> list)
  {
      root = nullptr;
      tree_size = 0;
      for(auto &element :list)
      {
          this->operator[](element.first) = element.second;
      }
  }

  TreeMap(const TreeMap& other)
  {
      root = nullptr;
      tree_size = 0;
    for(auto &element :other)
    {
      this->operator[](element.first) = element.second;
    }
  }

  TreeMap(TreeMap&& other)
  {
      root = other.root;
      other.root = nullptr;
      tree_size = other.tree_size;
      other.tree_size = 0;
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this ==&other) return *this ;
    clear(root);
    root= nullptr;
    tree_size=0;
      for(auto &element : other)
          this->operator[](element.first) = element.second;
      return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
      if(this == &other)
          return *this;
      clear(root);
      root= nullptr;
      tree_size=0;
      std::swap(root, other.root);
      std::swap(tree_size, other.tree_size);
      return *this;
  }

  bool isEmpty() const
  {
    return tree_size==0;
  }

  mapped_type& operator[](const key_type& key)
  {
    Node* temporary = root;
    Node* previous = nullptr;

    while(temporary != nullptr)  // check if given key already exist
    {
      previous = temporary;
      if(key == temporary->element.first)
        return temporary->element.second;
      else if(key > temporary->element.first)
        temporary = temporary->right;
      else
        temporary = temporary->left;
    }

    Node* newElement = new Node(key, mapped_type{});//wartosc nieistotna
    if(previous == nullptr)// only when tree is empty
      root = newElement;
    else if(key > previous->element.first) {
        previous->right = newElement;
        newElement->parent = previous;
    }
    else {
        previous->left = newElement;
        newElement->parent= previous;
    }
    ++tree_size;
    //rebalanceTree(newElement);// we need to rebalance tree after inserting element

    return newElement->element.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const_iterator temporary = find(key);
    if(temporary== cend()) throw std::out_of_range("no such key in the tree");
    return temporary.operator*().second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    iterator temporary = find(key);
    if(temporary== end()) throw std::out_of_range("no such key in the tree");
    return temporary.operator*().second;
  }

  const_iterator find(const key_type& key) const
  {
    Node* temporary= find1(key);
    return ConstIterator(temporary, maximum(root), minimum(root));
  }

  iterator find(const key_type& key)
  {
    Node* temporary= find1(key);
    return Iterator(temporary, maximum(root), minimum(root));
  }

  void remove(const key_type& key)
  {
        remove(find(key));
        /*
      Node* to_delete = find1(key);
      if(to_delete == nullptr) throw std::out_of_range("out_of_range");
      Node* right = to_delete->right;
      Node* left = to_delete->left;

      if(right != nullptr)
      {
          if(to_delete == root) root = right;
          else if(to_delete->parent->right == to_delete) to_delete->parent->right = right;
          else to_delete->parent->left = right;

          if(left != nullptr)
          {
              maximum(left)->right = right->left;
              right->left = left;
              left->parent = right;
              //balance(right);
          }
      }
      else
      {
          if(to_delete == root) root = left;
          else if(to_delete->parent->right == to_delete) to_delete->parent->right = left;
          else to_delete->parent->left = left;
      }
      delete to_delete;
      tree_size--;
         */
  }

  void remove(const const_iterator& it)
  {
     // if(it.current == nullptr) throw std::out_of_range("out_of_range");
     // remove(it.current->element.first);

    if(it ==end()) throw std::out_of_range("Removing non existing element");
    Node* to_remove = it.current;
    Node* previous = to_remove->parent;

    if(to_remove->left== nullptr && to_remove->right == nullptr) // we remove leaf
    {
        if(previous== nullptr) // we remove root
            root= nullptr;
        else if (previous->left==to_remove) // we remove left child
            previous->left= nullptr;
       else
            previous->right= nullptr;
        delete to_remove;
    } else if ( to_remove->left!= nullptr && to_remove->right== nullptr) // have only left child
    {
        if (previous == nullptr) //we delete root
        {
            root = to_remove->left;
            root->parent = nullptr;
        } else if (previous->left == to_remove) //we remove left child
        {
            previous->left=to_remove->left;
            to_remove->left->parent=previous;
        } else  //we remove right child
        {
            previous->right=to_remove->left;
            to_remove->left->parent=previous;
        }
        delete to_remove;

    } else if (to_remove->left== nullptr && to_remove->right!= nullptr) // have only right child
    {
        if (previous == nullptr) //we delete root
        {
            root = to_remove->right;
            root->parent = nullptr;
        } else if (previous->left == to_remove) //we remove left child
        {
            previous->left=to_remove->right;
            to_remove->right->parent=previous;
        } else  //we remove right child
        {
            previous->right=to_remove->right;
            to_remove->right->parent=previous;
        }
       delete to_remove;
    } else  // have two children
    {

        Node* temp= minimum(to_remove->right);
        if(to_remove->parent== nullptr) // we remove root
        {
            root=temp;
            root->left=to_remove->left;
            root->parent= nullptr;
        }
        else // not root
        {
            temp->left=to_remove->left;
            temp->parent=to_remove->parent;
        }
        delete to_remove;
    }
    tree_size--;
    //rebalance()

  }

  size_type getSize() const
  {
    return tree_size;
  }

  bool operator==(const TreeMap& other) const
  {
        if(tree_size!=other.tree_size) return false;
    const_iterator iterator1 = begin();
    const_iterator iterator2 = other.begin();

    while (iterator1 != end() ) {

        if((*iterator1).first != (*iterator2).first || (*iterator1).second != (*iterator2).second)
            return false;
      iterator1++;
      iterator2++;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return Iterator(minimum(root), maximum(root), minimum(root));
  }

  iterator end()
  {
    return Iterator(nullptr, maximum(root), minimum(root));
  }

  const_iterator cbegin() const
  {
    return ConstIterator(minimum(root), maximum(root), minimum(root));
  }

  const_iterator cend() const
  {
    return ConstIterator(nullptr, maximum(root), minimum(root));
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
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

private:
    friend class TreeMap<KeyType, ValueType>;
    Node* current;
    Node* maximum;    //do sprawdzenia begin i end
    Node* minimum;   // do sprawdzania begin i end
    friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);
public:

  explicit ConstIterator(Node* current, Node* maximum, Node*minimum):
  current(current), maximum(maximum), minimum(minimum)
  {}

  ConstIterator(const ConstIterator& other)
  {
   current=other.current;
  }

  ConstIterator& operator++()
  {
    if(current== nullptr) throw std::out_of_range("Incrementing end ");// tutaj coś napisać
    else {
      if (current->right == nullptr) {
        Node *previous = current;

        while (current->parent != nullptr) {
          current = current->parent;
          if (current->left == previous) return *this;
          previous=current;
        }
      }
      else
      {
        current= current->right;
        while(current->left!= nullptr)
        {
          current=current->left;
        }
        return *this;
      }
    }
    current= nullptr;
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto old= ConstIterator(current, maximum, minimum);
    operator++();
    return old;
  }

  ConstIterator& operator--()
  {
    if(current==minimum) throw std::out_of_range ("Decrementing beginning");
    if(current== nullptr) //decrementing end
    {
      current= maximum;
        return *this;
    }
    else {
      if (current->left == nullptr) {
        Node* previous = current;

        while (current->parent != nullptr) {
          current = current->parent;
          if (current->right == previous) return *this;
          previous=current;
        }
        //throw std::out_of_range("Decrementing beginning");
      }
      else
      {
        current= current->left;
        while(current->right!= nullptr)
        {
          current=current->right;
        }
        return *this;
      }
    }
    //current= nullptr;
    return *this;
  }

  ConstIterator operator--(int)
  {
    auto old = ConstIterator(current, maximum, minimum);
    operator--();
    return old;
  }

  reference operator*() const
  {
    if (current== nullptr) throw std::out_of_range("Referencing from end");
    return current->element;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
      bool iif = 1;
      if(current == nullptr || other.current == nullptr)
      {
          if(!(current == nullptr && other.current == nullptr))
              iif = 0;
      }
      else
          iif = current->element == other.current->element;

      return iif;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(Node* current, Node* maximum, Node* minimum): ConstIterator(current, maximum, minimum)
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

#endif /* AISDI_MAPS_MAP_H */
