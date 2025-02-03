#include "FormulaParser.h"
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

// Constructor: Initialize FormulaParser with reference to the spreadsheet
FormulaParser::FormulaParser(Spreadsheet& sheet) : spreadsheet(sheet) {}

// Parse a formula string, removing the '=' or '@' if present
double FormulaParser::parseFormula(const string &formula){
    if(formula.empty()) return 0; // If the formula is empty, return 0

    if(formula[0] == '@') return evaluateFunction(formula);// Handle function formulas (e.g., @SUM)

    string cleanFormula;
    if (formula[0] == '=') {
        cleanFormula = formula.substr(1); // Remove leading '=' for formula
    } else {
        cleanFormula = formula;
    }

    vector<string> tokens = tokenize(cleanFormula); // Tokenize the formula
    return evaluateExpression(tokens);  // Evaluate the expression from tokens
}

// Handle function formulas 
double FormulaParser::evaluateFunction(const string &formula){
    int openParen = formula.find('(');  // Find opening parenthesis
    int closeParen = formula.find(')'); // Find closing parenthesis

    string functionName = formula.substr(1, openParen - 1); // Get the function name
    string arguments = formula.substr(openParen + 1, closeParen - openParen - 1); // Get the arguments

    int rangeSep = arguments.find(".."); // Find the range separator
    
    string startCell = arguments.substr(0, rangeSep);  // Get the start cell reference
    string endCell = arguments.substr(rangeSep + 2); // Get the end cell reference

    int startRow, startCol, endRow, endCol;

    // Convert cell references to row and column indices
    if (!convertCellReference(startCell, startRow, startCol) || !convertCellReference(endCell, endRow, endCol)) {
        cerr << "Error: Invalid cell references in range" << endl;
        return 0;
    }

    // Call the appropriate function based on the function name
    if(functionName == "Sum" || functionName == "sum"){
        return calculateSum(startRow, startCol, endRow, endCol);
    }else if(functionName == "Aver" || functionName == "aver"){
        return calculateSum(startRow, startCol, endRow, endCol) / countCells(startRow, startCol, endRow, endCol);
    }else if(functionName == "Max" || functionName == "max"){
        return calculateMax(startRow, startCol, endRow, endCol);
    }else if(functionName == "Min" || functionName == "min"){
        return calculateMin(startRow, startCol, endRow, endCol);
    }else if(functionName == "Stddev" || functionName == "stddev"){
        return calculateStddev(startRow, startCol, endRow, endCol);
    }else {
        return 0;
    }
}

// Convert a cell reference to row and column indices
bool FormulaParser::convertCellReference(const string &ref, int &row, int &col){
    string colPart = "";
    string rowPart = "";

    // Separate the column and row parts
    for(char ch : ref){
        if(isalpha(ch)){
            colPart += ch; // Append alphabetic characters to column part
        }else if(isdigit(ch)){
            rowPart += ch; // Append numeric characters to row part
        }
    }  

    if (colPart.empty() || rowPart.empty()) return false; // Invalid reference if no column or row part

    col = spreadsheet.columnToIndex(colPart); // Convert column label to index
    row = stoi(rowPart) - 1; // Convert row string to integer and adjust for zero-based index

    return row >= 0 && col >= 0;  // Return true if valid row/col indices
}


// Check if a token is a valid cell reference
bool FormulaParser::isCellReference(const string& token) {
    if (token.empty()) return false;

    int i = 0;

    // Check if the first part of the token is alphabetic (column part)
    while (i < token.size() && isalpha(token[i])) {
        ++i;
    }

    // Ensure there's at least one digit after the alphabetic part
    if (i == 0 || i >= token.size() || !isdigit(token[i])) {
        return false;
    }

    // Check the rest of the token for numeric characters
    while (i < token.size()) {
        if (!isdigit(token[i])) {
            return false;
        }
        ++i;
    }

    return true;
}


// Calculate the sum of values in a given range of cells
double FormulaParser::calculateSum(int startRow, int startCol, int endRow, int endCol){
    double sum = 0;
    for(int r = startRow; r <= endRow; ++r){
        for (int c = startCol; c <= endCol; ++c) {
            string content = spreadsheet.getCellValue(r, c).getCellContent();
            if(isNumber(content)){
                sum += stod(content);
            }
        }
    }
    return sum;
}


