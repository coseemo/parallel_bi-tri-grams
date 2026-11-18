
#include "ngram_counter.hpp"
#include "tokenizer.hpp"

#include <iostream>
#include <string>

namespace {

//verifica che un n-gramma abbia la frequenza attesa
bool check_frequency(
    const ngram::FrequencyTable& table,
    const std::string& key,
    std::size_t expected
) {
    const auto it = table.find(key);

    if (it == table.end() || it->second != expected) {
        std::cerr << "test fallito: frequenza errata per ["
                  << key << "]\n";
        return false;
    }

    return true;
}

//verifica che una tabella contenga il numero atteso di n-grammi distinti
bool check_size(
    const ngram::FrequencyTable& table,
    std::size_t expected
) {
    if (table.size() != expected) {
        std::cerr << "test fallito: numero di n-grammi distinti errato\n";
        return false;
    }

    return true;
}

}

//esegue i test del contatore
int main() {

    bool success = true;

    //test dei quattro tipi di n-grammi con caratteri ASCII
    {
        const std::string text = "a b a b";

        ngram::NgramCounts counts;

        ngram::count_sequence(text, counts);

        //verifichiamo i bigrammi di parole
        success &= check_frequency(counts.word_bigrams, "a b", 2);
        success &= check_frequency(counts.word_bigrams, "b a", 1);

        //verifichiamo i trigrammi di parole
        success &= check_frequency(counts.word_trigrams, "a b a", 1);
        success &= check_frequency(counts.word_trigrams, "b a b", 1);

        //verifichiamo i bigrammi di caratteri, compresi gli spazi
        success &= check_frequency(counts.char_bigrams, "a ", 2);
        success &= check_frequency(counts.char_bigrams, " b", 2);
        success &= check_frequency(counts.char_bigrams, "b ", 1);
        success &= check_frequency(counts.char_bigrams, " a", 1);

        //verifichiamo i trigrammi di caratteri
        success &= check_frequency(counts.char_trigrams, "a b", 2);
        success &= check_frequency(counts.char_trigrams, " b ", 1);
        success &= check_frequency(counts.char_trigrams, "b a", 1);
        success &= check_frequency(counts.char_trigrams, " a ", 1);

        //verifichiamo il numero di n-grammi distinti
        success &= check_size(counts.word_bigrams, 2);
        success &= check_size(counts.word_trigrams, 2);
        success &= check_size(counts.char_bigrams, 4);
        success &= check_size(counts.char_trigrams, 4);
    }

    //test dei caratteri Unicode multibyte
    {
        const std::string text = ngram::normalize_utf8("à b à");

        ngram::NgramCounts counts;

        ngram::count_sequence(text, counts);

        //verifichiamo i bigrammi di parole
        success &= check_frequency(counts.word_bigrams, "à b", 1);
        success &= check_frequency(counts.word_bigrams, "b à", 1);

        //verifichiamo il trigramma di parole
        success &= check_frequency(counts.word_trigrams, "à b à", 1);

        //verifichiamo i bigrammi di caratteri Unicode
        success &= check_frequency(counts.char_bigrams, "à ", 1);
        success &= check_frequency(counts.char_bigrams, " b", 1);
        success &= check_frequency(counts.char_bigrams, "b ", 1);
        success &= check_frequency(counts.char_bigrams, " à", 1);

        //verifichiamo i trigrammi di caratteri Unicode
        success &= check_frequency(counts.char_trigrams, "à b", 1);
        success &= check_frequency(counts.char_trigrams, " b ", 1);
        success &= check_frequency(counts.char_trigrams, "b à", 1);
    }

    //test del testo vuoto
    {
        ngram::NgramCounts counts;

        ngram::count_sequence("", counts);

        success &= check_size(counts.word_bigrams, 0);
        success &= check_size(counts.word_trigrams, 0);
        success &= check_size(counts.char_bigrams, 0);
        success &= check_size(counts.char_trigrams, 0);
    }

    if (!success) {
        return 1;
    }

    std::cout << "tutti i test del contatore sono stati superati\n";

    return 0;
}

