
// #pragma once
// #include <string>
// #include <vector>

// using namespace std;

// vector<string> tokenize(const string &text);
// int edit_distance(string &a,string &b);
// string snippet(string &doc, string &term);

#pragma once
#include <string>
#include <vector>
using namespace std;

vector<string> tokenize(const string &text);
int edit_distance(const string &a, const string &b);
string snippet(const string &doc, const string &term);
