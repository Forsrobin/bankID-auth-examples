# Server application using BankID library and CrowCpp

cmake_minimum_required(VERSION 3.16)
project(BankIDServer)

# Enable SSL support for Crow
set(CROW_ENABLE_SSL ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(Crow REQUIRED)

# Server executable
add_executable(bankid_server main.cpp)

# Enable SSL compilation flag
target_compile_definitions(bankid_server PRIVATE CROW_ENABLE_SSL)

# Include directories for BankID headers
target_include_directories(bankid_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../bankid/includes
)

# Link libraries
target_link_libraries(bankid_server PRIVATE 
    Crow::Crow
    OpenSSL::SSL
    OpenSSL::Crypto
    bankid_lib  # Link directly to our bankid library target
)

# Set executable properties
set_target_properties(bankid_server PROPERTIES
    OUTPUT_NAME "bankid_server"
)

# If using shared libraries on Windows, copy the DLL
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET bankid_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:bankid_lib>
        $<TARGET_FILE_DIR:bankid_server>
        COMMENT "Copying BankID DLL to server output directory"
    )
endif()

# Copy certificates folder to output directory
add_custom_command(TARGET bankid_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/certs
    $<TARGET_FILE_DIR:bankid_server>/certs
    COMMENT "Copying certificates folder to server output directory"
)

# Also copy scripts folder for certificate conversion utilities
add_custom_command(TARGET bankid_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:bankid_server>/scripts
    COMMENT "Copying certificate conversion scripts to server output directory"
)
