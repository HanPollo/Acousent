set(ACOUSENT_HEADERS
        "Audio/OpenALErrorCheck.h"
        "Audio/SoundDevice.h"
        "Audio/SoundLibrary.h"
        "Audio/SoundSource.h"

        "Core/TopDown.h"
        "Core/utils.h"
        "Core/Window.h"
        #"Core/Application.h"

        "Objects/Object.h"
        "Objects/Speaker.h"
        "Objects/Instrument.h"
        "Objects/Seat.h"
        "Objects/SeatCoords.h"

        "GUI/imgui-style.h"

        "Rendering/Camera.h"
        "Rendering/Mesh.h"
        "Rendering/Model.h"
        "Rendering/Shader.h"
        "Rendering/ModelManager.h"

        )
set(ACOUSENT_SOURCES
        "Core/utils.cpp"
        "Core/Window.cpp"
        #"Core/Application.cpp"

        "Objects/Object.cpp"
        "Objects/Speaker.cpp"
        "Objects/Instrument.cpp"
        "Objects/Seat.cpp"

        "Audio/SoundDevice.cpp"
        "Audio/SoundLibrary.cpp"
        "Audio/SoundSource.cpp"
        
        "Rendering/Camera.cpp"
        "Rendering/Mesh.cpp"
        "Rendering/Model.cpp"
        "Rendering/Shader.cpp"
        "Rendering/ModelManager.cpp"
        )
add_library(acousent STATIC ${ACOUSENT_SOURCES} ${ACOUSENT_HEADERS} Core/system.h ${Shaders})
if (MSVC)
    target_compile_options(acousent PUBLIC /wd5033)
endif(MSVC)
target_include_directories(acousent PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ACOUSENT_INCLUDE_DIRECTORY)
target_link_libraries(acousent PRIVATE ${THIRD_PARTY_LIBRARIES})
set_property(TARGET acousent PROPERTY CXX_STANDARD 20)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ACOUSENT_SOURCES} ${ACOUSENT_HEADERS})


function(MakeExample TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})
	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER Examples)
	target_link_libraries(${TARGETNAME} PRIVATE acousent)
	target_include_directories(${TARGETNAME} PRIVATE ${ACOUSENT_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
	add_custom_command(TARGET ${TARGETNAME} POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:OpenAL> $<TARGET_FILE_DIR:${TARGETNAME}>)
    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw> $<TARGET_FILE_DIR:${TARGETNAME}>)
    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:${TARGETNAME}>)
    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:rtmidi> $<TARGET_FILE_DIR:${TARGETNAME}>)



endfunction(MakeExample)

add_compile_definitions(AL_LIBTYPE_STATIC)
MakeExample(main main.cpp)

