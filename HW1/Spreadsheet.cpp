#include "Spreadsheet.h"
#include "AnsiTerminal.h"
#include "FormulaParser.h"
#include "FileManager.h"
#include "Cell.h"
#include <string>
#include <vector>
#include<iostream>
#include <iomanip>
#include<sstream>
using namespace std;

Spreadsheet::Spreadsheet(int row, int col) 
    : totalRows(row), totalCols(col) {
    // Initialize the grid with the specified number of rows and columns
    grid = vector<vector<Cell>>(totalRows, vector<Cell>(totalCols));
    
    // Set position and link each cell to the spreadsheet
    for (int i = 0; i < totalRows; i++) {
        for (int j = 0; j < totalCols; j++) {
            grid[i][j].setPosition(i, j); 
            grid[i][j].setSpreadsheet(this); 
        }
    }
}

// Convert column index to column label (e.g., 0 -> A, 1 -> B, 26 -> AA)
string Spreadsheet::getColumnLabel(int col){
    string colLabel;
    if(col < 26){
        colLabel += 'A' + col;
    }else {
        colLabel += 'A' + (col / 26 - 1);
        colLabel += 'A' + (col % 26);
    }
    return colLabel;
}

// Draw the grid on the terminal, including the selected cell and input area
void Spreadsheet::drawGrid(AnsiTerminal &terminal, int selectedRow, int selectedCol, int rowOffset, int colOffset){
    terminal.clearScreen();

    string currentCellInfo = getColumnLabel(selectedCol) + to_string(selectedRow + 1);
    string typeDisplay;
    string cellContent = grid[selectedRow][selectedCol].getCellContent();

    // Display the type of content (Label, Value)
    if(grid[selectedRow][selectedCol].getCellType() == Cell::CellType::STRING){
        typeDisplay = "(L)";
    }else if(grid[selectedRow][selectedCol].getCellType() == Cell::CellType::NUMBER){
        typeDisplay = "(V)";
    }else if(grid[selectedRow][selectedCol].getCellType() == Cell::CellType::FORMULA){
        typeDisplay = "(F)";
    }else {
        typeDisplay = "";
    }

    // Display the first line with selected cell's info
    string paddedFirstLine = currentCellInfo + " " + typeDisplay + " " + cellContent;

    while (paddedFirstLine.length() < 75) {
        paddedFirstLine += ' '; 
    } 
    terminal.printInvertedAt(0, 1, paddedFirstLine);

    // Display the raw content of the selected cell
    string inputDisplay = grid[selectedRow][selectedCol].getRawContent();
    string paddedInputDisplay = inputDisplay;
    while (paddedInputDisplay.length() < 75) {
        paddedInputDisplay += ' '; 
    }
    terminal.printAt(3, 1, paddedInputDisplay);

     // Display the type of the selected cell (Label, Value)
    string typeDisplayLine;
    if (grid[selectedRow][selectedCol].getCellType() == Cell::CellType::STRING) {
        typeDisplayLine = "Label";
    } else if (grid[selectedRow][selectedCol].getCellType() == Cell::CellType::NUMBER) {
        typeDisplayLine = "Value";
    }else if (grid[selectedRow][selectedCol].getCellType() == Cell::CellType::FORMULA) {
            typeDisplayLine = "Formula";
    }else {
        typeDisplayLine = cellContent;
    }
    string paddedTypeDisplay = typeDisplayLine;
    while (paddedTypeDisplay.length() < 75) {
        paddedTypeDisplay += ' ';
    }

    terminal.printInvertedAt(2, 1, paddedTypeDisplay); 

    int gridStartRow = 5;

    // Print the top-left intersection square
    terminal.printInvertedAt(gridStartRow - 1, 1, string(cellWidth, ' '));


    // Printing columns labels
    for(int col = 0; col < visibleCols; ++col){
        int actualCol = col + colOffset;
        if(actualCol >= totalCols) break;

        string colLabel = getColumnLabel(actualCol);
        int paddingLeft = (cellWidth - colLabel.size()) / 2;
        int paddingRight = cellWidth - colLabel.size() - paddingLeft; 


        string paddedColLabel = "";
        for (int i = 0; i < paddingLeft; ++i) {
            paddedColLabel += ' '; 
        }
        paddedColLabel += colLabel;
        for (int i = 0; i < paddingRight; ++i) {
            paddedColLabel += ' '; 
        }

        terminal.printInvertedAt(gridStartRow - 1, 4 + col * cellWidth, paddedColLabel);
    }





    // Printing row labels and cell contents
    for(int row = 0; row < visibleRows; ++row){
        int actualRow = row + rowOffset;
        if(actualRow >= totalRows) break;


        string rowLabel = to_string(actualRow + 1);
        string paddedRowLabel = rowLabel;
        while (paddedRowLabel.size() < cellWidth - 1) {
                paddedRowLabel += ' ';
        }

        paddedRowLabel += ' '; 

        terminal.printInvertedAt(gridStartRow + row, 1, paddedRowLabel);
        // Printing cell contents
        for(int col = 0; col < visibleCols; ++col){
            int actualCol = col + colOffset; 
            if (actualCol >= totalCols) break;

            int cellRow = gridStartRow + row; 
            int cellCol = 4 + col * cellWidth;

            string temp = grid[actualRow][actualCol].getCellContent();


            string displayContent;
            if (temp.length() > cellWidth) {
                displayContent = temp.substr(0, cellWidth);
            } else {
                displayContent = temp;
            }


            string paddedContent = displayContent;
            while (paddedContent.size() < cellWidth) {
                paddedContent += ' ';
            }

            if (actualRow == selectedRow && actualCol == selectedCol) {
                terminal.printInvertedAt(cellRow, cellCol, paddedContent); // Highlight selected cell
            } else {
                if (!temp.empty()) {
                    terminal.printAt(cellRow, cellCol, paddedContent); // Print non-empty cell
                } else {
                    string emptyCell(cellWidth, ' '); // Clear the cell with spaces
                    terminal.printAt(cellRow, cellCol, emptyCell);
                }
            }
        }
    }
}


