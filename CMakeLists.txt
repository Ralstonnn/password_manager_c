cmake_minimum_required(VERSION 3.10)

# set output directory for binary
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

# set the project name
project(PasswordManagerApp)

find_package(SQLite3 REQUIRED)

# add header files dir
include_directories(${PROJECT_SOURCE_DIR}/include)

# add the executable
add_executable(PasswordManager src/main.c src/common.c src/os.c src/fs.c src/db.c src/interface.c src/terminal.c src/search.c src/copy_to_clipboard.c src/password_generator.c src/validations.c)

target_link_libraries(PasswordManager SQLite::SQLite3)
