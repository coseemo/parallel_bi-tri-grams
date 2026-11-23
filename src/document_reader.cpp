
#include "document_reader.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <cstdint>

namespace ngram {

//restituisce i percorsi dei file .txt presenti direttamente nella cartella
std::vector<std::filesystem::path> list_text_files(
    const std::filesystem::path& directory
) {

    //controlliamo che la cartella esista
    if (!std::filesystem::exists(directory)) {
        throw std::runtime_error("Directory does not exist: " + directory.string());
    }

    //controlliamo che il percorso indicato sia una cartella
    if (!std::filesystem::is_directory(directory)) {
        throw std::runtime_error("Path is not a directory: " + directory.string());
    }

    //vettore che conterrà i percorsi dei file individuati
    std::vector<std::filesystem::path> files;

    //scorriamo gli elementi presenti direttamente nella cartella
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {

        //controlliamo che l'elemento sia un file normale
        if (entry.is_regular_file()) {

            //recuperiamo il percorso del file
            const auto& file_path = entry.path();

            //controlliamo che l'estensione sia .txt
            if (file_path.extension() == ".txt") {

                //aggiungiamo il percorso al vettore
                files.push_back(file_path);
            }
        }
    }

    //ordiniamo alfabeticamente i percorsi per rendere riproducibile l'elaborazione
    std::sort(files.begin(), files.end());

    //restituiamo il vettore dei file individuati
    return files;
}

//legge il contenuto di un file di testo e lo restituisce come stringa
std::string read_text_file(
    const std::filesystem::path& file_path
) {

    //apriamo il file in modalità binaria e posizioniamoci alla fine
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    //controlliamo che il file sia stato aperto correttamente
    if (!file) {
        throw std::runtime_error("Unable to open file: " + file_path.string());
    }

    //recuperiamo la posizione finale del file
    const std::ifstream::pos_type end_position = file.tellg();

    //controlliamo che sia stato possibile determinare la dimensione
    if (end_position == std::ifstream::pos_type(-1)) {
        throw std::runtime_error("Unable to determine file size: " + file_path.string());
    }

    //convertiamo la posizione finale in una dimensione espressa in byte
    const std::streamoff file_size = end_position;

    //controlliamo che la dimensione sia valida
    if (file_size < 0) {
        throw std::runtime_error("Invalid file size: " + file_path.string());
    }

    //creiamo la stringa che conterrà il testo
    std::string content;

    //verifichiamo che la dimensione sia rappresentabile dalla stringa
    if (static_cast<std::uintmax_t>(file_size) >
        static_cast<std::uintmax_t>(content.max_size())) {
        throw std::length_error("File is too large: " + file_path.string());
    }

    //verifichiamo che la dimensione sia rappresentabile da streamsize
    if (file_size > std::numeric_limits<std::streamsize>::max()) {
        throw std::length_error("File is too large to read: " + file_path.string());
    }

    //allochiamo una stringa della dimensione esatta del file
    content.resize(static_cast<std::size_t>(file_size));

    //torniamo all'inizio del file
    file.seekg(0, std::ios::beg);

    //controlliamo che il riposizionamento sia riuscito
    if (!file) {
        throw std::runtime_error("Unable to seek file: " + file_path.string());
    }

    //leggiamo il contenuto soltanto se il file non è vuoto
    if (!content.empty()) {

        //leggiamo i byte direttamente nella memoria della stringa
        file.read(
            content.data(),
            static_cast<std::streamsize>(content.size())
        );

        //verifichiamo che siano stati letti tutti i byte previsti
        if (!file || file.gcount() != static_cast<std::streamsize>(content.size())) {
            throw std::runtime_error("Unable to read complete file: " + file_path.string());
        }
    }

    //restituiamo il contenuto del documento
    return content;
}

} // namespace ngram