// Handle user input such as navigation and file operations
void Spreadsheet::handleInput(char key, int cur_row, int cur_col, FileManager &fileManager) {
    string temp = grid[cur_row][cur_col].getRawContent();

    // Handle the menu input when '\' is pressed
    if(key == '\\'){
        AnsiTerminal terminal;
        terminal.clearScreen();

        terminal.printAt(1, 1, "Menu:");
        terminal.printAt(2, 1, "1. Save File");
        terminal.printAt(3, 1, "2. Load File");
        terminal.printAt(4, 1, "3. Cancel");
        terminal.printAt(5, 1, "Enter your choice: ");

        char choice = terminal.getKeystroke();
        switch (choice) {
            case '1': {
                if (fileManager.getCurrentFileName().empty()) {
                    terminal.printAt(6, 1, "Enter file name to save as: ");
                    string fileName;
                    cin >> fileName;
                    terminal.clearScreen();
                    fileManager.saveFileAs(fileName);
                } else {
                    fileManager.saveFile();
                }
                break;
            }
            case '2': {
                terminal.printAt(6, 1, "Enter file name to load: ");
                string fileName;
                cin >> fileName;
                fileManager.loadFile(fileName);
                break;
            }
            case '3':
                break;
            default:
                break;
        }
        terminal.clearScreen(); // Clear the terminal after the menu
        drawGrid(terminal, cur_row, cur_col, 0, 0); // Redraw the grid after the menu
        return;
    }
 // Handle backspace to remove character
    if (key == '\b' || key == 127 || key == 8) {
        if (!temp.empty()) {
            temp.pop_back(); 
            grid[cur_row][cur_col].setCellContent(temp);
        }
    // Handle regular character input
    }else if (key != 'U' && key != 'D' && key != 'R' && key != 'L') {
        temp.push_back(key);
        grid[cur_row][cur_col].setCellContent(temp); 

        // Handle formula input with '=' or '@'
        if (temp[0] == '=' || temp[0] == '@') {
            grid[cur_row][cur_col].setCellType(Cell::CellType::FORMULA);

            if (temp[0] == '=' && temp.length() > 1) {
                evaluateFormula(cur_row, cur_col); 
            } else if (temp[0] == '@' && temp.find(')') != string::npos) {
                FormulaParser parser(*this);
                double functionResult = parser.parseFormula(temp); 

                ostringstream formattedResult;
                formattedResult << fixed << setprecision(2) << functionResult;

                grid[cur_row][cur_col].setRawContent(temp); 
                grid[cur_row][cur_col].setComputedContent(formattedResult.str()); 
            }
        }
        // Handle number or string input
        else {
            istringstream iss(temp);
            double number;
            if (iss >> number && iss.eof()) {
                grid[cur_row][cur_col].setCellType(Cell::CellType::NUMBER); 
            }
            else {
                grid[cur_row][cur_col].setCellType(Cell::CellType::STRING); 
            }
        }
    }
}

