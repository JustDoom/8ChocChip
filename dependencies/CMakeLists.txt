include(FetchContent)

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML"
        GIT_TAG 69ea0cd863aed1d4092b970b676924a716ff718b # 2.6.1
)

add_subdirectory(sfml)