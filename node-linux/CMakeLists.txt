cmake_minimum_required(VERSION 3.10)
project(zanni-node-linux LANGUAGES C)

# Подключаем папку core, которая находится на уровень выше.
# Второй аргумент ${CMAKE_CURRENT_BINARY_DIR}/core_build нужен,
# потому что исходники core лежат вне дерева исходников node-linux.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core ${CMAKE_CURRENT_BINARY_DIR}/core_build)

# Создаем исполняемый файл (наш бинарник)
add_executable(zanni-node-linux src/main.c)

# Линкуем статическую библиотеку zanni-node-core к нашему бинарнику.
# Поскольку core экспортирует свои include директории (target_include_directories(... PUBLIC ...)),
# нам не нужно вручную прописывать include_directories здесь, CMake сделает это автоматически.
target_link_libraries(zanni-node-linux PRIVATE zanni-node-core)
