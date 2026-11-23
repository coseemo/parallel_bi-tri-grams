
#pragma once

#include "ngram_counter.hpp"

#include <filesystem>
#include <vector>

namespace ngram {

//elabora sequenzialmente tutti i documenti e accumula le frequenze degli n-grammi
void process_documents_sequentially(
    const std::vector<std::filesystem::path>& files,
    NgramCounts& counts
);

} // namespace ngram

