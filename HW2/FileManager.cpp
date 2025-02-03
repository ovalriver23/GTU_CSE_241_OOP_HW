#include "FileManager.h"
#include "Spreadsheet.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace Utils{

// Constructor: Initializes FileManager with a reference to the spreadsheet and no current file name
FileManager::FileManager(std::shared_ptr<GTUSpreadsheet::Spreadsheet> sheet)
    : spreadsheet(sheet), currentFileName("") {}

// Create a new file by clearing the spreadsheet and resetting the file name
void FileManager::makeNewFile() {
    if (!spreadsheet) {
        throw std::runtime_error("Spreadsheet instance is not initialized.");
    }
    spreadsheet->clear();  // Clears all content in the spreadsheet
    currentFileName = "";  // Resets the current file name
}

// Save the spreadsheet to the current file if a file name is specified
void FileManager::saveFile() {
    if (currentFileName.empty()) {
        throw std::runtime_error("No file name specified. Use 'Save As' to set a file name.");
    }
    saveFileAs(currentFileName); // Call saveFileAs with the current file name
}

// Save the spreadsheet to a specified file (CSV format)
void FileManager::saveFileAs(const std::string& fileName) {
    if (!spreadsheet) {
        throw std::runtime_error("Spreadsheet instance is not initialized.");
    }

    std::ofstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file for writing.");
    }

    try {
        // Iterate through each cell in the spreadsheet and write its content to the file
        for (int i = 0; i < spreadsheet->getTotalRows(); ++i) {
            for (int j = 0; j < spreadsheet->getTotalCols(); ++j) {
                auto cell = spreadsheet->getCell(i, j);
                if (cell) {
                    file << cell->getContent(); // Write the content of the cell
                }
                if (j < spreadsheet->getTotalCols() - 1) {
                    file << ",";  // Add a comma between cell values
                }
            }
            file << "\n";  // Add a newline after each row
        }
    } catch (const std::exception& e) {
        file.close();
        throw std::runtime_error("Error while writing to file: " + std::string(e.what()));
    }

    file.close();
    currentFileName = fileName; // Set the current file name to the specified file name
}

// Load a spreadsheet from a specified file (CSV format)
// Load a spreadsheet from a specified file (CSV format) with proper resizing
void FileManager::loadFile(const std::string& fileName) {
    if (!spreadsheet) {
        throw std::runtime_error("Spreadsheet instance is not initialized.");
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file for reading: " + fileName);
    }

    try {
        std::cout << "Starting to load file: " << fileName << std::endl;
        spreadsheet->clear();

        std::string line;
        int maxRow = 0, maxCol = 0;

        // First pass - Determine grid dimensions
        while (std::getline(file, line)) {
            // Trim any trailing whitespace or carriage returns
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
                line.pop_back();
            }
            
            if (line.empty()) continue;

            std::istringstream ss(line);
            std::string cellContent;
            int col = 0;
            
            while (std::getline(ss, cellContent, ',')) {
                // Trim any whitespace from the cell content
                while (!cellContent.empty() && (cellContent.back() == ' ' || cellContent.back() == '\r')) {
                    cellContent.pop_back();
                }
                col++;
            }
            maxRow++;
            maxCol = std::max(maxCol, col);
        }

        std::cout << "Detected dimensions: " << maxRow << " rows x " << maxCol << " columns" << std::endl;
        
        // Reset file pointer and clear any error flags
        file.clear();
        file.seekg(0);

        // Resize the grid
        spreadsheet->resizeGrid(maxRow, maxCol);

        // Second pass - Load the data
        int row = 0;
        while (std::getline(file, line)) {
            // Trim trailing whitespace or carriage returns
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
                line.pop_back();
            }
            
            if (line.empty()) continue;

            std::istringstream ss(line);
            std::string cellContent;
            int col = 0;

            while (std::getline(ss, cellContent, ',')) {
                // Trim whitespace
                while (!cellContent.empty() && (cellContent.back() == ' ' || cellContent.back() == '\r')) {
                    cellContent.pop_back();
                }
                
                if (!cellContent.empty() && row < maxRow && col < maxCol) {
                    try {
                        spreadsheet->setCellContent(row, col, cellContent);
                    } catch (const std::exception& e) {
                        std::cerr << "Warning: Failed to set content at (" << row << "," << col 
                                << "): " << e.what() << std::endl;
                    }
                }
                col++;
            }
            row++;
        }

        std::cout << "File loaded successfully" << std::endl;

    } catch (const std::exception& e) {
        file.close();
        throw std::runtime_error("Error while reading from file: " + std::string(e.what()));
    }

    file.close();
    currentFileName = fileName;
}


// Get the current file name (if a file is loaded or saved)
std::string FileManager::getCurrentFileName() const {
    return currentFileName; // Return the current file name
}
}