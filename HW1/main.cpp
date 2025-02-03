#include"AnsiTerminal.h"
#include"Spreadsheet.h"
#include"FileManager.h"
#include<iostream>
using namespace std;

int main(){
    AnsiTerminal terminal;
    Spreadsheet sheet(254, 63); //Initialize spreadsheet with 254 rows and 63 columns
    FileManager fileManager(sheet); // Pass the spreadsheet instance to file manager for file operations
    int selectedRow = 0, selectedCol = 0;
    int visibleRow = 0, visibleCol = 0;

    // Draw the initial grid on the terminal
    sheet.drawGrid(terminal, selectedRow, selectedCol, visibleRow, visibleCol);

    char key;

    while(true){

        key = terminal.getSpecialKey(); 

        //switch statement that checks the value of key and updates the selectedRow and selectedCol accordingly.
        switch(key){
            case 'U':
                if(selectedRow > 0) selectedRow -= 1;
                break;
            case 'D':
                if(selectedRow < sheet.getTotalrows() - 1) selectedRow += 1;
                break;
            case 'R':
                if(selectedCol < sheet.getTotalCols() - 1) selectedCol += 1;
                break;
            case 'L':
                if(selectedCol > 0) selectedCol -= 1;
                break;
            case 'q':
                terminal.clearScreen();
                return 0;
        }


        // If the selected row is above the current viewport, adjust the visibleRow to bring it into view
        if (selectedRow < visibleRow) {
            visibleRow = selectedRow;
        } 
        // If the selected row is below the current viewport, adjust the visibleRow to bring it into view
        else if (selectedRow >= visibleRow + sheet.getVisibleRows()) {
            visibleRow = selectedRow - sheet.getVisibleRows() + 1; // Ensure the row is fully visible
        }

        // If the selected column is to the left of the current visible area, update visibleCol to bring it into view
        if (selectedCol < visibleCol) {
            visibleCol = selectedCol;
        } 
        // If the selected column is to the right of the current visible area, adjust colOffset to bring it into view
        else if (selectedCol >= visibleCol + sheet.getVisibleCols()) {
            visibleCol = selectedCol - sheet.getVisibleCols() + 1; // Ensure the column is fully visible
        }

        sheet.handleInput(key, selectedRow, selectedCol, fileManager);
        sheet.drawGrid(terminal, selectedRow, selectedCol, visibleRow, visibleCol); // Redraw the grid with updated state

    }
    terminal.clearScreen();
    return 0;
}
    
