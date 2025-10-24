#include "Indexer.h"
#include "utils.h"
#include <cmath>
#include <iostream>

using namespace std;

void Indexer::addDocument(int docID, const string &text) {
    docs.push_back(text);
    vector<string> toks = tokenize(text);
    int L = (int)toks.size();

    unordered_map<string, vector<int>> positions;
    for (int i = 0; i < L; ++i)
        positions[toks[i]].push_back(i);

    // Positional index
    for (auto &p : positions) {
        const string &term = p.first;
        for (int pos : p.second) {
            inv[term].push_back({docID, pos});
            posIndex[term][docID].push_back(pos);
        }
    }

    // N-grams (bigrams .. maxN)
    for (int n = 2; n <= maxN; ++n) {
        if (L < n) break;
        for (int i = 0; i <= L - n; ++i) {
            string gram = toks[i];
            for (int k = 1; k < n; ++k)
                gram += "_" + toks[i + k];
            inv[gram].push_back({docID, i});
            posIndex[gram][docID].push_back(i);
        }
    }

    // Compute tf (term freq / total tokens)
    unordered_map<string, double> freq;
    for (auto &t : toks)
        freq[t] += 1.0;

    for (auto &f : freq)
        tfidf[docID][f.first] = f.second / (double)L;

    N = max(N, docID + 1);
}

void Indexer::computeIdfTfidf() {
    unordered_map<string, int> df;

    for (auto &p : inv) {
        unordered_set<int> s;
        for (auto &post : p.second)
            s.insert(post.docID);
        df[p.first] = (int)s.size();
    }

    for (auto &p : df)
        idf[p.first] = log((double)N / (1.0 + (double)p.second));

    // Finalize tf-idf (tf currently stored)
    for (int d = 0; d < N; ++d) {
        for (auto &kv : tfidf[d]) {
            const string &term = kv.first;
            double tf = kv.second;
            double id = idf.count(term) ? idf[term] : 0.0;
            tfidf[d][term] = tf * id;
        }
    }

    cerr << "Indexer: N=" << N << " vocab=" << inv.size() << "\n";
}
