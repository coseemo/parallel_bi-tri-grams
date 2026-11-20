#include "tokenizer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_normalize_utf8() {
    require(
        ngram::normalize_utf8("  Caffè\tMondo!  ") == "caffè mondo",
        "normalizzazione di maiuscole, spazi e punteggiatura"
    );

    require(
        ngram::normalize_utf8("L’amour e-mail 42") == "l amour e mail 42",
        "apostrofi, trattini e cifre"
    );

    require(
        ngram::normalize_utf8("Straße") == "strasse",
        "case folding Unicode completo"
    );

    const std::string decomposed = "caffe\xCC\x80";
    require(
        ngram::normalize_utf8(decomposed) == "caffè",
        "normalizzazione NFC di un carattere composto"
    );

    require(ngram::normalize_utf8("---!!!") == "", "testo solo separatori");
    require(ngram::normalize_utf8("").empty(), "testo vuoto");
}

void test_find_word_spans() {
    const std::string text = "caffè mondo";
    const std::vector<ngram::Span> spans = ngram::find_word_spans(text);

    require(spans.size() == 2, "numero di parole individuate");
    require(ngram::span_view(text, spans[0]) == "caffè", "prima parola");
    require(ngram::span_view(text, spans[1]) == "mondo", "seconda parola");
    require(spans[0].offset == 0 && spans[0].length == 6, "span UTF-8 della prima parola");
    require(spans[1].offset == 7 && spans[1].length == 5, "span della seconda parola");

    const auto repeated_spaces = ngram::find_word_spans("  una   prova  ");
    require(repeated_spaces.size() == 2, "gestione degli spazi ripetuti");
}

void test_find_codepoint_spans() {
    const std::string text = "à b";
    const std::vector<ngram::Span> spans = ngram::find_codepoint_spans(text);

    require(spans.size() == 3, "gli spazi devono essere mantenuti come code point");
    require(ngram::span_view(text, spans[0]) == "à", "primo code point");
    require(ngram::span_view(text, spans[1]) == " ", "spazio come code point");
    require(ngram::span_view(text, spans[2]) == "b", "ultimo code point");
    require(spans[0].length == 2, "il carattere à occupa due byte in UTF-8");
}

void test_invalid_utf8() {
    const std::string invalid_utf8("\xC3", 1);
    bool normalization_threw = false;
    bool span_search_threw = false;

    try {
        (void)ngram::normalize_utf8(invalid_utf8);
    } catch (const std::invalid_argument&) {
        normalization_threw = true;
    }

    try {
        (void)ngram::find_codepoint_spans(invalid_utf8);
    } catch (const std::invalid_argument&) {
        span_search_threw = true;
    }

    require(normalization_threw, "UTF-8 non valido rifiutato dalla normalizzazione");
    require(span_search_threw, "UTF-8 non valido rifiutato dalla scansione dei code point");
}

void test_span_view_bounds() {
    bool threw = false;
    try {
        (void)ngram::span_view("ciao", ngram::Span{3, 2});
    } catch (const std::out_of_range&) {
        threw = true;
    }
    require(threw, "span fuori dai limiti rifiutato");
}


void test_split_sentences_utf8() {

    //test della suddivisione di un testo in più frasi
    const std::string text =
        "Il cielo è blu. Domani pioverà! Che bella giornata?";

    const std::vector<std::string> sentences =
        ngram::split_sentences_utf8(text);

    //verifichiamo che siano state individuate tre frasi
    require(sentences.size() == 3, "numero di frasi individuate");

    //verifichiamo il contenuto delle frasi
    require(
        sentences[0] == "Il cielo è blu. ",
        "contenuto della prima frase"
    );

    require(
        sentences[1] == "Domani pioverà! ",
        "contenuto della seconda frase"
    );

    require(
        sentences[2] == "Che bella giornata?",
        "contenuto della terza frase"
    );

    //test di un testo senza punteggiatura finale
    const auto single_sentence =
        ngram::split_sentences_utf8("Ciao mondo");

    require(
        single_sentence.size() == 1,
        "testo senza punteggiatura"
    );

    //test di un testo vuoto
    const auto empty_sentences =
        ngram::split_sentences_utf8("");

    require(
        empty_sentences.empty(),
        "gestione del testo vuoto"
    );

    //test di un input UTF-8 non valido
    const std::string invalid_utf8("\xC3", 1);

    bool threw = false;

    try {
        (void)ngram::split_sentences_utf8(invalid_utf8);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(
        threw,
        "rifiuto di sequenze UTF-8 non valide nella segmentazione"
    );
}


}

int main() {
    try {
        test_normalize_utf8();
        test_find_word_spans();
        test_find_codepoint_spans();
        test_invalid_utf8();
        test_span_view_bounds();
	test_split_sentences_utf8();
    } catch (const std::exception& error) {
        std::cerr << "TEST FALLITO: " << error.what() << '\n';
        return 1;
    }

    std::cout << "Tutti i test del tokenizer sono superati.\n";
    return 0;
}
