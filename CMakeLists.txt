
cmake_minimum_required(VERSION 3.20)

#nome del progetto e linguaggio utilizzato
project(BigramTrigramOpenMP
    VERSION 1.0
    LANGUAGES CXX
)

#utilizziamo lo standard c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#disabilitiamo le estensioni specifiche 
#del compilatore
set(CMAKE_CXX_EXTENSIONS OFF)

#utilizziamo ICU per gestire correttamente
# i caratteri UNICODE:
#UTF-8 rappresenta caratteri tramite
#più byte quindi non possiamo fare cose 
#come for(char c : text)
#ICU ci fornisce alcune funzionalità per:
#interpretare e normalizzare,
# convertire maiuscole e minuscole ecc...

#qui cerchiamo la libreria nel sistema:
#REQUIRED ci dice che è obbligatoria
#altrimenti la configurazione fallisce.
#uc sta per UNICODE Common,
#è la componente di ICU che ci serve
find_package(ICU REQUIRED COMPONENTS uc)

#qui andiamo a creare una libreria c++
#chiamata text_processing 
#che contiene il tokenizer
#facciamo così per rendere
#il codice più ordinato
add_library(text_processing
    src/tokenizer.cpp
)

#questo ci dice al progetto
#dove trovare gli header del nostro progetto
target_include_directories(text_processing
    PUBLIC include
)

#qui andiamo a collegare le diverse librerie
target_link_libraries(text_processing
    PUBLIC ICU::uc
)

#qui diciamo al compilatore di esser rigoroso:
#-wall = abilita un insieme di avvisi comuni
#-wextra ulteriori avvisi
#wpedantic segnala utili non conformi 
#allo standard
#PRIVATE = per applicarlo solo 
#a text_processing
target_compile_options(text_processing
    PRIVATE -Wall -Wextra -Wpedantic
)

#qui andiamo a creare un eseguibile
add_executable(test_tokenizer
    tests/test_tokenizer.cpp
)

#qui andiamo a specificare le
#librerie utilizzate dall'eseguibile
target_link_libraries(test_tokenizer
    PRIVATE text_processing
)

#qui andiamo ad abilitare il sistema
#di testing CTest
enable_testing()

#qui registriamo un test:
#NAME nome del test
#COMMAND quale programma si esegue
add_test(
    NAME tokenizer_tests
    COMMAND test_tokenizer
)

