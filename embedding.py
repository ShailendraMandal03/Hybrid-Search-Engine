import os
from gensim.models import Word2Vec
from nltk.corpus import stopwords
import string
import nltk

nltk.download('stopwords')

DOCS_DIR = "docs"
OUTPUT_FILE = "embeddings1122.txt"

stop_words = set(stopwords.words('english'))

def preprocess(text):
    # Lowercase, remove punctuation, split words
    text = text.lower()
    text = text.translate(str.maketrans('', '', string.punctuation))
    tokens = text.split()
    # Remove stopwords
    tokens = [t for t in tokens if t not in stop_words]
    return tokens

def load_corpus():
    corpus = []
    for file in sorted(os.listdir(DOCS_DIR)):
        if file.endswith(".txt"):
            with open(os.path.join(DOCS_DIR, file), "r", encoding="utf-8") as f:
                text = f.read()
                tokens = preprocess(text)
                if tokens:
                    corpus.append(tokens)
    return corpus

def train_word2vec(corpus, vector_size=50):
    model = Word2Vec(sentences=corpus, vector_size=vector_size, window=3, min_count=1, sg=1)
    return model

def save_embeddings(model):
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        for word in model.wv.index_to_key:
            vector = model.wv[word]
            vector_str = " ".join([f"{v:.4f}" for v in vector])
            f.write(f"{word} {vector_str}\n")
    print(f"✅ Saved embeddings to '{OUTPUT_FILE}'")

def main():
    corpus = load_corpus()
    print(f"Loaded {len(corpus)} documents from '{DOCS_DIR}'")

    model = train_word2vec(corpus, vector_size=5)
    save_embeddings(model)

if __name__ == "__main__":
    main()
