# Hybrid-Search-Engine

A full-featured hybrid search engine combining:

TF-IDF and positional inverted index

Trie for autocomplete

Edit-distance–based spell correction

Word embeddings for semantic understanding

Bigram/trigram proximity ranking

**Features**

✅ Tokenization & inverted index
✅ TF-IDF ranking with IDF normalization
✅ Positional index for phrase & proximity search
✅ Trie-based autocomplete (:suggest mac)
✅ Edit distance spell correction (Levenshtein)
✅ Word embeddings for semantic similarity
✅ Semantic query expansion for misspellings & synonyms
✅ Embedding-based document ranking

**🧩 Folder Structure**
hybrid-search/
├── README.md
├── docs/
│   ├── 0.txt
│   └── 1.txt
| ___embedding.py
├── embeddings.txt
├── include/
│   ├── Indexer.h
│   ├── Trie.h
│   ├── Embeddings.h
│   ├── SearchEngine.h
│   └── utils.h
└── src/
    ├── Indexer.cpp
    ├── Trie.cpp
    ├── Embeddings.cpp
    ├── SearchEngine.cpp
    ├── utils.cpp
    └── main.cpp

**Add Your Data**
Put .txt documents inside the docs/ folder

**How to Use**
Run the program and try:  g++ -std=c++17 -Iinclude src/main.cpp src/Indexer.cpp src/Trie.cpp src/Embeddings.cpp src/SearchEngine.cpp src/utils.cpp -o search

                        : ./search
**Dependencies**
  1. C++17
  2. Standard C++ libraries only (<vector>, <unordered_map>, <filesystem>, <queue>)

**Future Add-ons**
1. Web interface (Flask / FastAPI or Crow C++)

2. Sentence-level embeddings (e.g., SBERT)

2. Document clustering visualization

**Author**
Built by Shailendra Mandal,
B.Tech Student — combining NLP, embeddings,IR and C++17 for hybrid search.
