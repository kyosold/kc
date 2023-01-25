project(KCRYPTO)

include_directories(/usr/local/openssl-1.1.1m/include)
link_directories(/usr/local/openssl-1.1.1m/lib)

add_executable(test test.c kcrypto.c)
target_link_libraries(test -lssl -lcrypto)