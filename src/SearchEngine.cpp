#include "SearchEngine.h"
#include "utils.h"
#include <filesystem>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <iostream>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

// -----------------------------------------------------
// Load all documents and index them
// -----------------------------------------------------
void SearchEngine::loadDocs(const string &folder) {
    int id = 0;

    for (auto &e : fs::directory_iterator(folder)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() != ".txt") continue;

        ifstream in(e.path());
        string txt((istreambuf_iterator<char>(in)), {});
        index.addDocument(id, txt);

        vector<string> toks = tokenize(txt);
        unordered_set<string> seen;
        for (auto &t : toks) {
            if (!seen.count(t)) {
                trie.insert(t);
                seen.insert(t);
            }
        }
        ++id;
    }

    index.computeIdfTfidf();
    cout << "[INFO] Indexed " << index.N << " documents.\n";
}

// -----------------------------------------------------
// Load word embeddings
// -----------------------------------------------------
void SearchEngine::loadEmb(const string &path) {
    emb.load(path); // build doc vectors

    for (int d = 0; d < index.N; ++d) {
        vector<string> toks = tokenize(index.docs[d]);
        Vec acc(emb.dim, 0.0f);
        int cnt = 0;

        for (auto &t : toks) {
            auto it = emb.word.find(t);
            if (it != emb.word.end()) {
                for (int i = 0; i < emb.dim; ++i)
                    acc[i] += it->second[i];
                ++cnt;
            }
        }

        if (cnt > 0) {
            for (int i = 0; i < emb.dim; ++i)
                acc[i] /= (float)cnt;
            docVec[d] = acc;
        } else {
            docVec[d] = Vec();
        }
    }

    cout << "[INFO] Loaded embeddings with dim=" << emb.dim << "\n";
}

// -----------------------------------------------------
// Create query vector using average word embeddings
// -----------------------------------------------------
static Vec make_query_vec(const Embeddings &emb, const vector<string> &toks) {
    if (emb.dim == 0) return Vec();

    Vec acc(emb.dim, 0.0f);
    int cnt = 0;

    for (auto &t : toks) {
        auto it = emb.word.find(t);
        if (it != emb.word.end()) {
            for (int i = 0; i < emb.dim; ++i)
                acc[i] += it->second[i];
            ++cnt;
        }
    }

    if (cnt == 0) return Vec();

    for (int i = 0; i < emb.dim; ++i)
        acc[i] /= cnt;

    return acc;
}

// -----------------------------------------------------
// Hybrid Search Query (TF-IDF + Trie + Embeddings)
// -----------------------------------------------------
vector<pair<int, double>> SearchEngine::query(const string &q, int topK) {
    vector<string> toks = tokenize(q);
    unordered_map<int, double> scores;

    // 1) TF-IDF and positional scores
    for (auto &t : toks) {
        if (index.inv.count(t)) {
            for (auto &p : index.inv[t]) {
                int d = p.docID;
                double w = 0.0;
                if (index.tfidf.count(d) && index.tfidf[d].count(t))
                    w = index.tfidf[d][t];
                scores[d] += alpha * w;
            }
        } else {
            // ---- Trie-based spelling / prefix correction ----
            auto sug = trie.suggest(t.substr(0, min((int)t.size(), 3)), 10);
            for (auto &cand : sug) {
                int ed = edit_distance(t, cand);
                if (ed <= 2 && index.inv.count(cand)) {
                    for (auto &p : index.inv[cand]) {
                        int d = p.docID;
                        double w = (index.tfidf.count(d) && index.tfidf[d].count(cand))
                            ? index.tfidf[d][cand] : 0.0;
                        scores[d] += alpha * w * 0.6;
                    }
                }
            }

            // ---- Embedding-based synonym expansion ----
            if (emb.word.count(t)) {
                priority_queue<pair<float, string>> pq;
                const Vec &qv = emb.word.at(t);
                int pushed = 0;

                for (auto &p : emb.word) {
                    if (p.first == t) continue;
                    float sim = emb.cosine(qv, p.second);
                    pq.push({sim, p.first});
                    ++pushed;
                    if (pushed > 5000) break;
                }

                int take = 5;
                while (!pq.empty() && take--) {
                    auto pr = pq.top();
                    pq.pop();
                    string sw = pr.second;
                    float simil = pr.first;

                    if (index.inv.count(sw)) {
                        for (auto &p : index.inv[sw]) {
                            int d = p.docID;
                            double w = (index.tfidf.count(d) && index.tfidf[d].count(sw))
                                ? index.tfidf[d][sw] : 0.0;
                            scores[d] += alpha * w * simil;
                        }
                    }
                }
            }
        }
    }

    // 2) Embedding-based document similarity (semantic matching)
    Vec qv = make_query_vec(emb, toks);
    if (!qv.empty()) {
        for (int d = 0; d < index.N; ++d) {
            float sim = emb.cosine(qv, docVec[d]);
            if (sim > 0)
                scores[d] += beta * sim;
        }
    }

    // 3) Phrase proximity / bigram boost
    if (toks.size() >= 2) {
        string bigram = toks[0];
        for (size_t i = 1; i < toks.size(); ++i)
            bigram += "_" + toks[i];

        if (index.posIndex.count(bigram)) {
            for (auto &kv : index.posIndex[bigram])
                scores[kv.first] += 1.0;
        } else {
            for (int d = 0; d < index.N; ++d) {
                double proxScore = 0.0;
                bool ok = true;
                int lastPos = -1;

                for (auto &t : toks) {
                    if (!index.posIndex.count(t) || !index.posIndex[t].count(d)) {
                        ok = false;
                        break;
                    }
                    auto &plist = index.posIndex[t][d];
                    int p = plist[0];
                    if (lastPos != -1)
                        proxScore += 1.0 / (1 + abs(p - lastPos));
                    lastPos = p;
                }

                if (ok && proxScore > 0)
                    scores[d] += 0.5 * proxScore;
            }
        }
    }

    // ---- Fallback: If no results, retry using Trie suggestion ----
    if (scores.empty()) {
        auto sug = trie.suggest(q, 3);
        if (!sug.empty()) {
            cout << "[INFO] No direct match found. Retrying with '" << sug[0] << "'...\n";
            return query(sug[0], topK);  // recursively retry
        }

        // Try embedding-based similar words
        if (emb.word.count(q)) {
            const Vec &qv = emb.word.at(q);
            float bestSim = -1.0;
            string bestWord;

            for (auto &p : emb.word) {
                if (p.first == q) continue;
                float sim = emb.cosine(qv, p.second);
                if (sim > bestSim) {
                    bestSim = sim;
                    bestWord = p.first;
                }
            }

            if (!bestWord.empty()) {
                cout << "[INFO] Retrying with embedding-similar word '" << bestWord << "'...\n";
                return query(bestWord, topK);
            }
        }
    }

    // 4) Rank results using max-heap
    priority_queue<pair<double, int>> pq;
    for (auto &p : scores)
        pq.push({p.second, p.first});

    vector<pair<int, double>> out;
    while (!pq.empty() && (int)out.size() < topK) {
        auto pr = pq.top();
        pq.pop();
        out.push_back({pr.second, pr.first});
    }

    return out;
}
