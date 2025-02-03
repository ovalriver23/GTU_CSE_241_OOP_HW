#ifndef CELL_H
#define CELL_H

#include <string>
using namespace std;

//Enable each Cell to reference other cells within the same spreadsheet
//when dealing with formulas
class Spreadsheet; 

class Cell{
    public:
        //Enum to define the type of content in the cell
        enum class CellType{
            EMPTY,
            STRING,
            NUMBER,
            FORMULA
        };

        Cell(); //Default constructor

        //Setter and getter for cell content
        string getCellContent() const;
        void setCellContent (const string &content);

        //Setter and getter for the type of content
        Cell::CellType getCellType() const;
        void setCellType(CellType type);

        //Check if the cell content is a formula 
        bool isFormula() const;
        
        // Set the reference to the parent Spreadsheet (so Cell can potentially interact with the full spreadsheet)
        void setSpreadsheet(Spreadsheet *sheet);

        //Set the position 
        void setPosition(int row, int col);

        //Setters for raw content and computed content
        void setRawContent(const string &rawContent); 
        void setComputedContent(const string &computedContent);

        //Getters for raw content and computed content
        string getRawContent() const;
        string getComputedContent() const;

    private:
        string content; //raw content of the cell
        CellType type; //type of the content
        Spreadsheet *spreadsheet; //pointer to the spreadsheet that contains this cell(used for formula evaluations)
        string computedContent; // The computed content (result of a formula evaluation)
        int row;
        int col;
};

#endif
