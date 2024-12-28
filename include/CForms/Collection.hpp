#pragma once

#include "Predicate.hpp"

#include <functional>
#include <memory>
#include <vector>
#include <algorithm>

namespace cf {

/// Storage class for pointer references of objects, with iterator compatibility.
/// A collection does not claim ownership of its contents!
template<typename T>
class Collection {

private:
    
    std::vector<T*> m_items;
    
public:
    
    auto begin() {
        return m_items.begin();
    }
    auto begin() const {
        return m_items.begin();
    }
    auto end() {
        return m_items.end();
    }
    auto end() const {
        return m_items.end();
    }
    auto rbegin() {
        return m_items.rbegin();
    }
    auto rbegin() const {
        return m_items.rbegin();
    }
    auto rend() {
        return m_items.rend();
    }
    auto rend() const {
        return m_items.rend();
    }
    
    auto operator[](size_t index) {
        return m_items[index];
    }
    
    // Item at the specified index
    T* operator[](size_t index) const {
        if (index >= Count()) {
            return nullptr;
        }
        return m_items[index];
    }
    
    // Current number of items
    size_t Count() const {
        return m_items.size();
    }
    
    // Index of specified item. -1 if not found.
    int64_t IndexOf(T* item) const {
        for (size_t i = 0; i < m_items.size(); ++i) {
            if (m_items[i] == item) {
                return i;
            }
        }
        return -1;
    }
    
    // Check wether the given item is present in the collection
    bool Contains(T* item) const {
        for (const auto& inner : m_items) {
            if (inner == item) {
                return true;
            }
        }
        return false;
    }
    
    // Removes an item from the collection
    bool Remove(T* item) {
        auto it = std::find_if(m_items.begin(), m_items.end(), [item](T* inner) { return inner == item; });
        if (it != m_items.end()) {
            m_items.erase(it);
            return true;
        }
        return false;
    }
    
    // Remove an item at the specified index
    bool RemoveAt(size_t index) {
        if (index >= Count()) {
            return false;
        }
        m_items.erase(m_items.begin() + index);
        return true;
    }
    
    // Add an item to the collection
    bool Add(T* item) {
        for (const auto& inner : m_items) {
            if (inner == item) {
                return false;
            }
        }
        m_items.push_back(item);
        return true;
    }
    
    // Insert an item into the collection at the specified index
    bool Insert(size_t index, T* item) {
        if (index >= Count()) {
            Add(item);
            return true;
        }
        m_items.insert(m_items.begin() + index, item);
        return true;
    }
    
    // Find the first item matching the given predicate function
    T* Find(typename Predicate<T>::Ptr p) const {
        for (const auto& item : m_items) {
            if (p(item)) {
                return item;
            }
        }
        return nullptr;
    }
    
    // Find all items matching the given predicate function
    std::vector<T*> FindAll(typename Predicate<T>::Ptr p) const {
        std::vector<T*> result;
        for (const auto& item : m_items) {
            if (p(item)) {
                result.push_back(item);
            }
        }
        return result;
    }
    
    Collection() {
        m_items = std::vector<T*>();
    }
    
    virtual ~Collection() {}
    
};

}