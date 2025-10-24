#pragma once
#include "Indexer.h"
#include "Trie.h"
#include "Embeddings.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class SearchEngine {
public:
    Indexer index;
    Trie trie;
    Embeddings emb;

    unordered_map<int, Vec> docVec; // cached doc embeddings
    float alpha = 0.7f;             // weight for tfidf
    float beta = 0.3f;              // weight for embedding similarity

    void loadDocs(const string &folder);
    void loadEmb(const string &path);
    vector<pair<int, double>> query(const string &q, int topK = 10);
};
