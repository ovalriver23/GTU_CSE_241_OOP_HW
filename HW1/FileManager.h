#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "Spreadsheet.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

class FileManager{
    private:
        Spreadsheet &spreadsheet;
        string currentFileName;
    public:
        FileManager(Spreadsheet &sheet); //Constructor
        void makeNewFile(); //Create a new file by clearing the spreadsheet
        void saveFile(); //Save the spreadsheet to the current file
        void saveFileAs(const string &fileName); //Save the spreadsheet to a specified file
        void loadFile(const string &fileName); //load a spreadsheet from a specific file
        string getCurrentFileName() const; //get the name of the current file
};

#endif