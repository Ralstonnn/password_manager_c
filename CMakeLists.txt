cmake_minimum_required(VERSION 3.10)

# set output directory for binary
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

# set the project name
project(PasswordManagerApp)

include_directories(include/)

# add the executable
add_executable(PasswordManager src/main.c src/common.c src/interface.c src/fs.c)