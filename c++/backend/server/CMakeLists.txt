# Server application using BankID library and CrowCpp

cmake_minimum_required(VERSION 3.16)
project(BankIDServer)

# Find required packages
find_package(Crow REQUIRED)

# Server executable
add_executable(bankid_server main.cpp)

# Include directories for BankID headers
target_include_directories(bankid_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../bankid/includes
)

# Link libraries
target_link_libraries(bankid_server PRIVATE 
    Crow::Crow
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
