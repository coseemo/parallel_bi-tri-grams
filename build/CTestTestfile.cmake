# CMake generated Testfile for 
# Source directory: /home/cosimo/progetti/parallel
# Build directory: /home/cosimo/progetti/parallel/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("tokenizer_tests" "/home/cosimo/progetti/parallel/build/test_tokenizer")
set_tests_properties("tokenizer_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;84;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
add_test("ngram_counter_tests" "/home/cosimo/progetti/parallel/build/test_ngram_counter")
set_tests_properties("ngram_counter_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;100;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
add_test("document_reader_tests" "/home/cosimo/progetti/parallel/build/test_document_reader")
set_tests_properties("document_reader_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/cosimo/progetti/parallel/CMakeLists.txt;116;add_test;/home/cosimo/progetti/parallel/CMakeLists.txt;0;")
