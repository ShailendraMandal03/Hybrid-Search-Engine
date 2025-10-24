#include "Trie.h"
#include <algorithm>
using namespace std;

Trie::Trie() {
    root = new TrieNode();
}

void Trie::insert(const string &s) {
    TrieNode* node = root;
    for (char c : s) {
        if (!node->ch.count(c))
            node->ch[c] = new TrieNode();
        node = node->ch[c];
    }
    node->end = true;
}

void Trie::dfsCollect(TrieNode* node, string &cur, vector<string> &out, int limit) {
    if ((int)out.size() >= limit) return;

    if (node->end)
        out.push_back(cur);

    for (auto &p : node->ch) {
        cur.push_back(p.first);
        dfsCollect(p.second, cur, out, limit);
        cur.pop_back();

        if ((int)out.size() >= limit)
            return;
    }
}

vector<string> Trie::suggest(const string &prefix, int limit) {
    TrieNode* node = root;
    string cur = prefix;
    vector<string> out;

    for (char c : prefix) {
        if (!node->ch.count(c))
            return {};
        node = node->ch[c];
    }

    dfsCollect(node, cur, out, limit);
    return out;

}
