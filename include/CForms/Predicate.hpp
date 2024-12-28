#pragma once

#include <functional>

namespace cf {

/// Comparison function for use in cf::ObjectOwner and cf::Collection.
template <typename T>
struct Predicate {
    
    /// Predicate<T>::Ref p = [value](const T& target) {
    ///     return target.value == value;
    /// });
    
    /// Constant reference comparison function.
    using Ref = std::function<bool(const T&)>;
    
    
    // Predicate<T>::Ptr p = [value](T* target) {
    //     return target->value == value;
    // });
    
    /// Pointer reference comparison function.
    using Ptr = std::function<bool(T*)>;
    
};

}
