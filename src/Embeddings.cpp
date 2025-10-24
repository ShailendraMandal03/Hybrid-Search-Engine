#include "Embeddings.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

void Embeddings::load(const string &path, int maxWords) {
    ifstream in(path);
    if (!in) {
        cerr << "Embeddings file not found: " << path << "\n";
        return;
    }

    string line;
    int loaded = 0;

    while (getline(in, line) && loaded < maxWords) {
        if (line.empty()) continue;

        stringstream ss(line);
        string w;
        ss >> w;

        Vec v;
        float x;
        while (ss >> x)
            v.push_back(x);

        if (dim == 0)
            dim = (int)v.size();

        if ((int)v.size() != dim)
            continue;

        word[w] = v;
        ++loaded;
    }

    cerr << "Loaded embeddings = " << word.size() << " dim = " << dim << "\n";
}

float Embeddings::cosine(const Vec &a, const Vec &b) const {
    if (a.empty() || b.empty() || a.size() != b.size())
        return 0.0f;

    double dot = 0, na = 0, nb = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += (double)a[i] * b[i];
        na += (double)a[i] * a[i];
        nb += (double)b[i] * b[i];
    }

    if (na == 0 || nb == 0)
        return 0.0f;

    return (float)(dot / (sqrt(na) * sqrt(nb) + 1e-12));
}
