/*
 * CS-250: Data Structures and Algorithms | Lab 6
 * Task 1: Expression Conversion and Evaluation
 *
 * A generic array-based stack is implemented from scratch (no STL containers).
 * Tokens in an expression are separated by optional spaces; operands may be
 * multi-character (e.g. 12, 3.5, A, x1). Supported operators: + - * / ^ and ( ).
 * All converted expressions are returned with tokens separated by single spaces.
 */

#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
using namespace std;

/* ---------------- Custom Stack (dynamic array) ---------------- */
template <typename T>
class Stack
{
    T* data;
    int topIdx;
    int capacity;

    void grow() // double capacity when full
    {
        int newCap = capacity * 2;
        T* bigger = new T[newCap];
        for (int i = 0; i <= topIdx; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        capacity = newCap;
    }

public:
    Stack(int cap = 16) : topIdx(-1), capacity(cap) { data = new T[capacity]; }
    ~Stack() { delete[] data; }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    bool isEmpty() const { return topIdx == -1; }

    void push(const T& x) { if (topIdx + 1 == capacity) grow(); data[++topIdx] = x; }

    T pop()
    {
        if (isEmpty()) throw runtime_error("Stack underflow (malformed expression)");
        return data[topIdx--];
    }

    T& top()
    {
        if (isEmpty()) throw runtime_error("Stack is empty");
        return data[topIdx];
    }
};

/* ---------------- Helpers ---------------- */
bool isOperator(const string& t)
{
    return t.size() == 1 && (t[0] == '+' || t[0] == '-' || t[0] == '*' || t[0] == '/' || t[0] == '^');
}

int precedence(char op)
{
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

// Splits expr into tokens. Returns a dynamically allocated array (caller deletes[]).
string* tokenize(const string& expr, int& count)
{
    string* tokens = new string[expr.size() + 1];
    count = 0;
    size_t i = 0;
    while (i < expr.size())
    {
        char c = expr[i];
        if (c == ' ' || c == '\t') { i++; continue; }
        if (isalnum((unsigned char)c) || c == '.') // operand
        {
            string t;
            while (i < expr.size() && (isalnum((unsigned char)expr[i]) || expr[i] == '.'))
                t += expr[i++];
            tokens[count++] = t;
        }
        else // operator / bracket
        {
            tokens[count++] = string(1, c);
            i++;
        }
    }
    return tokens;
}

string joinTokens(const string* t, int n)
{
    string r;
    for (int i = 0; i < n; i++) { if (i) r += ' '; r += t[i]; }
    return r;
}

/*
 * Shunting-yard algorithm. When prefixMode is true the token array is expected
 * to be already reversed (with brackets swapped) and the associativity rules are
 * flipped so that, after reversing the output, the prefix form is correct.
 */
string* shuntingYard(const string* tok, int n, bool prefixMode, int& outCount)
{
    string* out = new string[n + 1];
    outCount = 0;
    Stack<char> ops;

    for (int i = 0; i < n; i++)
    {
        const string& t = tok[i];
        if (t == "(")
        {
            ops.push('(');
        }
        else if (t == ")")
        {
            while (!ops.isEmpty() && ops.top() != '(') out[outCount++] = string(1, ops.pop());
            if (ops.isEmpty()) throw runtime_error("Mismatched parentheses");
            ops.pop(); // discard '('
        }
        else if (isOperator(t))
        {
            char cur = t[0];
            while (!ops.isEmpty() && ops.top() != '(')
            {
                int pt = precedence(ops.top()), pc = precedence(cur);
                bool pop;
                if (!prefixMode) pop = (pt > pc) || (pt == pc && cur != '^'); // '^' is right-assoc
                else pop = (pt > pc) || (pt == pc && cur == '^');             // flipped for reversed scan
                if (!pop) break;
                out[outCount++] = string(1, ops.pop());
            }
            ops.push(cur);
        }
        else
        {
            out[outCount++] = t; // operand
        }
    }
    while (!ops.isEmpty())
    {
        char c = ops.pop();
        if (c == '(') throw runtime_error("Mismatched parentheses");
        out[outCount++] = string(1, c);
    }
    return out;
}

/* ---------------- Required functions ---------------- */
string infix_to_postfix(string expr)
{
    int n, m;
    string* tok = tokenize(expr, n);
    string* out = nullptr;
    try { out = shuntingYard(tok, n, false, m); }
    catch (...) { delete[] tok; throw; }
    string result = joinTokens(out, m);
    delete[] tok; delete[] out;
    return result;
}

string infix_to_prefix(string expr)
{
    int n, m;
    string* tok = tokenize(expr, n);

    // reverse the tokens and swap the brackets
    string* rev = new string[n + 1];
    for (int i = 0; i < n; i++)
    {
        string t = tok[n - 1 - i];
        if (t == "(") t = ")"; else if (t == ")") t = "(";
        rev[i] = t;
    }

    string* out = nullptr;
    try { out = shuntingYard(rev, n, true, m); }
    catch (...) { delete[] tok; delete[] rev; throw; }

    // reverse the postfix-like output to get prefix
    string* fin = new string[m + 1];
    for (int i = 0; i < m; i++) fin[i] = out[m - 1 - i];

    string result = joinTokens(fin, m);
    delete[] tok; delete[] rev; delete[] out; delete[] fin;
    return result;
}

string postfix_to_infix(string expr)
{
    int n;
    string* tok = tokenize(expr, n);
    Stack<string> st;
    try
    {
        for (int i = 0; i < n; i++)
        {
            if (isOperator(tok[i]))
            {
                string b = st.pop();
                string a = st.pop();
                st.push("(" + a + " " + tok[i] + " " + b + ")");
            }
            else st.push(tok[i]);
        }
        string result = st.pop();
        if (!st.isEmpty()) throw runtime_error("Malformed postfix expression");
        delete[] tok;
        return result;
    }
    catch (...) { delete[] tok; throw; }
}

// Converts an operand token to a number, with a readable error for symbols like A, B...
double toNumber(const string& t)
{
    try { size_t pos; double v = stod(t, &pos); if (pos == t.size()) return v; }
    catch (...) {}
    throw runtime_error("Cannot evaluate: operand '" + t + "' is not numeric");
}

double applyOp(char op, double a, double b)
{
    switch (op)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': if (b == 0) throw runtime_error("Division by zero"); return a / b;
    case '^': return pow(a, b);
    }
    throw runtime_error("Unknown operator");
}

double evaluate_postfix(string expr)
{
    int n;
    string* tok = tokenize(expr, n);
    Stack<double> st;
    try
    {
        for (int i = 0; i < n; i++)
        {
            if (isOperator(tok[i]))
            {
                double b = st.pop();
                double a = st.pop();
                st.push(applyOp(tok[i][0], a, b));
            }
            else st.push(toNumber(tok[i]));
        }
        double r = st.pop();
        if (!st.isEmpty()) throw runtime_error("Malformed postfix expression");
        delete[] tok;
        return r;
    }
    catch (...) { delete[] tok; throw; }
}

double evaluate_prefix(string expr)
{
    int n;
    string* tok = tokenize(expr, n);
    Stack<double> st;
    try
    {
        for (int i = n - 1; i >= 0; i--) // scan right to left
        {
            if (isOperator(tok[i]))
            {
                double a = st.pop(); // first operand
                double b = st.pop(); // second operand
                st.push(applyOp(tok[i][0], a, b));
            }
            else st.push(toNumber(tok[i]));
        }
        double r = st.pop();
        if (!st.isEmpty()) throw runtime_error("Malformed prefix expression");
        delete[] tok;
        return r;
    }
    catch (...) { delete[] tok; throw; }
}

/* ---------------- Driver ---------------- */

// Converts one infix expression to every other form and evaluates it
void demoInfix(const string& e)
{
    cout << "Infix : " << e << "\n";
    try
    {
        string post = infix_to_postfix(e);
        string pre = infix_to_prefix(e);
        cout << "Postfix : " << post << "\n";
        cout << "Prefix : " << pre << "\n";
        cout << "Postfix -> Infix : " << postfix_to_infix(post) << "\n";
    }
    catch (const exception& ex)
    {
        cout << "Error: " << ex.what() << "\n";
        cout << "------------------------------------------\n";
        return;
    }

    try
    {
        double v1 = evaluate_postfix(infix_to_postfix(e));
        double v2 = evaluate_prefix(infix_to_prefix(e));
        cout << "Value (postfix) : " << v1 << "\n";
        cout << "Value (prefix) : " << v2 << "\n";
    }
    catch (const exception& ex)
    {
        cout << "Evaluation: " << ex.what() << "\n";
    }
    cout << "------------------------------------------\n";
}

int main()
{
    cout << "===== Task 1: Expression Conversion and Evaluation =====\n";
    cout << "Enter an infix expression with spaces between tokens, e.g. ( 2 + 3 ) * 4\n";
    cout << "Type 'exit' to quit.\n\n";

    string line;
    while (true)
    {
        cout << "> ";
        if (!getline(cin, line) || line == "exit") break;
        if (line.empty()) continue;
        demoInfix(line);
    }
    return 0;
}
