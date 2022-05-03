#pragma once
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <iostream>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_reserve_(capacity_to_reserve) {
    }

    size_t GetCapacity() {
        return capacity_reserve_;
    }

private:
    size_t capacity_reserve_ = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) 
        : SimpleVector(size, Type()) {
    }

        // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)    
        : vector_(size), size_(size), capacity_(size) {
        std::fill(vector_.Get(), vector_.Get() + size, value);
    }  
    
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
       :vector_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), vector_.Get());
    } 
    
    SimpleVector(const SimpleVector& other) {
        assert(size_ == 0);
        SimpleVector<Type> cop(other.GetSize());
        std::copy((other.vector_).Get(), ((other.vector_).Get() + other.GetSize()), (cop.vector_).Get());
        cop.capacity_ = other.capacity_;
        swap(cop);
    }
     SimpleVector(SimpleVector&& other) {
        assert(size_ == 0);
        vector_ .swap(other.vector_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

      
    SimpleVector(ReserveProxyObj obj) {
        Reserve(obj.GetCapacity());
    }
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty()) {
                Clear();
            } else {
                SimpleVector<Type> cop(rhs);
                swap(cop);
            }
        }
        return *this;
    } 
   
     SimpleVector& operator=(SimpleVector&&) = default;
     
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_==0;
    }
       
    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
       return vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
    if (index >= size_) {
        using namespace std;
        throw std::out_of_range("index >= size"s);}
        return vector_[index];  
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
     if (index >= size_) {
        using namespace std;
        throw std::out_of_range("index >= size"s);}
        return vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_=0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
      void Resize(size_t new_size) {
        if (size_ < new_size) {
            if (new_size <= capacity_){
                auto first = vector_.Get() + size_;
                auto last = vector_.Get() + new_size;
                assert(first < last);
                while (first < last) {
                    *first = std::move(Type());
                    first++;   
                }    
            } else{
                ReCapacity(new_size * 2);}
        }
        size_ = new_size;
    }
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
       if (size_ == capacity_) {
        ReCapacity(std::max(size_ * 2, static_cast<size_t>(1)));   
        }
         *(end()) = (item);
         ++size_;
    }
    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            ReCapacity(std::max(size_ * 2, static_cast<size_t>(1)));
        }
        *(end()) = std::move(item);
        size_++;
    }

     // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        vector_.swap(other.vector_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }
   // задает ёмкость вектора
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ReCapacity(new_capacity);
        }
    }
    
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return (vector_.Get() + size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return  (vector_.Get() + size_);
    }
     // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        auto step_ = pos - begin();
        if (capacity_ == size_) {
            ReCapacity(std::max(size_ * 2, static_cast<size_t>(1)));
        }
        std::copy_backward(begin() + step_, end(), end() + 1);
        auto* it = begin() + step_;
        *it = std::move(value);
        size_++;
        return it;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        auto step_ = pos - begin();
        if (capacity_ == size_) {
            ReCapacity(std::max(size_ * 2, static_cast<size_t>(1)));
        }
        std::move_backward(begin() + step_, end(), end() + 1);
        auto* it = begin() + step_;
        *it = std::move(value);
        size_++;
        return it;
    }
  
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
       assert(pos >= begin() && pos < end());
        auto step_ = pos - begin();
        auto it = begin() + step_;
        std::move((it + 1), end(), it);
        --size_;
        return (begin() + step_);
    } 
     
private:
    ArrayPtr<Type> vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
    
    void ReCapacity(size_t new_capacity) {
        ArrayPtr<Type> new_vector_(new_capacity);
        std::move(vector_.Get(), vector_.Get() + size_, new_vector_.Get());
        vector_.swap(new_vector_);
        capacity_ = new_capacity;
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return  !operator==(lhs, rhs);
}
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return   rhs < lhs;
}
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs);
} 

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
