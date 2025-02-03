#include "Cell.h"
#include "FormulaParser.h"
#include <sstream>

Cell::Cell() : content(""), type(CellType::EMPTY), spreadsheet(nullptr), row(-1), col(-1) {
    // Default constructor initializes cell with empty contenta and type as EMPTY
}

// Set the content of the cell (raw content)
void Cell::setCellContent(const string &cont) {
    content = cont;
}

// Get the current type of the cell
Cell::CellType Cell::getCellType() const {
    return type;
}

// Set the type of the cell 
void Cell::setCellType(CellType Ptype) {
    type = Ptype;
}

// Associate this cell with a specific spreadsheet
void Cell::setSpreadsheet(Spreadsheet *sheet) {
    spreadsheet = sheet;
}

// Set the position of the cell in terms of row and column
void Cell::setPosition(int r, int c) {
    row = r;
    col = c;
}

// Check if the cell content is a formula 
bool Cell::isFormula() const {
    return !content.empty() && content[0] == '=';
}

// Set the raw content (string or formula) of the cell
void Cell::setRawContent(const string &rawContent) {
    content = rawContent;
}

// Set the computed result (e.g., after formula evaluation) for the cell
void Cell::setComputedContent(const string &computedCont) {
    computedContent = computedCont;
}

// Get the raw content (original input) of the cell
string Cell::getRawContent() const {
    return content;
}

// Get the computed content (result of formula evaluation) of the cell
string Cell::getComputedContent() const {
    return computedContent;
}

// Get the current content of the cell: either raw content or computed result
string Cell::getCellContent() const {
    // If there's computed content, return it
    if (!computedContent.empty()) return computedContent;

    // If the cell is a formula and it has an associated spreadsheet, evaluate it
    if (isFormula() && spreadsheet != nullptr) {
        FormulaParser parser(*spreadsheet);
        double result = parser.parseFormula(content); // Parse and evaluate the formula

        ostringstream oss;
        oss.precision(6);  // Set precision for the result
        oss << result;
        return oss.str();  // Return the evaluated formula result as a string
    }

    // If no formula, return the raw content 
    return content;
}
