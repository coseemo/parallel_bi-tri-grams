#include "ngram_counter.hpp"
#include "tokenizer.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace ngram {

//conta i bigrammi e i trigrammi di parole e caratteri
void count_sequence(
    std::string_view normalized_text,
    NgramCounts& counts
) {
    //se il testo è vuoto non abbiamo nulla da contare
    if (normalized_text.empty()) {
        return;
    }

    //individuiamo tutte le parole presenti nel testo
    const std::vector<Span> word_spans =
        find_word_spans(normalized_text);

    //buffer temporaneo riutilizzato per costruire gli n-grammi
    std::string key;

    //conteggio dei bigrammi di parole
    for (std::size_t i = 0; i + 1 < word_spans.size(); ++i) {

        //recuperiamo la prima parola del bigramma
        const std::string_view first_word =
            span_view(normalized_text, word_spans[i]);

        //recuperiamo la seconda parola del bigramma
        const std::string_view second_word =
            span_view(normalized_text, word_spans[i + 1]);

        //svuotiamo il buffer senza eliminarne la capacità
        key.clear();

        //riserviamo lo spazio necessario per le due parole e il separatore
        key.reserve(first_word.size() + 1 + second_word.size());

        //costruiamo il bigramma separando le due parole con uno spazio
        key.append(first_word.data(), first_word.size());
        key.push_back(' ');
        key.append(second_word.data(), second_word.size());

        //incrementiamo la frequenza del bigramma
        ++counts.word_bigrams[key];
    }

    //conteggio dei trigrammi di parole
    for (std::size_t i = 0; i + 2 < word_spans.size(); ++i) {

        //recuperiamo le tre parole del trigramma
        const std::string_view first_word =
            span_view(normalized_text, word_spans[i]);

        const std::string_view second_word =
            span_view(normalized_text, word_spans[i + 1]);

        const std::string_view third_word =
            span_view(normalized_text, word_spans[i + 2]);

        //svuotiamo il buffer temporaneo
        key.clear();

        //riserviamo lo spazio necessario per le tre parole e i separatori
        key.reserve(
            first_word.size() + 1 +
            second_word.size() + 1 +
            third_word.size()
        );

        //costruiamo il trigramma
        key.append(first_word.data(), first_word.size());
        key.push_back(' ');
        key.append(second_word.data(), second_word.size());
        key.push_back(' ');
        key.append(third_word.data(), third_word.size());

        //inseriamo la chiave nella mappa a zero e poi
        //incrementiamo la frequenza del trigramma
        ++counts.word_trigrams[key];
    }

    //individuiamo tutti i code point Unicode presenti nel testo
    //questo ci permette di ottenere qualcosa come:
    //
    //à b
    //
    //indice codepoint offset lunghezza
    //     0         à      0         2
    //     1                2         1
    //     2         b      3         1
    //
    //gli spazi vengono mantenuti come elementi della sequenza
    const std::vector<Span> codepoint_spans =
        find_codepoint_spans(normalized_text);

    //conteggio dei bigrammi di caratteri
    for (std::size_t i = 0; i + 1 < codepoint_spans.size(); ++i) {

        //recuperiamo i due code point del bigramma
        const std::string_view first_char =
            span_view(normalized_text, codepoint_spans[i]);

        const std::string_view second_char =
            span_view(normalized_text, codepoint_spans[i + 1]);

        //svuotiamo il buffer temporaneo
        //non ne allochiamo sempre una nuova per
        //per ridurre le ri-allocazioni di memoria
        key.clear();

        //riserviamo lo spazio necessario per i due code point
        key.reserve(first_char.size() + second_char.size());

        //concatenamo i due code point senza aggiungere separatori
        key.append(first_char.data(), first_char.size());
        key.append(second_char.data(), second_char.size());

        //incrementiamo la frequenza del bigramma
        ++counts.char_bigrams[key];
    }

    //conteggio dei trigrammi di caratteri
    for (std::size_t i = 0; i + 2 < codepoint_spans.size(); ++i) {

        //recuperiamo i tre code point del trigramma
        const std::string_view first_char =
            span_view(normalized_text, codepoint_spans[i]);

        const std::string_view second_char =
            span_view(normalized_text, codepoint_spans[i + 1]);

        const std::string_view third_char =
            span_view(normalized_text, codepoint_spans[i + 2]);

        //svuotiamo il buffer temporaneo
        key.clear();

        //riserviamo lo spazio necessario per i tre code point
        key.reserve(
            first_char.size() +
            second_char.size() +
            third_char.size()
        );

        //concatenamo i tre code point
        key.append(first_char.data(), first_char.size());
        key.append(second_char.data(), second_char.size());
        key.append(third_char.data(), third_char.size());

        //incrementiamo la frequenza del trigramma
        ++counts.char_trigrams[key];
    }
}


//normalizza e conta separatamente tutte le frasi di un documento
void count_document(
    std::string_view document_text,
    NgramCounts& counts
) {

    //se il documento è vuoto non abbiamo nulla da contare
    if (document_text.empty()) {
        return;
    }

    //dividiamo il documento in frasi utilizzando ICU
    const std::vector<std::string> sentences =
        split_sentences_utf8(document_text);

    //scorriamo tutte le frasi individuate
    for (const std::string& sentence : sentences) {

        //normalizziamo la frase corrente
        const std::string normalized_sentence =
            normalize_utf8(sentence);

        //controlliamo che la frase non sia vuota dopo la normalizzazione
        if (!normalized_sentence.empty()) {

            //contiamo gli n-grammi della frase normalizzata
            count_sequence(normalized_sentence, counts);
        }
    }
}


} // namespace ngram

