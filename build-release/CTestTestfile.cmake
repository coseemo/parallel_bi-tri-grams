# CMake generated Testfile for 
# Source directory: /home/cosimo/progetti/parallel
# Build directory: /home/cosimo/progetti/parallel/build-release
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("tokenizer_tests" "/home/cosimo/progetti/parallel/build-release/test_tokenizer")
set_tests_properties("tokenizer_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;85;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
add_test("ngram_counter_tests" "/home/cosimo/progetti/parallel/build-release/test_ngram_counter")
set_tests_properties("ngram_counter_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;101;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
add_test("document_reader_tests" "/home/cosimo/progetti/parallel/build-release/test_document_reader")
set_tests_properties("document_reader_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;117;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
add_test("sequential_processor_tests" "/home/cosimo/progetti/parallel/build-release/test_sequential_processor")
set_tests_properties("sequential_processor_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;128;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
