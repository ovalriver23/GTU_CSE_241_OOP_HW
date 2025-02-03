#include "Spreadsheet.h"
#include"FileManager.h"
#include "Cell.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <memory>


namespace GTUSpreadsheet {

// Factory method for creating a Spreadsheet instance using shared_ptr
shared_ptr<Spreadsheet> Spreadsheet::create(int rows, int cols) {
    //Create a shared pointer to a new Spreadsheet object
    auto sheet = shared_ptr<Spreadsheet>(new Spreadsheet(rows, cols));
    // Then initialize the grid
    sheet->initializeGrid();
    //Return the fully initialized shared pointer
    return sheet;
}

//Constructor for initializing the spreadsheet dimensions and grid size
Spreadsheet::Spreadsheet(int rows, int cols) 
    : totalRows(rows), 
      totalCols(cols),
      visibleRows(21), 
      visibleCols(8),
      cellWidth(9),
      grid(rows, cols) {
}

// Initializes the grid by creating and assigning cells to the grid structure
void Spreadsheet::initializeGrid() {
    // First create all cells
    for (int i = 0; i < totalRows; ++i) {
        for (int j = 0; j < totalCols; ++j) {
            // Ensure the cell is only created if it's uninitialized
            if (!grid.at(i, j)) {  
                auto cell = make_shared<ValueCell>(""); // Create a new ValueCell with default content
                cell->setPosition(i, j);
                grid.at(i, j) = cell;
            }
        }
    }
    //Assign the spreadsheet reference to each cell
    // Required for formulas and references within the spreadsheet
    auto self = shared_from_this(); // Capture the shared pointer to this object
    for (int i = 0; i < totalRows; ++i) {
        for (int j = 0; j < totalCols; ++j) {
            // Assign the spreadsheet reference only if the cell is initialized
            if (grid.at(i, j)) {
                grid.at(i, j)->setSpreadsheet(self);
            }
        }
    }
}


// Resizes the grid to new dimensions while preserving existing data and initializing new cells
void Spreadsheet::resizeGrid(int newRows, int newCols) {
    // Check if the new dimensions are valid
    if (newRows <= 0 || newCols <= 0) {
        throw invalid_argument("Grid dimensions must be positive");
    }

    // Store old dimensions
    int oldRows = totalRows;
    int oldCols = totalCols;

    // Update dimensions
    totalRows = newRows;
    totalCols = newCols;

    // Resize the grid
    grid.resize(newRows, newCols);

    // Initialize new cells
    for (int i = 0; i < newRows; ++i) {
        for (int j = 0; j < newCols; ++j) {
            // Only create new cells if they are out of the old grid bounds
            if (i >= oldRows || j >= oldCols) {
                auto cell = make_shared<ValueCell>("");
                cell->setPosition(i, j);
                cell->setSpreadsheet(shared_from_this());
                grid.at(i, j) = cell;
            }
        }
    }
}

//Parses a cell reference string (e.g., "A1") into numeric row and column indices
void Spreadsheet::parseCellReference(const string& reference, int& row, int& col) const {
    string colLabel, rowLabel;

    //Separate column labels and row numbers from the reference string
    for (char ch : reference) {
        if (isalpha(ch)) {
            colLabel += ch; // Collect alphabetical characters as column label
        } else if (isdigit(ch)) {
            rowLabel += ch; // Collect numerical characters as row number
        } else {
            throw runtime_error("Invalid cell reference: " + reference);
        }
    }

    if (colLabel.empty() || rowLabel.empty()) {
        throw runtime_error("Incomplete cell reference: " + reference);
    }

    col = colLabel[0] - 'A'; // Assuming single-character column labels
    row = stoi(rowLabel) - 1; // Convert to zero-based index
}

// Destructor
Spreadsheet::~Spreadsheet() = default;

// Get total rows
int Spreadsheet::getTotalRows() const {
    return totalRows;
}

// Get total columns
int Spreadsheet::getTotalCols() const {
    return totalCols;
}

// Get visible rows
int Spreadsheet::getVisibleRows() const {
    return visibleRows;
}

// Get visible columns
int Spreadsheet::getVisibleCols() const {
    return visibleCols;
}

// Get column label (A, B, C, ...)
string Spreadsheet::getColumnLabel(int col) const {
    string label;
    while (col >= 0) {
        label = static_cast<char>(col % 26 + 'A') + label;
        col = col / 26 - 1;
    }
    return label;
}


//Renders the spreadsheet grid to the terminal and handles cursor positioning and resizing
void Spreadsheet::drawGrid(AnsiTerminal& terminal, int selectedRow, int selectedCol, 
                          int rowOffset, int colOffset){
    try {
        terminal.clearScreen();

        //Auto-resize the grid if the selection exceeds the current grid size
        if (selectedRow >= totalRows || selectedCol >= totalCols) {
            resizeGrid(max(totalRows, selectedRow + 10), 
                      max(totalCols, selectedCol + 5));
        }

        //Adjust vertical offset if the selected row is outside the visible range
        if (selectedRow - rowOffset >= visibleRows) {
            rowOffset = selectedRow - visibleRows + 1;
        } else if (selectedRow < rowOffset) {
            rowOffset = selectedRow;
        }
    
        //Adjust horizontal offset if the selected column is outside the visible range
        if (selectedCol - colOffset >= visibleCols) {
            colOffset = selectedCol - visibleCols + 1;
        } else if (selectedCol < colOffset) {
            colOffset = selectedCol;
        }

        // Get the currently selected cell
        shared_ptr<Cell> selectedCell = grid.at(selectedRow, selectedCol);
        
        // First line: Cell info and content
        string currentCellInfo = getColumnLabel(selectedCol) + to_string(selectedRow + 1);
        string typeDisplay;
        string cellContent = selectedCell ? selectedCell->getRawContent() : "";

        // Determine cell type display
        if (auto strCell = dynamic_pointer_cast<StringValueCell>(selectedCell)) {
            typeDisplay = "(L)"; // Label
        } else if (auto intCell = dynamic_pointer_cast<IntValueCell>(selectedCell)) {
            typeDisplay = "(V)"; // Value
        } else if (auto doubleCell = dynamic_pointer_cast<DoubleValueCell>(selectedCell)) {
            typeDisplay = "(V)"; // Value
        }else if (auto formulaCell = dynamic_pointer_cast<FormulaCell>(selectedCell)) {
            typeDisplay = "(F)"; // Formula
        }else {
            typeDisplay = "";
        }

        // Pad the first line
        string paddedFirstLine = currentCellInfo + " " + typeDisplay + " " + cellContent;
        while (paddedFirstLine.length() < 75) {
            paddedFirstLine += ' ';
        }
        terminal.printInvertedAt(0, 1, paddedFirstLine);

        // Display raw content (formula or value)
        string rawContent = selectedCell ? selectedCell->getRawContent() : "";
        string paddedInputDisplay = rawContent;
        while (paddedInputDisplay.length() < 75) {
            paddedInputDisplay += ' ';
        }
        terminal.printAt(3, 1, paddedInputDisplay);

        // Display cell type
        string typeDisplayLine;
        if (auto strCell = dynamic_pointer_cast<StringValueCell>(selectedCell)) {
            typeDisplayLine = "Label";
        } else if (auto intCell = dynamic_pointer_cast<IntValueCell>(selectedCell)) {
            typeDisplayLine = "Value";
        } else if (auto doubleCell = dynamic_pointer_cast<DoubleValueCell>(selectedCell)) {
            typeDisplayLine = "Value";
        } else if (auto formulaCell = dynamic_pointer_cast<FormulaCell>(selectedCell)) {
            typeDisplayLine = "Formula";
        }else {
            typeDisplayLine = "";
        }

        string paddedTypeDisplay = typeDisplayLine;
        while (paddedTypeDisplay.length() < 75) {
            paddedTypeDisplay += ' ';
        }
        terminal.printInvertedAt(2, 1, paddedTypeDisplay);

        // Grid display starting position
        const int gridStartRow = 5;

        // Print top-left corner
        terminal.printInvertedAt(gridStartRow - 1, 1, string(cellWidth, ' '));

        // Print column headers
        for (int col = 0; col < visibleCols; ++col) {
            int actualCol = col + colOffset;
            if (actualCol >= getTotalCols()) break;

            string colLabel = getColumnLabel(actualCol);
            int paddingLeft = (cellWidth - colLabel.size()) / 2;
            int paddingRight = cellWidth - colLabel.size() - paddingLeft;

            string paddedColLabel(paddingLeft, ' ');
            paddedColLabel += colLabel;
            paddedColLabel += string(paddingRight, ' ');

            terminal.printInvertedAt(gridStartRow - 1, 4 + col * cellWidth, paddedColLabel);
        }

        // Print rows with content
        for (int row = 0; row < visibleRows; ++row) {
            int actualRow = row + rowOffset;
            if (actualRow >= getTotalRows()) break;

            // Print row label
            string rowLabel = to_string(actualRow + 1);
            string paddedRowLabel = rowLabel + string(cellWidth - rowLabel.length() - 1, ' ') + ' ';
            terminal.printInvertedAt(gridStartRow + row, 1, paddedRowLabel);

            // Print cell contents
            for (int col = 0; col < visibleCols; ++col) {
                int actualCol = col + colOffset;
                if (actualCol >= getTotalCols()) break;

                shared_ptr<Cell> cell = grid.at(actualRow, actualCol);
                string content = cell ? cell->getContent() : "";

                // Trim content if too long
                string displayContent = content.length() > cellWidth ? 
                    content.substr(0, cellWidth) : content;
                
                // Pad content
                string paddedContent = displayContent + 
                    string(cellWidth - displayContent.length(), ' ');

                int cellRow = gridStartRow + row;
                int cellCol = 4 + col * cellWidth;

                if (actualRow == selectedRow && actualCol == selectedCol) {
                    terminal.printInvertedAt(cellRow, cellCol, paddedContent);
                } else {
                    if (!content.empty()) {
                        terminal.printAt(cellRow, cellCol, paddedContent);
                    } else {
                        terminal.printAt(cellRow, cellCol, string(cellWidth, ' '));
                    }
                }
            }
        }
    } catch (const exception& e) {
        throw runtime_error("Error in drawGrid: " + string(e.what()));
    }
}


// Clear the spreadsheet
void Spreadsheet::clear() {
    for (int r = 0; r < totalRows; ++r) {
        for (int c = 0; c < totalCols; ++c) {
            grid.at(r, c) = nullptr;
        }
    }
}


void Spreadsheet::handleInput(char key, int curRow, int curCol, Utils::FileManager &fileManager) {
    AnsiTerminal terminal;
    //Ignore navigation keys at this point
    if (key == 'U' || key == 'D' || key == 'L' || key == 'R') {
        return;
    }

    int rowOffset;
    int colOffset;

    //Handle directional input for moving the cursor
    switch(key) {
        case 'U':
            if (curRow > 0) {
                curRow--;
                if (curRow < rowOffset) {
                    rowOffset = curRow;
                }
            }
            break;
            
        case 'D':
            curRow++;
            if (curRow >= totalRows) {
                resizeGrid(curRow + 10, totalCols); // Expand grid if needed
            }
            if (curRow - rowOffset >= visibleRows) {
                rowOffset = curRow - visibleRows + 1;
            }
            break;
            
        case 'L':
            if (curCol > 0) {
                curCol--;
                if (curCol < colOffset) {
                    colOffset = curCol;
                }
            }
            break;
            
        case 'R':
            curCol++;
            if (curCol >= totalCols) {
                resizeGrid(totalRows, curCol + 5); // Expand grid horizontally if needed
            }
            if (curCol - colOffset >= visibleCols) {
                colOffset = curCol - visibleCols + 1;
            }
            break;
    }

    //Update the grid display after moving the cursor
    drawGrid(terminal, curRow, curCol, rowOffset, colOffset);

    //Ensure grid expansion if the cursor moves beyond current bounds
    if (curRow >= totalRows || curCol >= totalCols) {
        resizeGrid(curRow + 1, curCol + 1);
    }

    shared_ptr<Cell> currentCell = grid.at(curRow, curCol);
    string temp;
    if (currentCell) {
        temp = currentCell->getRawContent(); 
    } else {
        temp = "";
    }


    // Open the file menu when '\' is pressed
    if (key == '\\') {
        terminal.clearScreen();
        terminal.printAt(1, 1, "Menu:");
        terminal.printAt(2, 1, "1. Save File");
        terminal.printAt(3, 1, "2. Load File");
        terminal.printAt(4, 1, "3. Cancel");
        terminal.printAt(5, 1, "Enter your choice: ");

        char choice = terminal.getKeystroke();
        switch (choice) {
            case '1': {
                terminal.printAt(6, 1, "Enter file name to save as: ");
                string fileName;
                cin >> fileName;
                fileManager.saveFileAs(fileName);
                break;
            }
            case '2': {
                terminal.printAt(6, 1, "Enter file name to load: ");
                string fileName;
                cin >> fileName;
                fileManager.loadFile(fileName);
                break;
            }
            case '3': // Cancel
                break;
            default:
                break;
        }
        terminal.clearScreen();
        return;
    }

    //Handle backspace for deleting the last character in the cell content
    if(key == '\b' || key == 127 || key == 8) {
        if (!temp.empty()) {
            temp.pop_back();
            setCellContent(curRow, curCol, temp);
        }
    //Process regular input for formulas and values
    } else if(key != 'U' && key != 'D' && key != 'R' && key != 'L') {
        temp.push_back(key);

        //If the input starts a formula (using '@' for functions)
        if(temp[0] == '@' && temp.find(')') != string::npos) {
            auto formulaCell = make_shared<FormulaCell>(temp, shared_from_this());
            grid.at(curRow, curCol) = formulaCell;
            try {
                formulaCell->evaluate(); // Try to evaluate the formula
            } catch (const exception& e) {
                formulaCell->setContent("#ERROR");
            }
        //If the input starts an arithmetic formula (using '=')
        } else if(temp[0] == '=') {
            auto formulaCell = make_shared<FormulaCell>(temp, shared_from_this());
            grid.at(curRow, curCol) = formulaCell;
            try {
                formulaCell->evaluate();
            } catch (const exception& e) {
                formulaCell->setContent("#ERROR");
            }
        //Otherwise, treat input as a regular cell value
        } else {
            setCellContent(curRow, curCol, temp);
        }
    }
}

//Evaluates the formula in a specific cell (if it is a FormulaCell)
void Spreadsheet::evaluateFormula(int row, int col) {
    auto cell = grid.at(row, col);
    if (!cell) return;
    //Attempt to cast the cell to a FormulaCell
    auto formulaCell = dynamic_pointer_cast<FormulaCell>(cell);
    if (!formulaCell) return;

    try {
        formulaCell->evaluate();
    } catch (const exception& e) {
        // Keep the formula visible if evaluation fails
        formulaCell->setContent(formulaCell->getRawContent());
    }
}

// Converts a column label (e.g., "A", "B", "AA") into a zero-based column index
int GTUSpreadsheet::Spreadsheet::columnToIndex(const string& column) const {
    int index = 0;
    for (char ch : column) {
        if (isalpha(ch)) {
            index = index * 26 + (toupper(ch) - 'A' + 1);
        } else {
            throw invalid_argument("Invalid column label: " + column);
        }
    }
    return index - 1; // Convert to zero-based index
}

// Recalculates all cells that depend on the specified cell (row, col)
void GTUSpreadsheet::Spreadsheet::recalculateDependencies(int row, int col) {
    //Iterate through all cells in the grid
    for (int r = 0; r < totalRows; ++r) {
        for (int c = 0; c < totalCols; ++c) {
            //Retrieve the current cell and check if it's a FormulaCell
            auto cell = grid.at(r, c);
            auto formulaCell = dynamic_pointer_cast<FormulaCell>(cell);
            //If it's a FormulaCell, check its dependencies
            if (formulaCell) {
                const auto& deps = formulaCell->getDependencies(); // Access dependencies
                //Check if the specified (row, col) matches any of the dependencies
                for (int i = 0; i < deps.getSize(); ++i) {
                    if (deps[i].first == row && deps[i].second == col) {
                        formulaCell->evaluate(); // Recalculate the dependent cell's formula
                        break;
                    }
                }
            }
        }
    }
}


//Sets the content of a specified cell and determines the appropriate cell type
void GTUSpreadsheet::Spreadsheet::setCellContent(int row, int col, const string& content) {
    if (row < 0 || row >= totalRows || col < 0 || col >= totalCols) {
        throw out_of_range("Cell position out of range");
    }

    // If the content is empty, clear the cell and update dependencies
    if (content.empty()) {
        grid.at(row, col) = nullptr;
        recalculateDependencies(row, col);
        return;
    }

    // If content is a formula function starting with '@'
    if (content[0] == '@' && content.find(')') != string::npos) {
        auto formulaCell = make_shared<FormulaCell>(content, shared_from_this());
        grid.at(row, col) = formulaCell;
        try {
            formulaCell->evaluate();
        } catch (const exception& e) {
            formulaCell->setContent("#ERROR");
        }
    // If content starts with '=', treat it as a regular formula
    } else if (content[0] == '=') {
        auto formulaCell = make_shared<FormulaCell>(content, shared_from_this());
        grid.at(row, col) = formulaCell;
        formulaCell->evaluate();
        recalculateDependencies(row, col);
    } else {
        try {
            // First check if the content contains a decimal point
            if (content.find('.') != string::npos) {
                // If it has a decimal point, parse as double
                double doubleValue = stod(content);
                grid.at(row, col) = make_shared<DoubleValueCell>(doubleValue);
            } else {
                try {
                    // If no decimal point, try parsing as integer first
                    int intValue = stoi(content);
                    grid.at(row, col) = make_shared<IntValueCell>(intValue);
                } catch (...) {
                    // If integer parsing fails, try as double
                    try {
                        double doubleValue = stod(content);
                        grid.at(row, col) = make_shared<DoubleValueCell>(doubleValue);
                    } catch (...) {
                        // If all numeric parsing fails, treat as string
                        grid.at(row, col) = make_shared<StringValueCell>(content);
                    }
                }
            }
            recalculateDependencies(row, col);
        } catch (const exception& e) {
            // If any error occurs during parsing, create a string cell
            grid.at(row, col) = make_shared<StringValueCell>(content);
            recalculateDependencies(row, col);
        }
    }
}

//Retrieves a pointer to the cell at the specified row and column
shared_ptr<Cell> Spreadsheet::getCell(int row, int col) const {
    //Check if the provided row and column indices are within valid bounds
    if (row >= 0 && row < totalRows && col >= 0 && col < totalCols) {
        return grid.at(row, col); //Return the cell if it exists
    }
    //If out of bounds, return a nullptr indicating no valid cell
    return nullptr;
}

} // namespace GTUSpreadsheet
