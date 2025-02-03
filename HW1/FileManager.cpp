#include "FileManager.h"

// Constructor: Initializes FileManager with reference to the spreadsheet and no current file name
FileManager::FileManager(Spreadsheet &sheet): spreadsheet(sheet), currentFileName(""){}

// Create a new file by clearing the spreadsheet and resetting the file name
void FileManager::makeNewFile(){
    spreadsheet.clear();  // Clears all the content in the spreadsheet
    currentFileName = ""; // Resets the current file name
}

// Save the spreadsheet to the current file if a file name is specified
void FileManager::saveFile(){
    if(currentFileName.empty()){
        cerr << "No file name specified. Use 'Save As' to set a file name.\n"; // Error message if file name is not set
        return;
    }
    saveFileAs(currentFileName); // Call saveFileAs with the current file name
}

// Save the spreadsheet to a specified file (CSV format)
void FileManager::saveFileAs(const string &fileName){
    ofstream file(fileName); // Open the file for writing
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing.\n"; // // Error message if file cannot be opened
        return;
    }

    // // Iterate through each cell in the spreadsheet and write its content to the file
    for (int i = 0; i < spreadsheet.getTotalrows(); ++i) {
        for (int j = 0; j < spreadsheet.getTotalCols(); ++j) {
            file << spreadsheet.getCellValue(i, j).getRawContent(); //Write the raw content of each cell
            if (j < spreadsheet.getTotalCols() - 1) {
                file << ",";  // Add a comma between cell values
            }
        }
        file << "\n";  // Add a newline after each row
    }

    file.close(); 
    currentFileName = fileName; // Set the current file name to the specified file name
}

// Load a spreadsheet from a specified file (CSV format)
void FileManager::loadFile(const string &fileName){
    ifstream file(fileName); // Open the file for reading
    if(!file.is_open()){
        cerr<<"Error: Could not open file for reading.\n";
        return;
    }

    spreadsheet.clear();

    string line;
    int row = 0;
    while(getline(file, line)){ // Read each line of the file
        istringstream ss(line);
        string cellContent;
        int col = 0;
        while (getline(ss, cellContent, ',')){ // Read each cell in the row
            spreadsheet.setCellContent(row, col, cellContent); // Set the content of each cell
            ++col;
        }
        ++row;
    }

    file.close();
    currentFileName = fileName;
}

// Get the current file name (if a file is loaded or saved)
string FileManager::getCurrentFileName() const {
    return currentFileName; // Return the current file name
}
