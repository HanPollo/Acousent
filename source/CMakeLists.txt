set(ACOUSENT_HEADERS
        OpenALErrorCheck.h
        SoundDevice.h
        SoundLibrary.h
        SoundSource.h
        TopDown.h
        )
set(ACOUSENT_SOURCES
        SoundDevice.cpp
        SoundLibrary.cpp
        SoundSource.cpp
        )

add_library(acousent STATIC ${ACOUSENT_SOURCES} ${ACOUSENT_HEADERS})
if (MSVC)
    target_compile_options(acousent PUBLIC /wd5033)
endif(MSVC)
target_include_directories(acousent PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ACOUSENT_INCLUDE_DIRECTORY)
target_link_libraries(acousent PRIVATE ${THIRD_PARTY_LIBRARIES})
set_property(TARGET acousent PROPERTY CXX_STANDARD 20)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ACOUSENT_SOURCES} ${ACOUSENT_HEADERS})


add_executable(main main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 20)
target_link_libraries(main PRIVATE acousent)
target_include_directories(main PRIVATE ${ACOUSENT_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
add_custom_command(TARGET main POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:OpenAL> $<TARGET_FILE_DIR:main>)

