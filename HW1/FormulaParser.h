#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

#include"Spreadsheet.h"
#include<vector>
#include<string>
using namespace std;

class FormulaParser{
    public:
        explicit FormulaParser(Spreadsheet &sheet);
        double parseFormula(const string &formula);
    private:
        Spreadsheet spreadsheet;
        vector<string> tokenize(const string &formula); //Breaks the formula into smaller components
        double evaluateExpression(const vector<string> &tokens); //Takes a vector of tokens and evaluates the mathematical expression.
        bool isOperator(const string &token); //Checks if a given token is a valid operator
        bool isCellReference(const string &token);//Checks if a given token is a valid cell reference
        bool isNumber(const string &token); //Checks if a given token is a valid number.
        double getNumericValue(const string &token); //Converts a string token into a numeric value.
        int getPrecedence(const string &op); //Returns the precedence of operators (e.g., * and / have higher precedence than + and -).
        double applyOperator(double a, double b, const string &op); //Applies an operator (e.g., +, -, *, /) to two operands and returns the result.

        double evaluateFunction(const string& formula);//Evaluates special functions 
        bool convertCellReference(const string& ref, int& row, int& col); //Converts a cell reference to row and column indices
       
        //These methods handle the logic for calculating sums, maximum values, minimum values, and standard deviations over a range of cells.
        double calculateSum(int startRow, int startCol, int endRow, int endCol);
        double calculateMax(int startRow, int startCol, int endRow, int endCol);
        double calculateMin(int startRow, int startCol, int endRow, int endCol);
        double calculateStddev(int startRow, int startCol, int endRow, int endCol);
        //These methods handle the logic for calculating sums, maximum values, minimum values, and standard deviations over a range of cells.
        int countCells(int startRow, int startCol, int endRow, int endCol);
};

#endif
