#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "AnsiTerminal.h"
#include "Cell.h"
#include "Custom2DArray.h"
#include "FileManager.h"
#include <string>
#include <memory>

using namespace std;

class Cell; // Forward declaration to avoid circular dependency

namespace GTUSpreadsheet {

class Spreadsheet : public std::enable_shared_from_this<Spreadsheet> {
public:
    // Constructs a spreadsheet with the specified number of rows and columns
    Spreadsheet(int rows, int cols);

    // Destructor for cleanup
    ~Spreadsheet();

    // Factory method to create a shared pointer to a Spreadsheet object
    static std::shared_ptr<Spreadsheet> create(int rows, int cols); 

    // Returns the total number of rows in the spreadsheet
    int getTotalRows() const;

    // Returns the total number of columns in the spreadsheet
    int getTotalCols() const;

    // Returns the number of visible rows in the spreadsheet
    int getVisibleRows() const;

    // Returns the number of visible columns in the spreadsheet
    int getVisibleCols() const;

    // Parses a cell reference string (e.g., "A1") and extracts row and column indices
    void parseCellReference(const std::string& reference, int& row, int& col) const;

    // Recalculates dependencies for the specified cell to ensure formula correctness
    void recalculateDependencies(int row, int col);

    // Sets the content of a specified cell in the grid
    void setCellContent(int row, int col, const std::string& content);

    // Returns a shared pointer to a cell at the specified position
    std::shared_ptr<Cell> getCell(int row, int col) const;

    // Handles user keyboard inputs for navigation and interaction
    void handleInput(char key, int curRow, int curCol, Utils::FileManager &fileManager);

    // Draws the grid on the terminal, highlighting the currently selected cell
    void drawGrid(AnsiTerminal& terminal, int selectedRow, int selectedCol, int rowOffset, int colOffset);

    // Converts a column index to a corresponding label (e.g., 0 -> "A")
    std::string getColumnLabel(int col) const;

    // Evaluates the formula in a specified cell, updating its computed value
    void evaluateFormula(int row, int col);

    // Converts a column label (e.g., "A") into its numerical index
    int columnToIndex(const std::string& column) const;

    // Dynamically resizes the grid to accommodate more rows or columns
    void resizeGrid(int newRows, int newCols);

    // Clears all content in the spreadsheet
    void clear();

private:
    int totalRows;          // Total number of rows in the spreadsheet
    int totalCols;          // Total number of columns in the spreadsheet
    int visibleRows;        // Number of rows visible on the terminal
    int visibleCols;        // Number of columns visible on the terminal
    int cellWidth;          // Width of each cell for uniform spacing in the grid

    // The 2D container storing cells in the spreadsheet
    Dynamic2DVector<std::shared_ptr<Cell>> grid;

    // Initializes the grid with empty cells during construction
    void initializeGrid();
};

} // namespace GTUSpreadsheet

#endif // SPREADSHEET_H
