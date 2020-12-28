/****************************************************************************************************************************
  StringArray_STM32.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
  For STM32 with built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)
  
  AsyncWebServer_STM32 is a library for the STM32 run built-in Ethernet WebServer
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32
  Licensed under MIT license
 
  Version: 1.2.5
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      02/09/2020 Initial coding for STM32 for built-in Ethernet (Nucleo-144, DISCOVERY, etc).
                                  Bump up version to v1.2.3 to sync with ESPAsyncWebServer v1.2.3
  1.2.4   K Hoang      05/09/2020 Add back MD5/SHA1 authentication feature.
  1.2.5   K Hoang      28/12/2020 Suppress all possible compiler warnings. Add examples.
 *****************************************************************************************************************************/

#pragma once
 
#ifndef STRINGARRAY_STM32_H_
#define STRINGARRAY_STM32_H_

#include "stddef.h"
#include "WString.h"

template <typename T>
class LinkedListNode 
{
    T _value;
    
  public:
    LinkedListNode<T>* next;
    LinkedListNode(const T val): _value(val), next(nullptr) {}
    ~LinkedListNode() {}
    
    const T& value() const 
    {
      return _value;
    };
    
    T& value() 
    {
      return _value;
    }
};

template <typename T, template<typename> class Item = LinkedListNode>
class LinkedList 
{
  public:
    typedef Item<T> ItemType;
    typedef std::function<void(const T&)> OnRemove;
    typedef std::function<bool(const T&)> Predicate;
    
  private:
    ItemType* _root;
    OnRemove _onRemove;

    class Iterator 
    {
        ItemType* _node;
        
      public:
        Iterator(ItemType* current = nullptr) : _node(current) {}
        Iterator(const Iterator& i) : _node(i._node) {}
        
        Iterator& operator ++() 
        {
          _node = _node->next;
          return *this;
        }
        
        bool operator != (const Iterator& i) const 
        {
          return _node != i._node;
        }
        
        const T& operator * () const 
        {
          return _node->value();
        }
        
        const T* operator -> () const 
        {
          return &_node->value();
        }
    };

  public:
    typedef const Iterator ConstIterator;
    
    ConstIterator begin() const 
    {
      return ConstIterator(_root);
    }
    
    ConstIterator end() const 
    {
      return ConstIterator(nullptr);
    }

    LinkedList(OnRemove onRemove) : _root(nullptr), _onRemove(onRemove) {}
    ~LinkedList() {}
    
    void add(const T& t) 
    {
      auto it = new ItemType(t);
      
      if (!_root) 
      {
        _root = it;
      } 
      else 
      {
        auto i = _root;
        
        while (i->next) 
          i = i->next;
          
        i->next = it;
      }
    }
    
    T& front() const 
    {
      return _root->value();
    }

    bool isEmpty() const 
    {
      return _root == nullptr;
    }
    
    size_t length() const 
    {
      size_t i = 0;
      auto it = _root;
      
      while (it) 
      {
        i++;
        it = it->next;
      }
      
      return i;
    }
    
    size_t count_if(Predicate predicate) const 
    {
      size_t i = 0;
      auto it = _root;
      
      while (it) 
      {
        if (!predicate) 
        {
          i++;
        }
        else if (predicate(it->value())) 
        {
          i++;
        }
        
        it = it->next;
      }
      return i;
    }
    
    const T* nth(size_t N) const 
    {
      size_t i = 0;
      auto it = _root;

      while (it) 
      {
        if (i++ == N)
          return &(it->value());
          
        it = it->next;
      }
      
      return nullptr;
    }
    
    bool remove(const T& t) 
    {
      auto it = _root;
      auto pit = _root;
      
      while (it) 
      {
        if (it->value() == t) 
        {
          if (it == _root) 
          {
            _root = _root->next;
          } 
          else 
          {
            pit->next = it->next;
          }

          if (_onRemove) 
          {
            _onRemove(it->value());
          }

          delete it;
          return true;
        }
        
        pit = it;
        it = it->next;
      }
      
      return false;
    }
    
    bool remove_first(Predicate predicate) 
    {
      auto it = _root;
      auto pit = _root;
      
      while (it) 
      {
        if (predicate(it->value())) 
        {
          if (it == _root) 
          {
            _root = _root->next;
          } 
          else 
          {
            pit->next = it->next;
          }
          
          if (_onRemove) 
          {
            _onRemove(it->value());
          }
          
          delete it;
          return true;
        }
        
        pit = it;
        it = it->next;
      }
      
      return false;
    }

    void free() 
    {
      while (_root != nullptr) 
      {
        auto it = _root;
        _root = _root->next;
        
        if (_onRemove) 
        {
          _onRemove(it->value());
        }
        
        delete it;
      }
      
      _root = nullptr;
    }
};


class StringArray : public LinkedList<String> 
{
  public:

    StringArray() : LinkedList(nullptr) {}

    bool containsIgnoreCase(const String& str) 
    {
      for (const auto& s : *this) 
      {
        if (str.equalsIgnoreCase(s)) 
        {
          return true;
        }
      }
      
      return false;
    }
};

#endif /* STRINGARRAY_STM32_H_ */
