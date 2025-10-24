#pragma once
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct TrieNode {
    unordered_map<char, TrieNode*> ch;
    bool end = false;
};

class Trie {
public:
    Trie();

    void insert(const string &s);
    vector<string> suggest(const string &prefix, int limit = 10);

private:
    TrieNode* root;
    void dfsCollect(TrieNode* node, string &cur, vector<string> &out, int limit);
};