// Calculate the maximum value in a given range of cells
double FormulaParser::calculateMax(int startRow, int startCol, int endRow, int endCol) {
    double maxVal = -1000000000; 
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            string content = spreadsheet.getCellValue(r, c).getCellContent();
            if (isNumber(content)) {
                maxVal = max(maxVal, stod(content));
            }
        }
    }
    return maxVal;
}


// Calculate the minimum value in a given range of cells
double FormulaParser::calculateMin(int startRow, int startCol, int endRow, int endCol) {
    double minVal = 1000000000; 
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            string content = spreadsheet.getCellValue(r, c).getCellContent();
            if (isNumber(content)) {
                minVal = min(minVal, stod(content));
            }
        }
    }
    return minVal;
}


// Calculate the standard deviation in a given range of cells
double FormulaParser::calculateStddev(int startRow, int startCol, int endRow, int endCol){
    double sum = 0, sumSq = 0;
    int count = 0;

    for(int r = startRow; r <= endRow; ++r){
        for(int c = startCol; c <=endCol; ++c){
            string content = spreadsheet.getCellValue(r, c).getCellContent();
            if(isNumber(content)){
                double val = stod(content);
                sum += val;
                sumSq += val * val;
                ++count;
            }
        }
    }
    if(count == 0) return 0;

    double mean = sum / count;
    return sqrt(sumSq / count - mean * mean);
}


// Count the number of non-empty cells in a given range
int FormulaParser::countCells(int startRow, int startCol, int endRow, int endCol) {
    int count = 0;
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            if (!spreadsheet.getCellValue(r, c).getCellContent().empty()) {
                ++count;
            }
        }
    }
    return count;
}


// Tokenize the formula string into individual components
vector<string> FormulaParser::tokenize(const string& formula) {
    vector<string> tokens;
    string token;

    // Loop through the formula and separate it into tokens
    for (char ch : formula) {
        if (isspace(ch)) {
            if (!token.empty()) {
                tokens.push_back(token);  // Add token to list
                token.clear();
            }
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            if (!token.empty()) {
                tokens.push_back(token);  // Add token to list
                token.clear();
            }
            tokens.push_back(string(1, ch));  // Add operator as token
        } else {
            token += ch; // Build token
        }
    }

    if (!token.empty()) {
        tokens.push_back(token); // Add final token
    }

    return tokens;
}

// Evaluate a mathematical expression based on the tokens
double FormulaParser::evaluateExpression(const vector<string>& tokens) {
    double result = 0;
    string currentOp = "";

     // Loop through the tokens and perform the operations
    for (const auto& token : tokens) {
        if (isOperator(token)) {
            currentOp = token; // Update operator
        } else {
            double value = getNumericValue(token); // Get numeric value for token
            if (currentOp.empty()) {
                result = value; // Set result to the first value
            } else {
                result = applyOperator(result, value, currentOp); // Apply the operator
            }
        }
    }

    return result;
}

// Check if the token is a valid operator (+, -, *, /)
bool FormulaParser::isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

// Check if the token is a valid number
bool FormulaParser::isNumber(const string &token){
    if (token.empty()) return false;

    int i = 0;
    if (token[i] == '+' || token[i] == '-') ++i;

    bool decimalPoint = false;
    while (i < token.size()) {
        if (isdigit(token[i])) {
            ++i;
        } else if (token[i] == '.' && !decimalPoint) {
            decimalPoint = true;
            ++i;
        } else {
            return false;
        }
    }

    return true;
}

// Get the numeric value of a token, either from a direct number or a cell reference
double FormulaParser::getNumericValue(const string &token){
    if(isNumber(token)){
        return stod(token);  // Return number if token is numeric
    }else if(isCellReference(token)){
        string cellContent = spreadsheet.getCellContentByOperand(token);
        if(isNumber(cellContent)){
            return stod(cellContent); // Return value from referenced cell if it's numeric
        }else {
            return 0;  // Return 0 if the cell content is not numeric
        }
    }
    return 0; // Return 0 if the token is not valid
}


// Get the precedence of an operator 
int FormulaParser::getPrecedence(const std::string& op) {
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

// Apply an operator to two operands and return the result
double FormulaParser::applyOperator(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) {
            return 0; // Prevent division by zero
        }
        return a / b;
    }
    return 0; // Default return if operator is unrecognized
}