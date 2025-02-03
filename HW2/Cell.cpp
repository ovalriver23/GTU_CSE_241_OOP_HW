#include "Cell.h"
#include "Spreadsheet.h" 

// Default constructor for the Cell base class. Initializes with default values.
Cell::Cell() : content(""), row(-1), col(-1), spreadsheet(nullptr){}

// Sets the position of the cell in the grid (row and column indices).
void Cell::setPosition(int r, int c){
    row = r;
    col = c;
}

// Returns the row index of the cell.
int Cell::getRow() const {
    return row;
}

// Returns the column index of the cell.
int Cell::getCol() const {
    return col;
}
 
// Assigns the cell to a specific spreadsheet by storing a shared pointer to it.
void Cell::setSpreadsheet(shared_ptr<GTUSpreadsheet::Spreadsheet> sheet) {
    try {
        if (sheet) {
            spreadsheet = sheet;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error setting spreadsheet: " << e.what() << std::endl;
    }
}

// Constructor for ValueCell. Initializes the content with the provided value.
ValueCell::ValueCell(const string &initialContent) : value(initialContent){
    content = initialContent;  // Sync the content with the value provided
}

// Returns the content of the ValueCell as a string.
string ValueCell::getContent() const {
    return value;
}

// Sets the content of the ValueCell and syncs it with the base class content.
void ValueCell::setContent(const string &kontent){
    value = kontent;
    this->content = kontent; // Ensure the base class content is updated as well
}

// Constructor for StringValueCell initializes the content with a given string value
StringValueCell::StringValueCell(const string& initialContent) : ValueCell(initialContent) {}

// Constructor for IntValueCell initializes with an integer value
IntValueCell::IntValueCell(int initialValue) : ValueCell(to_string(initialValue)), intValue(initialValue) {}

// Returns the content of the IntValueCell as a string
string IntValueCell::getContent() const{
    return to_string(intValue);
}

// Sets the content of the IntValueCell, converting from a string and validating it
void IntValueCell::setContent(const string &content){
    try{
        intValue = stoi(content); //Convert string to int
        value = to_string(intValue); 
        this->content = value; // Sync content with the base class
    }catch (const invalid_argument&){
        throw runtime_error("Invalid content for IntValueCell. Must be an integer.");
    }
}

// Constructor for DoubleValueCell initializes the content with a double value
DoubleValueCell::DoubleValueCell(double initialValue)
    : ValueCell(to_string(initialValue)), doubleValue(initialValue) {}

// Returns the content of the DoubleValueCell formatted to two decimal places
string DoubleValueCell::getContent() const {
    ostringstream oss;
    oss << fixed << setprecision(2) << doubleValue; // Format to 2 decimal places
    return oss.str();
}

// Set the content of the cell, converting the string to a double
void DoubleValueCell::setContent(const string& content) {
    try {
        doubleValue = stod(content); // Convert string to double
        value = getContent(); // Format the double
        this->content = value; // Sync with the base class
    } catch (const invalid_argument&) {
        throw runtime_error("Invalid content for DoubleValueCell. Must be a double.");
    }
}

// Constructor for FormulaCell initializes with a formula and a spreadsheet reference
FormulaCell::FormulaCell(const string& initialFormula, shared_ptr<GTUSpreadsheet::Spreadsheet> sheet) 
    : formula(initialFormula), spreadsheet(sheet), dependencies() {
    content = initialFormula;
    updateDependencies(); // Identify dependencies during initialization
    evaluate(); // Evaluate the formula immediately
}

// Sets new content for the FormulaCell and updates its dependencies and value
void FormulaCell::setContent(const string& newContent) {
    formula = newContent;
    content = newContent;
    updateDependencies(); // Refresh dependencies based on the new formula
    evaluate(); // Recalculate the formula's result
}

// Returns the computed value after evaluating the formula
string FormulaCell::getContent() const {
    return computedValue;
}

// Returns the raw formula string without computation
string FormulaCell::getRawContent() const {
    return formula;
}

// Checks whether a given formula content represents a function
bool FormulaCell::isFunction(const string& content) const {
    return content.size() > 1 && content[0] == '@';
}

// Parses a function and range from a formula string, extracting components
pair<string, string> FormulaCell::parseFunctionAndRange(const string& content) const {
    size_t openParen = content.find('(');
    size_t closeParen = content.find(')');
    
    if (openParen == string::npos || closeParen == string::npos) {
        throw runtime_error("Invalid function format");
    }
    
    // Extract the function name and range
    string funcName = content.substr(1, openParen - 1);
    string range = content.substr(openParen + 1, closeParen - openParen - 1);
    return {funcName, range};
}

// Parses a cell range string and splits it into start and end references
pair<string, string> FormulaCell::parseRange(const string& range) const {
    size_t sep = range.find("..");
    if (sep == string::npos) {
        throw runtime_error("Invalid range format");
    }
    return {range.substr(0, sep), range.substr(sep + 2)};
}

// Fetches numerical values from a specified range of cells
DynamicArray<double> FormulaCell::getValuesFromRange(const string& start, const string& end) const {
    int startRow, startCol, endRow, endCol;
    spreadsheet->parseCellReference(start, startRow, startCol);
    spreadsheet->parseCellReference(end, endRow, endCol);
    
    DynamicArray<double> values;
    // Iterate through the specified range and collect numeric values
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            auto cell = spreadsheet->getCell(r, c);
            if (cell && !cell->getContent().empty()) {
                try {
                    values.pushBack(stod(cell->getContent()));
                } catch (...) {
                    // Skip non-numeric cells
                }
            }
        }
    }
    return values;
}

