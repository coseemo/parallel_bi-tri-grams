//importo le dichiarazioni di funzioni definite nell'header
#include "tokenizer.hpp"

//libreria per utilizzare tipi numerici a dimensione fissa
#include <cstdint>

//libreria per conoscere i limiti dei tipi numerici
#include <limits>

//libreria per gestire eccezioni standard
#include <stdexcept>

//librerie ICU necessarie per elaborare testo unicode
#include <unicode/normalizer2.h>
#include <unicode/stringpiece.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf16.h>
#include <unicode/utf8.h>

//inseriamo le funzionzalità del tokenizer dentro al
//namespace ngram

namespace ngram{

//namespace anonimo: contiene funzioni ausiliarie
//utilizzate esclusivamente all'interno di questo file.
//sono funzioni che non vogliamo esporre alle altre
//componenti del progetto.

    namespace {
        
        //ICU utilizza int32_t per rappresentare la lunghezza
        //delle stringhe.
        //questa funzione controlla che il testo non superi
        //il limite consentito
        int32_t checked_length(std::string_view text){

            //verifichiamo se la lungheza del testo
            //supera il valore massimo rappresentabile
            //da int32_t
            
            if(text.size() >
                static_cast<std::size_t>(
                std::numeric_limits<int32_t>::max()))
                    
                    //se il testo è troppo grande, interrompiamo
                    //e solleviamo un'eccezione
                    throw std::length_error(
                            "Input text is too large for ICU"
                            );
            //ritorniamo la lunghezza della stringa
            return static_cast<int32_t>(text.size());
        }
        
        //controlla che il testo contenga delle sequenze UTF-8 valide
        //altrimenti solleva un'eccezione
        void validate_utf8(std::string_view text){

            //recuperiamo la lunghezza del testo
            //nel formato richiesto da ICU
            //variabile che non posso modificare dopo inizializzazione
            const int32_t length = checked_length(text);

            //interpretiamo i dati della stringa come una sequenza
            //di byte senza modificarne il contenuto
            //
            //const auto* data: puntatore a dati che non posso modificare tramite
            //quel puntatore
            //
            //text.data(): restituisce un puntatore di tipo const char*
            //
            //reinterpret_cast...: lo convertiamo nel tipo che serve a ICU
            const auto* data =
                reinterpret_cast<const std::uint8_t*>(text.data());
            
            //indice che indica il byte attualmente utilizzato
            int32_t index = 0;

            //continuiamo fin quando non abbiamo analizzato
            //tutti i byte
            while (index < length) {

                //variabile nella quale verrà inserito
                //il code point unicode decodificato
                UChar32 codepoint;

                //decodificiamo il prossimo carattere UTF-8
                //l'indice viene aggiornato in base al
                //numero di byte utilizzati
                U8_NEXT(data, index, length, codepoint);

                //un valore negativo indica una sequenza UTF-8
                //non valida
                if(codepoint < 0) {

                    //interrompiamo l'elaborazione del testo
                    //non valido
                    throw std::invalid_argument(
                            "Invalid UTF-8 input"
                            );
                }
            }
        }

        //verifica se un code point unicode
        //sta usando una combinazione di segni
        //ex.: a + ' = à
        bool is_combining_mark(UChar32 cp) {

            //recuperiamo la categoria unicode del code point
            const auto category = u_charType(cp);

            //verifichiamo se appartiene a una delle categorie
            //dei segni combinanti
            return category == U_NON_SPACING_MARK ||
                   category == U_COMBINING_SPACING_MARK ||
                   category == U_ENCLOSING_MARK;
        }

