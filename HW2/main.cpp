#include "AnsiTerminal.h"
#include "Spreadsheet.h"
#include "FileManager.h"
#include <memory> 
#include <iostream>

int main() {
    try {
        AnsiTerminal terminal;
        
        // Create initial spreadsheet with 21x8 dimensions
        auto sheet = GTUSpreadsheet::Spreadsheet::create(21, 8);
        if (!sheet) {
            throw std::runtime_error("Failed to create spreadsheet");
        }

        // Initialize FileManager with the spreadsheet instance
        Utils::FileManager fileManager(sheet);

        int selectedRow = 0, selectedCol = 0;
        int rowOffset = 0, colOffset = 0;

        // Draw the initial grid
        sheet->drawGrid(terminal, selectedRow, selectedCol, rowOffset, colOffset);

        char key;
        while (true) {
            key = terminal.getSpecialKey();

            if (key == 'q') {
                terminal.clearScreen();
                break;
            }

            // Handle navigation
            switch (key) {
                case 'U':
                    if (selectedRow > 0) selectedRow--;
                    break;
                case 'D':
                    selectedRow++;
                    // Resize if needed
                    if (selectedRow >= sheet->getTotalRows()) {
                        sheet->resizeGrid(selectedRow + 10, sheet->getTotalCols());
                    }
                    break;
                case 'R':
                    selectedCol++;
                    // Resize if needed
                    if (selectedCol >= sheet->getTotalCols()) {
                        sheet->resizeGrid(sheet->getTotalRows(), selectedCol + 5);
                    }
                    break;
                case 'L':
                    if (selectedCol > 0) selectedCol--;
                    break;
            }

            // Adjust visible area
            if (selectedRow - rowOffset >= sheet->getVisibleRows()) {
                rowOffset = selectedRow - sheet->getVisibleRows() + 1;
            } else if (selectedRow < rowOffset) {
                rowOffset = selectedRow;
            }

            if (selectedCol - colOffset >= sheet->getVisibleCols()) {
                colOffset = selectedCol - sheet->getVisibleCols() + 1;
            } else if (selectedCol < colOffset) {
                colOffset = selectedCol;
            }

            // Handle input and redraw
            sheet->handleInput(key, selectedRow, selectedCol, fileManager);
            sheet->drawGrid(terminal, selectedRow, selectedCol, rowOffset, colOffset);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}