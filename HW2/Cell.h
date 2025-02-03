#ifndef CELL_H
#define CELL_H

// Header file defining cell classes for a spreadsheet implementation
// Supports basic values, strings, numbers and formula calculations

#include <iostream>
#include <string>
#include <memory>
#include <iomanip>
#include <cmath>
#include<stdexcept>
#include "Spreadsheet.h"
#include "Custom1DArray.h"

using namespace std;

namespace GTUSpreadsheet {
    class Spreadsheet; //Forward declaration
}

class Cell{
    public:
        Cell();
        //Core interface for getting/setting cell content
        virtual string getContent() const = 0; // Returns formatted cell content
        virtual void setContent(const string &cont) = 0; // Sets cell content
        virtual string getRawContent() const = 0; // Returns unformatted content

        // Position management
        void setPosition(int r, int c); // Sets cell position in spreadsheet
        int getRow() const;             // Gets row number
        int getCol() const;             // Gets column number

        void setSpreadsheet(shared_ptr<GTUSpreadsheet::Spreadsheet> sheet);

        virtual ~Cell() = default;

    protected:
        string content;  // Raw content storage
        int row, col;    // Cell position
        shared_ptr<GTUSpreadsheet::Spreadsheet> spreadsheet; // Parent spreadsheet
};

// Base class for cells containing simple values
class ValueCell : public Cell {
protected:
    string value;
public:
    explicit ValueCell(const string &initialContent = "");
    string getRawContent() const override { return content; }
    string getContent() const override;
    void setContent(const string &kontent) override;
};

// Specialized cell for string values
class StringValueCell : public ValueCell {
public:
    explicit StringValueCell(const string& initialContent = "");
};

// Specialized cell for integer values
class IntValueCell : public ValueCell {
public:
    explicit IntValueCell(int initialValue = 0);
    void setContent(const string &content) override;
    string getContent() const override;
private:
    int intValue; // Stores parsed integer value
};

// Specialized cell for floating-point values
class DoubleValueCell : public ValueCell {
public:
    explicit DoubleValueCell(double initialValue = 0.0);
    void setContent(const string& content) override;
    string getContent() const override;
private:
    double doubleValue; // Stores parsed double value
};


// Complex cell type supporting formula evaluation
class FormulaCell : public Cell {
private:
    string formula;  // Stores the raw formula
    string computedValue;  // Cached computed result
    shared_ptr<GTUSpreadsheet::Spreadsheet> spreadsheet;
    DynamicArray<pair<int, int>> dependencies;  // Tracks cell dependencies

    // Formula parsing methods
    bool isFunction(const string& content) const; // Checks if content is a function (SUM, AVG etc)
    pair<string, string> parseFunctionAndRange(const string& content) const; // Splits function and range
    pair<string, string> parseRange(const string& range) const; // Parses cell range (e.g. A1:B5)
    DynamicArray<double> getValuesFromRange(const string& start, const string& end) const; // Gets values from range

    // Mathematical functions
    double calculateSum(const DynamicArray<double>& values) const;
    double calculateAverage(const DynamicArray<double>& values) const;
    double calculateMax(const DynamicArray<double>& values) const;
    double calculateMin(const DynamicArray<double>& values) const;
    double calculateStdDev(const DynamicArray<double>& values) const;

    // Expression evaluation helpers
    void tokenize(const string& formula, DynamicArray<string>& tokens) const; // Splits formula into tokens
    double evaluateExpression(const DynamicArray<string>& tokens) const; // Evaluates expression
    double fetchValueFromReference(const string& reference) const; // Gets value from cell reference
    bool isOperator(const string& token) const; // Checks if token is an operator
    bool isCellReference(const string& token) const; // Checks if token is cell reference
    bool isNumber(const string& token) const; // Checks if token is numeric
    double applyOperator(double a, double b, const string& op) const; // Applies arithmetic operator

public:
    // Constructs a formula cell with initial formula and reference to parent spreadsheet
    FormulaCell(const string& formula, shared_ptr<GTUSpreadsheet::Spreadsheet> spreadsheet);
    string getContent() const override; // Returns computed result
    string getRawContent() const override; // Returns raw formula
    void setContent(const string& content) override; // Sets a new formula and triggers recalculation
    // Evaluates the formula and updates the computed value
    // Called when formula or dependent cells change
    void evaluate();
    // Returns array of cell coordinates (row,col) that this formula depends on
    const DynamicArray<pair<int, int>>& getDependencies() const;
    // Analyzes formula and updates the dependency tracking information
    // Called when formula changes or during initialization
    void updateDependencies();
};

#endif // CELL_H