        //normalizza una stringa UTF-8 secondo le regole
        //che abbiamo definito
        std::string normalize_utf8_impl(std::string_view input) {
            
            //se il testo è vuoto, restituiamo una stringa vuota
            if (input.empty()) {
                return{};
            }
            
            //verifichiamo che il testo sia valido per ICU
            validate_utf8(input);

            //variabile utilizzata da ICU per segnalare errori
            UErrorCode status = U_ZERO_ERROR;

            //utilizziamo il normalizzatore Unicode NFC
            //fornito da ICU
            const icu::Normalizer2* normalizer =
                icu::Normalizer2::getNFCInstance(status);

            //se non si riesce a iniziallizzare il normalizzatore,
            //si interrompe l'esecuzione
            if(U_FAILURE(status)) {
                throw std::runtime_error(
                    "Unable to inizialize ICU normalizer"
                        );
            }
            
            //convertiamo il testo UTF-8 in un oggetto
            //UnicodeString di ICU
            icu::UnicodeString unicode =
                icu::UnicodeString::fromUTF8(
                        icu::StringPiece(
                            input.data(),
                            checked_length(input)
                            )
                        );

            //creiamo una stringa destinata a contenere
            //il testo dopo la prima normalizzazione
            icu::UnicodeString normalized;

            //applichiamo la normalizzazione NFC
            normalizer->normalize(unicode, normalized, status);

            //controlliamo che l'operazione sia terminata correttamente
            if (U_FAILURE(status)) {
                throw std::runtime_error(
                    "Unicode normalization failed"
                );
            }
            
            //facciamo case folding completo
            //si usa questo e non tolower() perché non supporta alcuni
            //caratteri come strabe = strasse
            normalized.foldCase();

            //prepariamo un nuovo oggetto per la nuova normalizzazione
            icu::UnicodeString folded;

            //applichiamo di nuovo NFC dopo case folding
            normalizer->normalize(normalized, folded, status);

            //controlliamo se la seconda norm. è riuscita
            if(U_FAILURE(status)) {
                throw std::runtime_error(
                        "Normalization after case folding failed"
                        );
            }
            
            //contiene risultato normalizzato
            icu::UnicodeString result;

            //indica che abbiamo incontrato almeno un separatore
            //dopo aver già inserito il testo  nel risultato.
            //ci servirà a comprimere spazi multipli
            bool pending_space = false;

            //indica che ci troviamo all'interno di una parola.
            //ci servirà per capire se un segno combinante
            //appartiene alla parola precedente.
            bool inside_word = false;

            //scorriamo il testo analizzando un code point alla volta
            for (int32_t i = 0; i < folded.length();) {

                //recuperiamo il code point presente nella posizione corrente
                const UChar32 cp = folded.char32At(i);

                //avanziamo di uno o due elementi UTF-16
                //a seconda della rappresentazione del code point
                i += U16_LENGTH(cp);

                //verifichiamo se il carattere è una lettera o un numero
                const bool alphanumeric = u_isalnum(cp);

                //un segno combinante viene conservato
                //soltanto se appartiene a una parola già iniziata
                const bool combining =
                    inside_word && is_combining_mark(cp);

            //se il carattere è alfanumerico oppure un segno combinante valido,
            //lo manteniamo all'interno della parola
            if (alphanumeric || combining) {

                //se abbiamo incontrato separatori precedenti,
                //inseriamo un solo spazio prima della nuova parola.
                if (pending_space && !result.isEmpty()) {
                    result.append(static_cast<UChar32>(' '));
                }

                //aggiungiamo il carattere al risultato.
                result.append(cp);

                //abbiamo gestito l'eventuale spazio pendente.
                pending_space = false;

                //da questo momento siamo all'interno di una parola.
                inside_word = true;

            } else {

                //tutti gli elementi che non sono lettere,
                //numeri o segni combinanti validi diventano separatori
                //se il risultato non è vuoto, segnaliamo uno spazio pendente
                if (!result.isEmpty()) {
                    pending_space = true;
                }

                //abbiamo terminato la parola corrente.
                inside_word = false;
            }
            }

             //creiamo la stringa C++ che conterrà il risultato UTF-8
            std::string output;

            //convertiamo la stringa unicode elaborata di nuovo in UTF-8
            result.toUTF8String(output);

            //restituiamo il testo normalizzato
            return output;
        }
        
    }

    std::string normalize_utf8(std::string_view input) {
        return normalize_utf8_impl(input);
    }

    std::vector<Span> find_word_spans(std::string_view normalized) {
        std::vector<Span> spans;
        std::size_t index = 0;

        while (index < normalized.size()) {
            while (index < normalized.size() && normalized[index] == ' ') {
                ++index;
            }

            const std::size_t start = index;

            while (index < normalized.size() && normalized[index] != ' ') {
                ++index;
            }

            if (index > start) {
                spans.push_back(Span{start, index - start});
            }
        }

        return spans;
    }

    std::vector<Span> find_codepoint_spans(std::string_view utf8) {
        validate_utf8(utf8);

        std::vector<Span> spans;
        const int32_t length = checked_length(utf8);
        const auto* data =
            reinterpret_cast<const std::uint8_t*>(utf8.data());
        int32_t index = 0;

        while (index < length) {
            const int32_t start = index;
            UChar32 codepoint;
            U8_NEXT(data, index, length, codepoint);

            if (codepoint < 0) {
                throw std::invalid_argument("Invalid UTF-8 input");
            }

            spans.push_back(Span{
                static_cast<std::size_t>(start),
                static_cast<std::size_t>(index - start)
            });
        }

        return spans;
    }

    std::string_view span_view(std::string_view text, Span span) {
        if (span.offset > text.size() ||
            span.length > text.size() - span.offset) {
            throw std::out_of_range("Span is outside the text");
        }

        return text.substr(span.offset, span.length);
    }

}
