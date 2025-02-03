#ifndef SPREADSHEET_H
#define SPREADSHEET_H
#include"AnsiTerminal.h"
#include"Cell.h" 
#include<string>
#include<vector>
using namespace std;
class FileManager;

class Spreadsheet{
    private:
        const int totalRows;        // Total rows in the grid
        const int totalCols;        // Total columns in the grid
        const int visibleRows = 21; // Number of visible rows
        const int visibleCols = 8;  // Number of visible columns
        const int cellWidth = 9;     // Width of each cell
        vector<vector<Cell>> grid;
    public:
        Spreadsheet(int row = 254, int col = 63); //Initializes the grid with the required number of rows and columns.

        int getTotalrows() const;
        int getTotalCols() const;
        int getVisibleRows() const;
        int getVisibleCols() const;

        // Converts column indices (like 0, 1, 2, ...) into spreadsheet labels (like A, B, C, ...).
        string getColumnLabel(int col);

        //Renders the spreadsheet grid in the terminal, showing the cells, and marking the selected row/column.
        void drawGrid(AnsiTerminal &terminal, int selectedRow, int selectedCol, int rowOffset, int colOffset);

        //Handles the user’s interaction with the spreadsheet.
        void handleInput(char key, int cur_row, int cur_col, FileManager &fileManager);

        // Allows access to a specific Cell in the grid so that its content or type can be accessed or modified.
        Cell &getCellValue(int row, int col);

        //This method takes an operand (e.g. "A1", "B2") and returns the content of that cell.
        string getCellContentByOperand(const string &operand);

        //This method sets the content (raw content) of a cell at the specified row and column.
        void setCellContent(int row, int col, const string &content);

        //Evaluates a formula that’s stored in a cell and updates the content accordingly
        void evaluateFormula(int row, int col);

        // Converts column labels like "A", "B" etc., to their corresponding index values.
        int columnToIndex(const string &column);

        //Resets the spreadsheet by clearing all the cell contents.
        void clear();    
};

#endif 