// Calculates the sum of values in the provided range
double FormulaCell::calculateSum(const DynamicArray<double>& values) const {
    double sum = 0;
    for (int i = 0; i < values.getSize(); ++i) {
        sum += values[i];
    }
    return sum;
}

// Calculates the average of values in the provided range
double FormulaCell::calculateAverage(const DynamicArray<double>& values) const {
    if (values.getSize() == 0) return 0;
    return calculateSum(values) / values.getSize();
}

// Finds the maximum value in the provided range
double FormulaCell::calculateMax(const DynamicArray<double>& values) const {
    if (values.getSize() == 0) return 0;
    double max = values[0];
    for (int i = 1; i < values.getSize(); ++i) {
        if (values[i] > max) max = values[i];
    }
    return max;
}

// Finds the minimum value in the provided range
double FormulaCell::calculateMin(const DynamicArray<double>& values) const {
    if (values.getSize() == 0) return 0;
    double min = values[0];
    for (int i = 1; i < values.getSize(); ++i) {
        if (values[i] < min) min = values[i];
    }
    return min;
}



// Calculates the standard deviation of a given set of values.
double FormulaCell::calculateStdDev(const DynamicArray<double>& values) const {
    if (values.getSize() <= 1) return 0;
    
    double sum = 0, squareSum = 0;
    for (int i = 0; i < values.getSize(); ++i) {
        sum += values[i];
        squareSum += values[i] * values[i];
    }
    
    double mean = sum / values.getSize();
    return sqrt(squareSum / values.getSize() - mean * mean);
}

// Evaluates the formula stored in the FormulaCell.
// Supports various functions such as SUM, AVER, MAX, MIN, and STDDEV.
// If a formula starts with '=', it will be treated as an arithmetic expression.
void FormulaCell::evaluate() {
    try {
        if (formula.empty()) { // If the formula is empty, return an empty result
            computedValue = "";
            return;
        }

        // If the formula is a function (e.g., @SUM(A1..B2)), parse and evaluate it
        if (isFunction(formula)) {
            auto [funcName, range] = parseFunctionAndRange(formula);
            auto [start, end] = parseRange(range);
            auto values = getValuesFromRange(start, end);
            
            double result = 0;
            // Check for supported functions and perform the appropriate calculation
            if (funcName == "SUM" || funcName == "Sum") {
                result = calculateSum(values);
            } else if (funcName == "AVER" || funcName == "Aver") {
                result = calculateAverage(values);
            } else if (funcName == "MAX" || funcName == "Max") {
                result = calculateMax(values);
            } else if (funcName == "MIN" || funcName == "Min") {
                result = calculateMin(values);
            } else if (funcName == "STDDEV" || funcName == "Stddev") {
                result = calculateStdDev(values);
            } else {
                throw runtime_error("Unknown function: " + funcName); // Error if the function is unsupported
            }

           // Convert the result to a formatted string with 2 decimal places 
            ostringstream oss;
            oss << fixed << setprecision(2) << result;
            computedValue = oss.str();
            return;
        }

        // If the formula starts with '=', treat it as a mathematical expression
        if (formula[0] == '=') {
            DynamicArray<string> tokens;
            tokenize(formula.substr(1), tokens); // Tokenize the expression after '='
            double result = evaluateExpression(tokens); // Evaluate the expression
            ostringstream oss;
            oss << fixed << setprecision(2) << result;
            computedValue = oss.str();
            return;
        }
         // If it's not a formula, treat it as a plain value
        computedValue = formula;
    } catch (...) {
        computedValue = "#ERROR";
    }
}

