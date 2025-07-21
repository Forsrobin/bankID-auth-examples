# Example CMakeLists.txt for using the BankID library in another project

cmake_minimum_required(VERSION 3.16)
project(MyProject)

# Find the BankID package
find_package(BankID REQUIRED)

# Create your executable
add_executable(my_app main.cpp)

# Link with the BankID library
target_link_libraries(my_app PRIVATE BankID::bankid_lib)

# If you're using Windows and shared libraries, you might need to copy the DLL
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET my_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:BankID::bankid_lib>
        $<TARGET_FILE_DIR:my_app>
    )
endif()
