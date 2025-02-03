#ifndef CUSTOM2DARRAY
#define CUSTOM2DARRAY

#include <memory>
#include <stdexcept>

template <typename T>
class Dynamic2DVector {
private:
    std::unique_ptr<std::unique_ptr<T[]>[]> data;
    int rows;
    int cols;
    
    void allocate(int newRows, int newCols) {
        data = std::make_unique<std::unique_ptr<T[]>[]>(newRows);
        for (int i = 0; i < newRows; ++i) {
            data[i] = std::make_unique<T[]>(newCols);
        }
    }

public:
    // Constructor
    Dynamic2DVector(int initialRows = 21, int initialCols = 8) 
        : rows(initialRows), cols(initialCols) {
        allocate(rows, cols);
    }

    // Copy constructor
    Dynamic2DVector(const Dynamic2DVector& other) 
        : rows(other.rows), cols(other.cols) {
        allocate(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

    // Move constructor
    Dynamic2DVector(Dynamic2DVector&& other) noexcept
        : data(std::move(other.data)), rows(other.rows), cols(other.cols) {
        other.rows = 0;
        other.cols = 0;
    }

    // Copy assignment operator
    Dynamic2DVector& operator=(const Dynamic2DVector& other) {
        if (this != &other) {
            Dynamic2DVector temp(other);
            *this = std::move(temp);
        }
        return *this;
    }

    // Move assignment operator
    Dynamic2DVector& operator=(Dynamic2DVector&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            rows = other.rows;
            cols = other.cols;
            other.rows = 0;
            other.cols = 0;
        }
        return *this;
    }

    // Resize method
    void resize(int newRows, int newCols) {
        if (newRows <= 0 || newCols <= 0) {
            throw std::invalid_argument("New dimensions must be positive");
        }

        // Only resize if new dimensions are larger
        if (newRows <= rows && newCols <= cols) {
            return;
        }

        // Create new array with expanded dimensions
        Dynamic2DVector<T> temp(newRows, newCols);

        // Copy existing data
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                temp.data[i][j] = std::move(data[i][j]);
            }
        }

        // Move the temporary object
        *this = std::move(temp);
    }

    T& at(int row, int col) {
        if (row >= rows || col >= cols) {
            // Calculate new dimensions with some extra space
            int newRows = std::max(rows, row + 10);  // Add 10 extra rows
            int newCols = std::max(cols, col + 5);   // Add 5 extra columns
            resize(newRows, newCols);
        }
        return data[row][col];
    }

    const T& at(int row, int col) const {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[row][col];
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

#endif