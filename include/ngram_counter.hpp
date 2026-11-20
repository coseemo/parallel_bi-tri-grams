#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ngram {

//tabella che associa ogni n-gramma alla sua frequenza
//usiamo un alias "FrequencyTable" per riscrivere
//ogni volta il tipo std::unorderd_map<std::string, std::size_t>
using FrequencyTable = std::unordered_map<std::string, std::size_t>;

//struttura che contiene le quattro tabelle delle frequenze
struct NgramCounts {
    FrequencyTable word_bigrams;
    FrequencyTable word_trigrams;
    FrequencyTable char_bigrams;
    FrequencyTable char_trigrams;
};

//conta gli n-grammi presenti in una sequenza di testo già normalizzata
void count_sequence(
    //testo normalizzato dal tokenizer
    std::string_view normalized_text,
    //struttura in cui aggiornare le frequenze
    NgramCounts& counts
);

void count_sequence(
    std::string_view normalized_text,
    NgramCounts& counts
);

//normalizza e conta separatamente tutte le frasi di un documento
void count_document(
    std::string_view document_text,
    NgramCounts& counts
);

}

