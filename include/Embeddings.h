#pragma once
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

using Vec = vector<float>;

class Embeddings{
public:
    int dim = 0;
    unordered_map<string, Vec> word;
    
    void load(const string &path, int maxWords=200000);

    float cosine(const Vec &a, const Vec &b) const;
};