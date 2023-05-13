set(CMAKE_LEGACY_CYGWIN_WIN32 OFF)
cmake_minimum_required(VERSION 3.15)
project(BoomBoom C CXX)
find_package(OpenGL REQUIRED)
#add_definitions(-D_USE_MATH_DEFINES)
set(THIRD_PARTY_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-1.78"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/openal-soft/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/openmesh/src"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/dr_libs"
									"${CMAKE_BINARY_DIR}/third_party/assimp/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/MainLoop"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/entt/src")
set(BOOM_BOOM_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/source")

if(MSVC)
    set(THIRD_PARTY_LIBRARIES glad glfw ${OPENGL_LIBRARIES} ImGui ${BULLET_LIBRARIES} OpenAL dr_libs assimp stb OpenMeshCore)
else()
    set(THIRD_PARTY_LIBRARIES glad glfw ${OPENGL_LIBRARIES} ImGui ${BULLET_LIBRARIES} stdc++fs OpenAL dr_libs assimp stb OpenMeshCore)
endif(MSVC)



configure_file(CMakeConfigFiles/root_directory.h.in "${CMAKE_SOURCE_DIR}/source/root_directory.h")
add_subdirectory(third_party)
add_subdirectory(source)
add_subdirectory(Examples)
#add_subdirectory(Engine)

