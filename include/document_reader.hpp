
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ngram {

//restituisce i percorsi dei file .txt presenti direttamente nella cartella
[[nodiscard]]
std::vector<std::filesystem::path> list_text_files(
    const std::filesystem::path& directory
);

//legge il contenuto di un file di testo e lo restituisce come stringa
[[nodiscard]]
std::string read_text_file(
    const std::filesystem::path& file_path
);

} // namespace ngram
