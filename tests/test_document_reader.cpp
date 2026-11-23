
#include "document_reader.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

//verifica che una condizione sia rispettata
bool check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "test fallito: " << message << "\n";
        return false;
    }

    return true;
}

//crea un file di testo con il contenuto indicato
bool write_test_file(
    const fs::path& path,
    const std::string& content
) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    file.write(content.data(), static_cast<std::streamsize>(content.size()));

    return file.good();
}

}

//esegue i test del lettore dei documenti
int main() {

    bool success = true;

    //creiamo una cartella temporanea con un nome univoco
    const auto timestamp =
        std::chrono::steady_clock::now().time_since_epoch().count();

    const fs::path test_directory =
        fs::temp_directory_path() /
        ("ngram_reader_test_" + std::to_string(timestamp));

    try {

        //creiamo la cartella di test e una sottocartella
        fs::create_directories(test_directory / "subfolder");

        //creiamo due file di testo
        success &= check(
            write_test_file(test_directory / "a.txt", "Caffè mondo"),
            "creazione del primo file"
        );

        success &= check(
            write_test_file(test_directory / "b.txt", "Secondo documento"),
            "creazione del secondo file"
        );

        //creiamo un file con estensione diversa
        success &= check(
            write_test_file(test_directory / "ignored.pdf", "contenuto"),
            "creazione del file da ignorare"
        );

        //creiamo un file txt dentro una sottocartella
        success &= check(
            write_test_file(
                test_directory / "subfolder" / "nested.txt",
                "test"
            ),
            "creazione del file nella sottocartella"
        );

        //creiamo un file vuoto
        success &= check(
            write_test_file(test_directory / "empty.txt", ""),
            "creazione del file vuoto"
        );

        //verifichiamo l'individuazione dei file txt
        const auto files = ngram::list_text_files(test_directory);

        success &= check(
            files.size() == 3,
            "individuazione dei soli file txt nella cartella principale"
        );

        //verifichiamo che i percorsi siano ordinati alfabeticamente
        if (files.size() == 3) {
            success &= check(
                files[0].filename() == "a.txt" &&
                files[1].filename() == "b.txt" &&
                files[2].filename() == "empty.txt",
                "ordinamento alfabetico dei file"
            );
        }

        //verifichiamo la lettura di un file con caratteri UTF-8
        const std::string first_content =
            ngram::read_text_file(test_directory / "a.txt");

        success &= check(
            first_content == "Caffè mondo",
            "lettura corretta del contenuto UTF-8"
        );

        //verifichiamo la lettura di un secondo file
        const std::string second_content =
            ngram::read_text_file(test_directory / "b.txt");

        success &= check(
            second_content == "Secondo documento",
            "lettura del secondo documento"
        );

        //verifichiamo la lettura di un file vuoto
        const std::string empty_content =
            ngram::read_text_file(test_directory / "empty.txt");

        success &= check(
            empty_content.empty(),
            "lettura di un file vuoto"
        );

        //verifichiamo la gestione di una cartella inesistente
        bool missing_directory_error = false;

        try {
            (void)ngram::list_text_files(
                test_directory / "missing"
            );
        } catch (const std::runtime_error&) {
            missing_directory_error = true;
        }

        success &= check(
            missing_directory_error,
            "errore per una cartella inesistente"
        );

        //verifichiamo la gestione di un percorso che non è una cartella
        bool not_directory_error = false;

        try {
            (void)ngram::list_text_files(
                test_directory / "a.txt"
            );
        } catch (const std::runtime_error&) {
            not_directory_error = true;
        }

        success &= check(
            not_directory_error,
            "errore quando il percorso non è una cartella"
        );

        //verifichiamo la gestione di un file inesistente
        bool missing_file_error = false;

        try {
            (void)ngram::read_text_file(
                test_directory / "missing.txt"
            );
        } catch (const std::runtime_error&) {
            missing_file_error = true;
        }

        success &= check(
            missing_file_error,
            "errore durante la lettura di un file inesistente"
        );

    } catch (const std::exception& error) {

        //se si verifica un errore inatteso lo segnaliamo
        std::cerr << "errore durante i test: "
                  << error.what() << "\n";

        success = false;
    }

    //eliminiamo la cartella temporanea utilizzata dai test
    std::error_code cleanup_error;
    fs::remove_all(test_directory, cleanup_error);

    if (cleanup_error) {
        std::cerr << "impossibile eliminare la cartella temporanea\n";
        success = false;
    }

    if (!success) {
        return 1;
    }

    std::cout << "tutti i test del document reader sono stati superati\n";

    return 0;
}
