//ci serve a impedire che lo stesso header
//venga incluso più volte durante la compilazione, 
//evitando così dichiarazioni duplicate
#pragma once

//andiamo a includere alcune componenti
//della libreria standard del C++:
//
//include std::size_t che serve per rappresentare
//dimensioni e posizioni
#include <cstdef>
//gestion delle stringhe
#include <string>
//accesso alle stringhe senza copiarle
#include <string_view>
//memorizzazione dinamica degli elementi
#include <vector>

//utilizzo namespace per raggruppare
//funzionalità relative agli n-grams.
//evitiamo così eventuali conflitti
//con nomi utilizzati da altre librerie
namespace ngram{

//rappresenta una porzione di stringa usando
//le posizioni dei bytes.
//
//offset: posizione di partenza nella stringa UTF-8.
//lenght: numero di bytes occupato dalla porzione.
//
//esempio: in "ciao mondo" la parola mondo
//  ha Span span{5,5}
//
struct Span{
    std::size_t offset;
    std::size_t lenght;
};

//funzione che riceve una stringa di caratteri
//e la restituisce normalizzata in UTF-8:
//
//-normalizzazione unicode NFC
//  ex: U+00E0 à = U+0061 + U+0300 a + '
//-uniforma maiuscole e minuscole
//-sostituisce la punteggiatura e 
//  i simboli con separatori
//-spazi multipli sono collassati
//-rimozione spazi iniziali e finali
[[nodiscard]]
std::string normalize_utf8(std::string_view input)

//trova tutte le parole in un testo normalizzato
//e ritorna un vettore di span, ciascuno
//rappresentante la posizione e la lunghezza
//in byte di una parola
//
[[nodiscard]]
std::vector<Span> find_word_spans(std::string_view normalized)

//individua i singoli code point unicode presenti nel testo
//e restituisce un vettore, uno per ciascun code point.
//utilizzando la macro U8_NEXT di ICU
//
//anche gli spazi vengono considerati caratteri
//e quindi vengono mantenuti.
//la lunghezza di ogni span è espressa in byte
[[nodiscard]]
std::vector<Span> find_codepoint_spans(std::string_view utf8);

//permette di ottenere una vista della porzione di testo
//identificata da uno span, senza creare una nuova riga.
//
//riceve il resto originale e l'intervallo da estrarre
//restituisce uno string_view che si riferisce alla
//memoria originale
[[nodiscard]]
std::string_view span_view(
        std::string_view text,
        Span span
        )

}
