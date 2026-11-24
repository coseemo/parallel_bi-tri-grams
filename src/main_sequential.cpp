#include "document_reader.hpp"
#include "sequential_processor.hpp"

#include <chrono>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

//calcola il numero totale di occorrenze presenti in una tabella
std::size_t total_occurrences(const ngram::FrequencyTable& table) {

    std::size_t total = 0;

    for (const auto& entry : table) {
        total += entry.second;
    }

    return total;
}

//stampa il numero di n-grammi distinti e il totale delle occorrenze
void print_table_summary(
    const std::string& label,
    const ngram::FrequencyTable& table
) {

    std::cout << label << ":\n";

    std::cout << "  N-grammi distinti: "
              << table.size() << '\n';

    std::cout << "  Occorrenze totali: "
              << total_occurrences(table) << '\n';
}

} // namespace

int main(int argc, char* argv[]) {

    //controlliamo se e' stata specificata una directory personalizzata
    if (argc > 2) {
        std::cerr << "Uso: ngram_sequential [cartella_dati]\n";
        return 1;
    }

    //utilizziamo la directory specificata oppure quella predefinita
    const std::filesystem::path data_directory =
        (argc == 2) ? argv[1] : "data";

    try {

        //recuperiamo l'elenco dei documenti da elaborare
        const auto files =
            ngram::list_text_files(data_directory);

        //controlliamo che la directory contenga almeno un documento
        if (files.empty()) {
            std::cerr << "Nessun file .txt trovato in: "
                      << data_directory << '\n';
            return 1;
        }

        //inizializziamo le tabelle delle frequenze
        ngram::NgramCounts counts;

        //registriamo l'inizio della misurazione
        const auto start = std::chrono::steady_clock::now();

        //elaboriamo sequenzialmente tutti i documenti
        ngram::process_documents_sequentially(files, counts);

        //registriamo la fine della misurazione
        const auto end = std::chrono::steady_clock::now();

        //calcoliamo il tempo trascorso in secondi
        const std::chrono::duration<double> elapsed = end - start;

        //stampiamo le informazioni generali dell'elaborazione
        std::cout << "=== RISULTATI ELABORAZIONE SEQUENZIALE ===\n";

        std::cout << "Directory: "
                  << data_directory << '\n';

        std::cout << "Documenti elaborati: "
                  << files.size() << '\n';

        std::cout << "Tempo di elaborazione: "
                  << elapsed.count() << " secondi\n\n";

        //stampiamo il riepilogo delle quattro distribuzioni
        print_table_summary(
            "Bigrammi di parole",
            counts.word_bigrams
        );

        std::cout << '\n';

        print_table_summary(
            "Trigrammi di parole",
            counts.word_trigrams
        );

        std::cout << '\n';

        print_table_summary(
            "Bigrammi di caratteri",
            counts.char_bigrams
        );

        std::cout << '\n';

        print_table_summary(
            "Trigrammi di caratteri",
            counts.char_trigrams
        );

    } catch (const std::exception& error) {

        //se si verifica un errore, stampiamo il messaggio e terminiamo
        std::cerr << "Errore: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
