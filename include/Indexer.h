#pragma once
#include <string>

#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Posting {
    int docID;
    int pos;
};

class Indexer {
public:
    int N = 0; // number of docs
    vector<string> docs; // raw text by docID

    unordered_map<string, vector<Posting>> inv; // token -> postings
    unordered_map<string, unordered_map<int, vector<int>>> posIndex; // token -> docID -> [positions]
    unordered_map<string, double> idf; // idf per term
    unordered_map<int, unordered_map<string, double>> tfidf; // doc -> (term -> tfidf)

    int maxN = 3; // up to tri-grams

    void addDocument(int docID, const string &text);
    void computeIdfTfidf();
};