// Rest of the implementation remains the same as in the previous version: tokenize
void FormulaCell::tokenize(const string& formula, DynamicArray<string>& tokens) const {
    string token;
    bool inFunction = false; // Flag to track whether the parser is inside a function call
    
    // Loop through each character in the formula string
    for (size_t i = 0; i < formula.length(); ++i) {
        char ch = formula[i];
        
        // If the character is '@', it signals the start of a function call
        if (ch == '@') {
            // If a token was already being built, push it to the token array
            if (!token.empty()) {
                tokens.pushBack(token);
                token.clear();
            }
            inFunction = true; // Set the function flag and start collecting the function call
            token += ch;
            // Continue collecting characters until the closing parenthesis of the function
        } else if (inFunction) {
            token += ch;
            if (ch == ')') {
                tokens.pushBack(token);
                token.clear();
                inFunction = false; // Exit function parsing mode
            }
        // If the character is a whitespace, push the current token if it's non-empty
        } else if (isspace(ch)) {
            if (!token.empty()) {
                tokens.pushBack(token);
                token.clear();
            }
        // If the character is an arithmetic operator, push the current token and the operator
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            if (!token.empty()) {
                tokens.pushBack(token);
                token.clear();
            }
            tokens.pushBack(string(1, ch));  // Push the operator as a separate token
        } 
        // For all other characters, continue building the token
        else {
            token += ch;
        }
    }
    // If there's any leftover token, push it to the token list
    if (!token.empty()) {
        tokens.pushBack(token);
    }
}

// Evaluates a mathematical expression from a sequence of tokens
double FormulaCell::evaluateExpression(const DynamicArray<string>& tokens) const {
    double result = 0;
    string currentOp; // To keep track of the current operator being processed

    // Iterate through each token in the provided list
    for (int i = 0; i < tokens.getSize(); ++i) {
        const string& token = tokens[i];
        // If the token is an operator, store it for the next operation
        if (isOperator(token)) {
            currentOp = token;
        } 
        // If the token is a number or cell reference, evaluate its value
        else {
            double value = isCellReference(token) ? fetchValueFromReference(token) : stod(token);
            // If no operator has been set yet, initialize the result with the current value
            if (currentOp.empty()) {
                result = value;
            } 
            // If there's an operator stored, apply it with the current value and result
            else {
                result = applyOperator(result, value, currentOp);
            }
        }
    }

    return result;
}

// Updates the dependencies of the current FormulaCell based on the formula content
void FormulaCell::updateDependencies() {
    dependencies.clear(); // Clear previous dependencies before recalculating
    
    // If the formula is empty or not a formula/function, skip further processing
    if (formula.empty() || (formula[0] != '=' && formula[0] != '@')) return;

    // Tokenize the formula, ignoring the first character ('=' or '@')
    DynamicArray<string> tokens;
    tokenize(formula.substr(1), tokens);

    // Identify cell references within the tokenized formula
    for (int i = 0; i < tokens.getSize(); ++i) {
        if (isCellReference(tokens[i])) {
            int row, col;
            // Parse the cell reference and add it to the dependencies list
            spreadsheet->parseCellReference(tokens[i], row, col);
            dependencies.pushBack(make_pair(row, col));
        }
    }
}

// Fetches the numeric value from a referenced cell in the spreadsheet
double FormulaCell::fetchValueFromReference(const string& reference) const {
    int row, col;
    // Parse the cell reference (e.g., "A1" -> row, col)
    spreadsheet->parseCellReference(reference, row, col);
    
    // Attempt to retrieve the referenced cell
    auto cell = spreadsheet->getCell(row, col);
    if (!cell) throw runtime_error("Invalid cell reference");
    // Fetch the content of the cell
    string content = cell->getContent();
    // Ensure the content can be converted to a numeric value
    if (!isNumber(content)) throw runtime_error("Non-numeric cell content");
    // Convert the content to a double and return the value
    return stod(content);
}

// Checks if a given token is a mathematical operator (+, -, *, /)
bool FormulaCell::isOperator(const string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

// Applies the specified mathematical operator on two numeric operands
double FormulaCell::applyOperator(double a, double b, const string& op) const {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("Division by zero");
        return a / b;
    }
    throw runtime_error("Unknown operator");
}

// Returns the list of cell dependencies for the current formula
const DynamicArray<pair<int, int>>& FormulaCell::getDependencies() const {
    return dependencies;
}


// Determines if a given token is a valid cell reference (e.g., A1, B2)
bool FormulaCell::isCellReference(const string& token) const {
    if (token.empty()) return false;
    
    size_t i = 0;

    // Ensure the token starts with one or more alphabetical characters
    while (i < token.size() && isalpha(token[i])) ++i;
    if (i == 0 || i >= token.size()) return false;
    // Ensure the remaining part of the token consists of digits
    while (i < token.size() && isdigit(token[i])) ++i;
    // Return true only if the entire token was validated
    return i == token.size();
}

// Checks if a string can be converted to a numeric value
bool FormulaCell::isNumber(const string& token) const {
    try {
        if (token.empty()) return false;
        stod(token); // Attempt to convert to a double
        return true;
    } catch (...) {
        // Return false if conversion fails
        return false;
    }
}
