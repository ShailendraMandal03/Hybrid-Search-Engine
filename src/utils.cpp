#include "utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

using namespace std;

static inline bool is_word_char(char c) {
    return isalnum((unsigned char)c) || c == '\'' || c == '-';
}

vector<string> tokenize(const string &text) {
    vector<string> tokens;
    string cur;

    for (char ch : text) {
        if (is_word_char(ch)) {
            cur.push_back((char)tolower((unsigned char)ch));
        } else {
            if (!cur.empty()) {
                tokens.push_back(cur);
                cur.clear();
            }
        }
    }

    if (!cur.empty()) 
        tokens.push_back(cur);

    return tokens;
}

int edit_distance(const string &a, const string &b) {
    int n = a.size(), m = b.size();

    if (n == 0) return m;
    if (m == 0) return n;

    vector<int> prev(m + 1), cur(m + 1);

    for (int j = 0; j <= m; ++j)
        prev[j] = j;

    for (int i = 1; i <= n; ++i) {
        cur[0] = i;
        for (int j = 1; j <= m; ++j) {
            cur[j] = min({
                prev[j] + 1,
                cur[j - 1] + 1,
                prev[j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1)
            });
        }
        prev.swap(cur);
    }

    return prev[m];
}

string snippet(const string &doc, const string &term) {
    string lowDoc = doc;
    for (char &c : lowDoc)
        c = (char)tolower((unsigned char)c);

    string lowTerm = term;
    for (char &c : lowTerm)
        c = (char)tolower((unsigned char)c);

    size_t pos = lowDoc.find(lowTerm);
    if (pos == string::npos)
        return doc.substr(0, min((size_t)200, doc.size())) +
               (doc.size() > 200 ? "..." : "");

    size_t start = pos > 60 ? pos - 60 : 0;
    size_t len = min((size_t)200, doc.size() - start);

    string out = (start > 0 ? "..." : "") +
                 doc.substr(start, len) +
                 ((start + len < doc.size()) ? "..." : "");

    return out;
}
