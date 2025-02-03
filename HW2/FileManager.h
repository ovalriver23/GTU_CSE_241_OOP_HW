#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <memory>



namespace GTUSpreadsheet {
class Spreadsheet;
}

namespace Utils{

class FileManager {
private:
    std::shared_ptr<GTUSpreadsheet::Spreadsheet> spreadsheet; // Pointer to the spreadsheet instance
    std::string currentFileName;                              // Name of the current file

public:
    // Constructor
    explicit FileManager(std::shared_ptr<GTUSpreadsheet::Spreadsheet> sheet);

    // Create a new file
    void makeNewFile();

    // Save the spreadsheet to the current file
    void saveFile();

    // Save the spreadsheet to a specified file
    void saveFileAs(const std::string& fileName);

    // Load a spreadsheet from a specified file
    void loadFile(const std::string& fileName);

    // Get the current file name
    std::string getCurrentFileName() const;
};
}

#endif // FILEMANAGER_H
