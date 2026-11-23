
#include "sequential_processor.hpp"
#include "document_reader.hpp"

#include <string>

namespace ngram {

//elabora sequenzialmente tutti i documenti e accumula le frequenze degli n-grammi
void process_documents_sequentially(
    const std::vector<std::filesystem::path>& files,
    NgramCounts& counts
) {

    //scorriamo tutti i documenti presenti nel vettore
    for (const auto& file_path : files) {

        //leggiamo il contenuto del documento corrente
        const std::string document_text =
            read_text_file(file_path);

        //elaboriamo il documento e aggiorniamo le frequenze degli n-grammi
        count_document(document_text, counts);
    }
}

} // namespace ngram
