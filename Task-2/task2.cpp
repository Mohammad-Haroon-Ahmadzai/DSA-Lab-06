/*
 * CS-250: Data Structures and Algorithms | Lab 6
 * Task 2: Expression Validators Using Stack
 * Part A - Bracket validator : isValidBrackets(s)
 * Part B - HTML/XML validator : validate_markup(s)
 *
 * The stack is implemented from scratch (dynamic array).
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>
using namespace std;

/* ---------------- Custom Stack ---------------- */
template <typename T>
class Stack
{
    T* data;
    int topIdx;
    int capacity;

    void grow()
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
    int size() const { return topIdx + 1; }
    void push(const T& x) { if (topIdx + 1 == capacity) grow(); data[++topIdx] = x; }
    T pop() { return data[topIdx--]; } // caller checks isEmpty() first
    T& top() { return data[topIdx]; }
    T& at(int i) { return data[i]; }   // 0 = bottom of the stack
    void popTo(int newSize) { topIdx = newSize - 1; }
};

/* =================== Part A: Bracket Validator =================== */
bool isValidBrackets(string s)
{
    Stack<char> st;
    for (size_t i = 0; i < s.size(); i++)
    {
        char c = s[i];
        if (c == '(' || c == '{' || c == '[')
        {
            st.push(c);
        }
        else if (c == ')' || c == '}' || c == ']')
        {
            if (st.isEmpty()) return false; // closing with nothing open
            char open = st.pop();
            if ((c == ')' && open != '(') ||
                (c == '}' && open != '{') ||
                (c == ']' && open != '[')) return false; // wrong type
        }
    }
    return st.isEmpty(); // nothing left open
}

/* =================== Part B: HTML/XML Tag Validator =================== */
struct OpenTag
{
    string name;
    int pos; // position of '<' in the input
};

// Extracts the tag name starting at index i (after '<' or '</')
static string readName(const string& s, size_t i)
{
    string name;
    while (i < s.size() && !isspace((unsigned char)s[i]) && s[i] != '>' && s[i] != '/')
        name += s[i++];
    return name;
}

/*
 * Handles: opening tags (with attributes), closing tags, self-closing tags (<br/>),
 * comments (<!-- -->), <?xml ...?> declarations and <!DOCTYPE ...>.
 * Returns (isValid, listOfErrors).
 */
pair<bool, vector<string>> validate_markup(string s)
{
    vector<string> errors;
    Stack<OpenTag> st;
    size_t i = 0;

    while (i < s.size())
    {
        if (s[i] != '<') { i++; continue; }
        int start = (int)i;

        // comment
        if (s.compare(i, 4, "<!--") == 0)
        {
            size_t end = s.find("-->", i + 4);
            if (end == string::npos)
            {
                errors.push_back("Error at position " + to_string(start) + ": unterminated comment");
                break;
            }
            i = end + 3;
            continue;
        }

        // <?xml ... ?> or <!DOCTYPE ...> -> skip
        if (i + 1 < s.size() && (s[i + 1] == '?' || s[i + 1] == '!'))
        {
            size_t end = s.find('>', i);
            if (end == string::npos)
            {
                errors.push_back("Error at position " + to_string(start) + ": unterminated declaration");
                break;
            }
            i = end + 1;
            continue;
        }

        size_t end = s.find('>', i);
        if (end == string::npos)
        {
            errors.push_back("Error at position " + to_string(start) + ": '<' has no closing '>'");
            break;
        }
        string inside = s.substr(i + 1, end - i - 1); // text between < and >
        i = end + 1;

        if (inside.empty() || inside == "/")
        {
            errors.push_back("Error at position " + to_string(start) + ": empty tag");
            continue;
        }

        if (inside[0] == '/') // ---- closing tag ----
        {
            string name = readName(inside, 1);
            if (name.empty())
            {
                errors.push_back("Error at position " + to_string(start) + ": closing tag has no name");
                continue;
            }

            // find the nearest matching open tag in the stack
            int found = -1;
            for (int k = st.size() - 1; k >= 0; k--)
                if (st.at(k).name == name) { found = k; break; }

            if (found == -1)
            {
                errors.push_back("Error at position " + to_string(start) + ": unexpected closing tag </" + name + "> (no matching opening tag)");
            }
            else
            {
                // every tag above the match was never closed
                for (int k = st.size() - 1; k > found; k--)
                    errors.push_back("Error at position " + to_string(st.at(k).pos) + ": tag <" + st.at(k).name +
                                     "> is not closed before </" + name + "> at position " + to_string(start));
                st.popTo(found); // remove match and everything above
            }
        }
        else // ---- opening / self-closing ----
        {
            bool selfClosing = (inside.back() == '/');
            string name = readName(inside, 0);
            if (name.empty())
            {
                errors.push_back("Error at position " + to_string(start) + ": tag has no name");
                continue;
            }
            if (!selfClosing) st.push({name, start});
        }
    }

    // anything left in the stack was never closed
    for (int k = 0; k < st.size(); k++)
        errors.push_back("Error at position " + to_string(st.at(k).pos) + ": tag <" + st.at(k).name + "> is never closed");

    return make_pair(errors.empty(), errors);
}

/* ---------------- Driver (menu-driven, no built-in examples) ---------------- */
int main()
{
    int choice;
    string line;

    do
    {
        cout << "\n===== Task 2: Expression Validators =====\n"
             << "1. Bracket validator\n"
             << "2. HTML/XML tag validator\n"
             << "0. Exit\n"
             << "Enter choice: ";
        if (!(cin >> choice)) break;
        getline(cin, line); // discard rest of the line

        if (choice == 1)
        {
            cout << "Enter a string containing brackets: ";
            getline(cin, line);
            cout << "Result: " << (isValidBrackets(line) ? "VALID" : "INVALID") << "\n";
        }
        else if (choice == 2)
        {
            cout << "Enter the HTML/XML string (single line): ";
            getline(cin, line);
            pair<bool, vector<string>> r = validate_markup(line);
            cout << "Result: " << (r.first ? "VALID" : "INVALID") << "\n";
            for (size_t i = 0; i < r.second.size(); i++) cout << " - " << r.second[i] << "\n";
        }
        else if (choice != 0)
        {
            cout << "Invalid choice, try again.\n";
        }
    } while (choice != 0);

    cout << "Goodbye!\n";
    return 0;
}
