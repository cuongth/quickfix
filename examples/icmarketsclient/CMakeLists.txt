if (HAVE_SSL AND WIN32)
set(applink_SOURCE ${OPENSSL_INCLUDE_DIR}/openssl/applink.c)
endif()

add_executable(icmarketsclient FixApp.cpp icmarketsclient.cpp ${applink_SOURCE})

target_include_directories(icmarketsclient PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/quickfix ${PROJECT_SOURCE_DIR})


target_link_libraries(icmarketsclient ${PROJECT_NAME})

if (NOT WIN32)
ADD_CUSTOM_TARGET(icmarketsclient_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:icmarketsclient> ${PROJECT_SOURCE_DIR}/bin/icmarketsclient)
else()
set_target_properties(icmarketsclient PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/debug/icmarketsclient/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release/icmarketsclient/
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin/release/icmarketsclient/)
endif()