// Get the value of a specific cell in the grid
Cell& Spreadsheet::getCellValue(int row, int col) {
    return grid[row][col]; 
}

// Set the content of a specific cell and update its type 
void Spreadsheet::setCellContent(int row, int col, const string &content) {
    grid[row][col].setCellContent(content);
    
    if (grid[row][col].isFormula()) {
        grid[row][col].setCellType(Cell::CellType::FORMULA);
    } else {
        istringstream iss(content);
        double number;
        if (iss >> number && iss.eof()) {
            grid[row][col].setCellType(Cell::CellType::NUMBER);
        } else {
            grid[row][col].setCellType(Cell::CellType::STRING);
        }
    }
}


// Convert a column label (e.g., "A", "B", "AA") to its corresponding index
int Spreadsheet::columnToIndex(const string &column) {
    int index = 0;
    for (int i = 0; i < column.size(); ++i) {
        index = index * 26 + (column[i] - 'A' + 1);
    }
    return index - 1; 
}

// Get the content of a cell based on the operand (e.g., "A1", "B2")
string Spreadsheet::getCellContentByOperand(const string &operand) {
    string column = ""; 
    string row_str = ""; 

    for (char ch : operand) {
        if (isalpha(ch)) {
            column += ch; 
        } else if (isdigit(ch)) {
            row_str += ch; 
        }
    }

    int row = stoi(row_str) - 1; 
    int col = columnToIndex(column); 

    if (row >= 0 && col >= 0 && row < grid.size() && col < grid[0].size()) {
        return grid[row][col].getCellContent(); 
    }

    return "INVALID";
}

// Evaluate a formula and set the result to the cell
void Spreadsheet::evaluateFormula(int row, int col) {
    string content = grid[row][col].getCellContent();
    if (content.empty() || content[0] != '=') return;
    
    FormulaParser parser(*this);
    double result = parser.parseFormula(content);
    grid[row][col].setCellContent(to_string(result));
}

// Get the total number of rows in the spreadsheet
int Spreadsheet::getTotalrows() const {
    return totalRows;
}
// Get the total number of columns in the spreadsheet
int Spreadsheet::getTotalCols() const {
    return totalCols;
}
// Get the number of visible rows in the terminal display
int Spreadsheet::getVisibleRows() const {
    return visibleRows;
}
// Get the number of visible columns in the terminal display
int Spreadsheet::getVisibleCols() const {
    return visibleCols;
}

// Clear all content in the spreadsheet
void Spreadsheet::clear() {
    for (int i = 0; i < totalRows; ++i) {
        for (int j = 0; j < totalCols; ++j) {
            grid[i][j].setCellContent("");             
            grid[i][j].setCellType(Cell::CellType::EMPTY); 
        }
    }
}
