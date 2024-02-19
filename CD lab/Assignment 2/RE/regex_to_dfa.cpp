#include <bits/stdc++.h>
#define pb push_back
using namespace std;

int gpos, fl_pos, accepted_state;       // for finding  position and follow position index
vector<int> follow_pos[30], state[30];  // follow_pos store the follow position and state store total no of Dstates
map<int, char> alpha_int;
set<char> in_alpha;
map<vector<int>, map<char, vector<int>>> re_dfa;

vector<int> operator+(vector<int> a, vector<int> b) { // operator Overloading for vector Additions
    a.insert(a.end(), b.begin(), b.end());
    sort(a.begin(), a.end());                      // required for making unique element
    a.erase(unique(a.begin(), a.end()), a.end());  // for finding unique element within vector
    return a;
}

struct tree { // Structure for Treenode
    char alpha;
    int pos;
    bool nullable;
    vector<int> fpos, lpos;
    tree *left, *right;
};

bool is_op(char ch) {
    if (ch == '|' || ch == '/' || ch == '*' || ch == '.')
        return true;
    return false;
}

tree *create(char ch, int pos) { // Creating Node Memory and initialization
    tree *node = new tree;
    node->alpha = ch;
    node->pos = pos;
    node->left = node->right = NULL;
    node->lpos.clear();
    node->fpos.clear();
    return node;
}

void vec_print(vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
}

void infixToPostfix(string &infix, string &postfix) {
    map<char, int> precedence;
    precedence['|'] = 1;
    precedence['/'] = 2;
    precedence['.'] = 3;
    precedence['*'] = 4;

    stack<char> operators;
    for (char ch : infix) {
        if (isalpha(ch) || ch == '#') {
            postfix += ch;
        } else if (ch == '(') {
            operators.push(ch);
        } else if (ch == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop();  // Discard the '('
        } else if (is_op(ch)) {
            while (!operators.empty() && precedence[operators.top()] >= precedence[ch]) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(ch);
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }
}

void postfix(tree *root)  // For Traversing The tree
{
    if (root) {
        string s("	    	");
        postfix(root->left);
        postfix(root->right);
        cout << root->alpha << s << root->pos << s << root->nullable << s << "{ ";
        vec_print(root->fpos);
        cout << " }" << s << s << "{ ";
        vec_print(root->lpos);
        cout << " }" << endl;
    }
}

void dfa(tree *root, string input)  // Finding DFA
{
    int num_state = 1, cur_state = 1;
    char ch = 'A';
    vector<int> temp;
    map<vector<int>, char> out_state;  // Out_state used for removing Redundant States
    // map<vector<int>, map<char, vector<int>>> re_dfa; // for Storing The final DFA state
    state[num_state++] = root->fpos;  // total number of State , Starting with root's First position
    out_state[root->fpos] = ch++;
    while (1) {
        for (int i = 0; i < input.size(); i++) {
            for (int j = 0; j < state[cur_state].size(); j++) {
                if (alpha_int[state[cur_state][j]] == input[i])
                    temp = temp + follow_pos[state[cur_state][j]];
                if (out_state[temp] == 0 && temp.size() > 0) {
                    out_state[temp] = ch++;
                    state[num_state++] = temp;
                }
            }
            re_dfa[state[cur_state]][input[i]] = temp;
            temp.clear();
        }
        if (cur_state == num_state - 1)
            break;
        cur_state++;
    }
    cout << "\n\nThe Final State Table :\n\n";  // for Displaying the Final DFA states
    for (auto an : re_dfa) {
        cout << "{ ";
        for (auto jn : an.first)
            cout << jn << " ";
        cout << " } ";
        for (auto jn : an.second) {
            cout << " at : " << jn.first << " { ";
            for (auto kn : jn.second)
                cout << kn << " ";
            cout << " } ";
        }
        cout << endl;
    }
}

bool checkAcceptance(string str) {
    vector<int> curr_states = state[1];
    for (int i = 0; i < str.size(); i++) {
        if (in_alpha.find(str[i]) == in_alpha.end()) {
            return false;
        }
        if (re_dfa[curr_states].find(str[i]) == re_dfa[curr_states].end()) {
            return false;
        }
        vector<int> next_states = re_dfa[curr_states][str[i]];
        curr_states = next_states;
    }
    for (auto &it : curr_states) {
        if (it == accepted_state) {
            return true;
        }
    }
    return false;
}

int main() {
    tree *temp;
    stack<tree *> s;
    string infix, postfix_input, sp("		"), input;
    cout << "\nEnter the Infix Expression(in format (RE).#) = ";
    cin >> infix;

    infixToPostfix(infix, postfix_input);

    cout << "\nPostfix Expression: " << postfix_input << endl;

    for (int i = 0; i < postfix_input.size(); i++) {
        if (!is_op(postfix_input[i])) {
            gpos++;
            if (postfix_input[i] != '#') {
                fl_pos++;
                alpha_int[fl_pos] = postfix_input[i];
                in_alpha.insert(postfix_input[i]);
            } else {
                accepted_state = gpos;
            }
            temp = create(postfix_input[i], gpos);
            temp->nullable = false;
            temp->fpos.pb(gpos);
            temp->lpos.pb(gpos);
        } else if (postfix_input[i] == '*') {
            temp = create(postfix_input[i], 0);
            temp->left = s.top(), s.pop();
            temp->nullable = true;
            temp->fpos = temp->left->fpos;
            temp->lpos = temp->left->lpos;
            for (int i = 0; i < temp->lpos.size(); i++)
                follow_pos[temp->lpos[i]] = follow_pos[temp->lpos[i]] + temp->fpos;
        } else if (postfix_input[i] == '.') {
            temp = create(postfix_input[i], 0);
            temp->right = s.top(), s.pop();
            temp->left = s.top(), s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            if (temp->left->nullable)
                temp->fpos = temp->right->fpos + temp->left->fpos;
            else
                temp->fpos = temp->left->fpos;
            if (temp->right->nullable)
                temp->lpos = temp->right->lpos + temp->left->lpos;
            else
                temp->lpos = temp->right->lpos;
            for (int i = 0; i < temp->left->lpos.size(); i++)
                follow_pos[temp->left->lpos[i]] = follow_pos[temp->left->lpos[i]] + temp->right->fpos;
        } else {
            temp = create(postfix_input[i], 0);
            temp->right = s.top(), s.pop();
            temp->left = s.top(), s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            temp->fpos = temp->right->fpos + temp->left->fpos;
            temp->lpos = temp->right->lpos + temp->left->lpos;
        }
        s.push(temp);
    }
    for (auto temp : in_alpha)
        input.pb(temp);
    cout << "\n\nNODE" << sp << "Position"
         << "       "
         << "Nullable"
         << "        "
         << "First position"
         << "		   "
         << "Last position" << endl;
    postfix(temp);
    cout << "\n\nFollow Position" << endl;
    for (int i = 1; i <= fl_pos; i++)  // Display of Follow Position
    {
        cout << i << sp << alpha_int[i] << sp << "{ ";
        for (int j = 0; j < follow_pos[i].size(); j++) {
            cout << follow_pos[i][j] << " ";
        }
        cout << " }\n";
    }
    dfa(temp, input);
    cout << accepted_state << endl;
    string test1, test2;
    cout << "Enter first string to test" << endl;
    cin >> test1;
    cout << "Enter second string to test" << endl;
    cin >> test2;
    bool isAcc1 = checkAcceptance(test1);
    cout << isAcc1 << endl;
    bool isAcc2 = checkAcceptance(test2);
    cout << isAcc2 << endl;
    return 0;
}
