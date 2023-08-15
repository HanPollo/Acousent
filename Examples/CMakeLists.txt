function(MakeExample TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})
	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER Examples)
	target_link_libraries(${TARGETNAME} PRIVATE acousent)
	target_include_directories(${TARGETNAME} PRIVATE ${ACOUSENT_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
	add_custom_command(TARGET ${TARGETNAME} POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:OpenAL> $<TARGET_FILE_DIR:${TARGETNAME}>)



endfunction(MakeExample)

add_compile_definitions(AL_LIBTYPE_STATIC)
#MakeExample(top_down top_down.cpp)
MakeExample(MIDI_Controller midi_controller.cpp)
MakeExample(SoundFont soundfont_notes.cpp)
MakeExample(midi_lecture midi_playback.cpp)
MakeExample(files_info files_info.cpp)
