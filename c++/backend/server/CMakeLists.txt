# Example CMakeLists.txt for using the BankID library in another project

cmake_minimum_required(VERSION 3.16)
project(BankIdAuthExample)

# Find the BankID package
find_package(BankID REQUIRED)

# Create your executable
add_executable(BankIdAuthExample main.cpp)

# Link with the BankID library
target_link_libraries(BankIdAuthExample PRIVATE BankID::bankid_lib)

# If you're using Windows and shared libraries, you might need to copy the DLL
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BankIdAuthExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:BankID::bankid_lib>
        $<TARGET_FILE_DIR:BankIdAuthExample>
    )
endif()
