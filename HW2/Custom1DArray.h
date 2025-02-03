#ifndef DYNAMIC1DARRAY_H
#define DYNAMIC1DARRAY_H

#include <stdexcept>

// Template class for a dynamic array
template <class T>
class DynamicArray {
private:
    T* data;          // Pointer to the array data
    int capacity;     // Capacity of the array
    int size;         // Current size of the array

    // Resizes the array to the new capacity
    void resize(int newCapacity);

public:
    // Constructor with an optional initial capacity
    DynamicArray(int initialCapacity = 10);

    // Destructor to clean up the allocated memory
    ~DynamicArray();

    // Adds a new element to the end of the array
    void pushBack(const T& value);

    // Overloaded subscript operator to access elements
    const T& operator[](int index) const;

    // Returns the current size of the array
    int getSize() const;

    // Clears the array
    void clear();
};

// Implementation

// Clears the array by setting size to 0
template <class T>
void DynamicArray<T>::clear() {
    size = 0;
}

// Constructor to initialize the array with the given capacity
template <class T>
DynamicArray<T>::DynamicArray(int initialCapacity)
    : capacity(initialCapacity), size(0) {
    data = new T[capacity];
}

// Destructor to delete the allocated array
template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

// Resizes the array to the new capacity
template <class T>
void DynamicArray<T>::resize(int newCapacity) {
    T* newData = new T[newCapacity];
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

// Adds a new element to the end of the array, resizing if necessary
template <class T>
void DynamicArray<T>::pushBack(const T& value) {
    if (size == capacity) {
        resize(capacity * 2);
    }
    data[size++] = value;
}

// Overloaded subscript operator to access elements with bounds checking
template <class T>
const T& DynamicArray<T>::operator[](int index) const {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

// Returns the current size of the array
template <class T>
int DynamicArray<T>::getSize() const {
    return size;
}

#endif // DYNAMIC1DARRAY_H
