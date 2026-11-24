#include "document_reader.hpp"
#include "sequential_processor.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

//scrive il contenuto di un file di test
void write_file(
    const std::filesystem::path& file_path,
    const std::string& content
) {
    std::ofstream file(file_path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Impossibile creare il file di test");
    }

    file << content;

    if (!file) {
        throw std::runtime_error("Errore durante la scrittura del file di test");
    }
}

//verifica una condizione e segnala l'errore se non viene rispettata
void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

} // namespace

int main() {

    //creiamo una directory temporanea per i documenti di prova
    const std::filesystem::path test_directory =
        std::filesystem::temp_directory_path() / "ngram_sequential_test";

    try {

        //creiamo la directory, eliminando eventuali residui di test precedenti
        std::filesystem::remove_all(test_directory);
        std::filesystem::create_directories(test_directory);

        //creiamo due documenti con sequenze di parole differenti
        write_file(test_directory / "a.txt", "il gatto dorme");
        write_file(test_directory / "b.txt", "il gatto corre");

        //recuperiamo l'elenco dei documenti da elaborare
        const std::vector<std::filesystem::path> files =
            ngram::list_text_files(test_directory);

        //verifichiamo che siano stati individuati entrambi i documenti
        require(files.size() == 2, "Numero di documenti individuati non corretto");

        //inizializziamo le quattro tabelle delle frequenze
        ngram::NgramCounts counts;

        //elaboriamo sequenzialmente tutti i documenti
        ngram::process_documents_sequentially(files, counts);

        //verifichiamo i bigrammi di parole
        require(
            counts.word_bigrams.at("il gatto") == 2,
            "Frequenza errata per il bigramma 'il gatto'"
        );

        require(
            counts.word_bigrams.at("gatto dorme") == 1,
            "Frequenza errata per il bigramma 'gatto dorme'"
        );

        require(
            counts.word_bigrams.at("gatto corre") == 1,
            "Frequenza errata per il bigramma 'gatto corre'"
        );

        //verifichiamo i trigrammi di parole
        require(
            counts.word_trigrams.at("il gatto dorme") == 1,
            "Frequenza errata per il trigramma 'il gatto dorme'"
        );

        require(
            counts.word_trigrams.at("il gatto corre") == 1,
            "Frequenza errata per il trigramma 'il gatto corre'"
        );

        //verifichiamo che siano stati generati anche gli n-grammi di caratteri
        require(
            !counts.char_bigrams.empty(),
            "La tabella dei bigrammi di caratteri e' vuota"
        );

        require(
            !counts.char_trigrams.empty(),
            "La tabella dei trigrammi di caratteri e' vuota"
        );

        std::cout << "Test del processore sequenziale superato."
                  << std::endl;

        //rimuoviamo i documenti temporanei
        std::filesystem::remove_all(test_directory);

        return 0;

    } catch (const std::exception& error) {

        //rimuoviamo la directory temporanea anche in caso di errore
        std::error_code cleanup_error;
        std::filesystem::remove_all(test_directory, cleanup_error);

        std::cerr << "Test fallito: " << error.what() << std::endl;

        return 1;
    }
}
