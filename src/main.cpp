#include <iostream>
#include <queue>
#include "SearchEngine.h"
#include "utils.h"

using namespace std;

int main() {
    SearchEngine S;

    cout << "Hybrid Search Engine (C++)\n";
    cout << "Loading docs from ./docs ...\n";
    S.loadDocs("docs");

    cout << "Indexed " << S.index.N << " docs. Vocab=" << S.index.inv.size() << "\n";
    cout << "Loading embeddings (optional) ...\n";
    S.loadEmb("embeddings1122.txt");

    string line;
    while (true) {
        cout << "SEARCH : \n> ";
        if (!getline(cin, line)) break;
        if (line == ":quit" || line == ":exit") break;

        // Trie suggestions
        if (line.rfind(":suggest ", 0) == 0) {
            auto pref = line.substr(9);
            auto s = S.trie.suggest(pref, 10);
            cout << "Suggestions:\n";
            for (auto &x : s)
                cout << " " << x << "\n";
            continue;
        }

        // Embedding-based similar words
        if (line.rfind(":syn ", 0) == 0) {
            auto w = line.substr(5);
            if (S.emb.word.count(w)) {
                cout << "Similar words (embedding) for '" << w << "':\n";
                auto qv = S.emb.word[w];
                priority_queue<pair<float, string>> pq;

                for (auto &p : S.emb.word) {
                    if (p.first == w) continue;
                    float sim = S.emb.cosine(qv, p.second);
                    pq.push({sim, p.first});
                }

                for (int i = 0; i < 10 && !pq.empty(); ++i) {
                    auto pr = pq.top();
                    pq.pop();
                    cout << " " << pr.second << " (" << pr.first << ")\n";
                }
            } else {
                cout << "Word not in embeddings.\n";
            }
            continue;
        }

        // Regular query
        auto res = S.query(line, 10);
        if (res.empty()) {
            cout << "No results found.\n";
            continue;
        }

        cout << "Top results:\n";
        for (auto &r : res) {
            int id = r.first;
            double sc = r.second;
            cout << "[doc=" << id << "] score=" << sc << "\n";
            cout << snippet(S.index.docs[id], line) << "\n\n";
        }
    }

    cout << "Goodbye\n";
    return 0;
}
