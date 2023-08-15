#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp" 


#include "root_directory.h"

#include "Rendering/Shader.h"
#include "Rendering/Model.h"
#include "Rendering/Camera.h"

#include "Core/Window.h"

#include "Objects/Speaker.h"
#include "Objects/Instrument.h"
#include "Objects/Seat.h"

#include "Audio/SoundDevice.h"
#include "Audio/SoundLibrary.h"




namespace ac = Acousent;
using json = nlohmann::json;


const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// Set up camera
Camera camera(glm::vec3(-0.5f, 3.0f, 9.0f));
float deltaTime = .01667f;
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
char seatRow = 'A';
int seatColumn = 1;

//Speaker speaker2;
vector<Speaker> all_speakers;
vector<Instrument> all_instruments;
vector<SoundSource*> all_sources;
vector<Model*> all_models;
SoundLibrary* AudioLib = SoundLibrary::Get();

// JSON Configuration
json readJsonFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    json jsonObject;
    file >> jsonObject;

    return jsonObject;
}

void getAllJSONKeys(const json& jsonObject, std::vector<std::string>& keys) {
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        if (it->is_object()) {
            getAllJSONKeys(it.value(), keys);  // Recursively process nested objects
        }
        keys.push_back(it.key());
    }
}
//End JSON Config

// Process speakers from json
void processSpeakers(vector<Speaker> speakers, json distribution) {
    try {
        // Access the speaker objects in the array
        if (distribution.contains("speakers") && distribution["speakers"].is_array()) {
            for (const auto& speaker : distribution["speakers"]) {
                // Access the properties of each piano object
                float speakerX = speaker["position"]["x"];
                float speakerY = speaker["position"]["y"];
                float speakerZ = speaker["position"]["z"];
                std::string audioFileName = speaker["audio_file"];


                // Process the piano properties...
                
                //Model speaker_model(ac::getPath("Resources/Models/Speaker/scene.gltf").string());
                //all_models.push_back(&speaker_model);

                Speaker speaker1; // revisar in placeback

                //SoundSource source;
                //all_sources.push_back(&source);
                int speaker_sound = AudioLib->Load(ac::getPath("Resources/Audio/Wav/"+audioFileName).string().c_str());
                //speaker1.setModel(speaker_model);

                //speaker1.addAudioSource(source);
                speaker1.addSound(speaker_sound);
                //speaker1.SetLooping(true);
                
                // move speaker 1
                speaker1.Translate(speakerX, speakerY, speakerZ);
                speaker1.Update();
                all_speakers.push_back(speaker1);
                

                std::cout << "Speaker position: X=" << speakerX << ", Y=" << speakerY << ", Z=" << speakerZ << std::endl;
                std::cout << "Audio file name: " << audioFileName << std::endl;
                std::cout << std::endl;
            }
        }

      

    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

// Process instruments from json
void processInstruments(vector<Instrument> instruments, json distribution) {
    try {
        // Access the instrument objects in the array
        if (distribution.contains("instruments") && distribution["instruments"].is_array()) {
            for (const auto& instrument : distribution["instruments"]) {
                // Access the properties of each piano object
                float instrumentX = instrument["position"]["x"];
                float instrumentY = instrument["position"]["y"];
                float instrumentZ = instrument["position"]["z"];
                std::string midiFileName = instrument["midi_file"];
                std::string sfFileName = instrument["soundfont_file"];
                int sfPreset = -1;
                int midiTrack = -1;
                bool singleTrack = false;
                if (instrument.contains("midi_track") && instrument.contains("soundfont_preset"))
                    singleTrack = true;

                if (singleTrack) {
                    midiTrack = instrument["midi_track"];
                    sfPreset = instrument["soundfont_preset"];
                }

                Instrument instrument1;

                //SoundSource source;
                //all_sources.push_back(&source);
                int instrument_sound;
                //this_thread::sleep_for(chrono::milliseconds(1000));
                if (singleTrack)
                    instrument_sound = AudioLib->LoadMIDI(ac::getPath("Resources/Audio/MIDI/" + midiFileName).string().c_str(), ac::getPath("Resources/Audio/SoundFonts/" + sfFileName).string().c_str(), 1, midiTrack, sfPreset);
                else
                    instrument_sound = AudioLib->LoadMIDI(ac::getPath("Resources/Audio/MIDI/" + midiFileName).string().c_str(), ac::getPath("Resources/Audio/SoundFonts/" + sfFileName).string().c_str());

                //instrument1.addAudioSource(source);
                instrument1.addSound(instrument_sound);
                //instrument1.SetLooping(true);

                // move instrument 1
                instrument1.Translate(instrumentX, instrumentY, instrumentZ);
                instrument1.Update();
                all_instruments.push_back(instrument1);


                std::cout << "Piano position: X=" << instrumentX << ", Y=" << instrumentY << ", Z=" << instrumentZ << std::endl;
                std::cout << "Midi file name: " << midiFileName << std::endl;
                if(singleTrack)
                    std::cout << "Midi track: " << midiTrack << std::endl;
                std::cout << "SoundFont file name: " << sfFileName << std::endl;
                if(singleTrack)
                    std::cout << "SoundFont preset: " << sfPreset << std::endl;
                std::cout << std::endl;
            }
        }



    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

void setShaderToInstruments(Shader& shader) {
    for (Speaker speaker : all_speakers) {
        speaker.setShader(shader);
    }
}

void setModelsToInstruments(Model& model) {
    for (Speaker instrument : all_speakers) {
        instrument.setModel(model);
    }
}

void setSoundSourcesToInstruments(vector<Object*> instruments) {
    for (Object* instrument : instruments) {
        SoundSource source;
        //sources.push_back(&source);
        instrument->addAudioSource(source);

        //speaker1.addSound(speaker_sound);
        instrument->SetLooping(true);
    }
}

// end instrument process
GLuint RenderToTexture()
{
    GLuint fbo, texture;

    // Create framebuffer object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create texture object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Check for framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer incomplete: %x\n", status);
        exit(1);
    }

    // Render your scene to the FBO
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // ...render your scene here...
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Return the texture ID
    return texture;
}

void renderImGui() {
    // ImGUI related

    ImGui::Begin("Main Window");

    // Render your OpenGL output to a texture
    GLuint textureID = RenderToTexture();

    // Display the texture inside an ImGui window
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::Image((void*)(intptr_t)textureID, windowSize);

    // Add other viewports
    //if (ImGui::Begin("Side Viewport", nullptr, ImGuiWindowFlags_NoScrollbar)) {
        // Render your other viewport here
    //}
    //ImGui::End();

    // Add a main menu
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // Add menu items for File
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            // Add menu items for Edit
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            // Add menu items for View
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::End();

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        //speaker2.Play();

    //Camera position
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        string Sname = std::to_string(seatRow) + std::to_string(seatColumn);
        Seat seat = Seat(Sname, camera.position_);
        seat.AddSeatCoordinate();
        seatColumn++;
        if (seatRow == 'A' && seatColumn == 17) {
            seatRow = 'B';
            seatColumn = 1;
        }
        if (seatRow == 'B' && seatColumn == 17) {
            seatRow = 'C';
            seatColumn = 1;
        }
        if (seatRow == 'C' && seatColumn == 17) {
            seatRow = 'D';
            seatColumn = 1;
        }
        if (seatRow == 'D' && seatColumn == 17) {
            seatRow = 'E';
            seatColumn = 1;
        }
        if (seatRow == 'E' && seatColumn == 17) {
            seatRow = 'F';
            seatColumn = 1;
        }
        if (seatRow == 'F' && seatColumn == 17) {
            seatRow = 'G';
            seatColumn = 1;
        }
    }

    //Camera controls
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, false);
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, false);
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, false);
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, false);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        camera.processKeyboard(CameraMovement::FORWARD, true);
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        camera.processKeyboard(CameraMovement::BACKWARD, true);
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        camera.processKeyboard(CameraMovement::LEFT, true);
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        camera.processKeyboard(CameraMovement::RIGHT, true);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset, false);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

int main()
{
    // Initialize GLFW and create a window
    Window window(SCR_WIDTH, SCR_HEIGHT, "3D Audio Simulation");

    // init shaders
    Shader shader(ac::getPath("Resources/Shaders/default.vs").string().c_str(), ac::getPath("Resources/Shaders/default.fs").string().c_str());

    // init auditorium
    Model auditorium(ac::getPath("Resources/Models/Auditorium/scene.gltf").string());
    //init Sound Device
    SoundDevice* sd = LISTENER->Get();
    ALint attunation = AL_INVERSE_DISTANCE_CLAMPED;
    sd->SetAttunation(attunation);
    sd->SetLocation(camera.position_[0], camera.position_[1], camera.position_[2]);
    sd->SetOrientation(camera.front_[0], camera.front_[1], camera.front_[2], camera.up_[0], camera.up_[1], camera.up_[2]);

    // json config file
    json distribution = readJsonFromFile(ac::getPath("Resources/Config/distribution2.json").string());

    // init sounds
    int speaker_sound = AudioLib->Load(ac::getPath("Resources/Audio/Wav/Sound1_R.wav").string().c_str());
    // init audio sources
    //SoundSource speaker1_source;
   
    //Speakers;
    Model speaker_model(ac::getPath("Resources/Models/Speaker/scene.gltf").string());
    shared_ptr<Model> shared_speaker_model = make_shared<Model>(speaker_model);
    processSpeakers(all_speakers, distribution);
    for (int i = 0; i < all_speakers.size(); i++) {
        all_speakers[i].setModel(speaker_model);
    }
    for (int i = 0; i < all_speakers.size(); i++) {
        all_speakers[i].setShader(shader);
    }

    
    vector<SoundSource> sources_speakers;
    for (int i = 0; i < all_speakers.size(); i++) {
        SoundSource* source_speaker = new SoundSource;
        
        all_speakers[i].addAudioSource(*source_speaker);
    }

    //Instruments;
    Model instrument_model(ac::getPath("Resources/Models/Piano/scene.gltf").string());
    shared_ptr<Model> shared_instrument_model = make_shared<Model>(instrument_model);
    processInstruments(all_instruments, distribution);
    for (int i = 0; i < all_instruments.size(); i++) {
        all_instruments[i].setModel(instrument_model);
    }
    for (int i = 0; i < all_instruments.size(); i++) {
        all_instruments[i].setShader(shader);
    }

    vector<SoundSource> sources_instrum;
    for (int i = 0; i < all_instruments.size(); i++) {
        SoundSource* source_instrum = new SoundSource;

        all_instruments[i].addAudioSource(*source_instrum);
    }
    
    glfwSetKeyCallback(window.getGLFWWindow(), key_callback);
    glfwSetCursorPosCallback(window.getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);
    glfwSetFramebufferSizeCallback(window.getGLFWWindow(), framebuffer_size_callback);

    // Set up projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),
        (float)window.getWidth() / (float)window.getHeight(),
        0.1f, 100.0f);

    glm::mat4 auditorium_model = glm::mat4(1.0f);

    // Set up theater:
    //auditorium_model = glm::scale(modelMatrix, glm::vec3(0.9f, 0.9f, 0.001f));
    
    // Speaker 1 Play
    
    for (Speaker& speaker : all_speakers) {
        speaker.SetLooping(true);
       speaker.Play();
    }
    


    for (Instrument& instrument : all_instruments) {
        instrument.SetLooping(true);
        instrument.Play();
    }


    // Set up rendering
    glEnable(GL_DEPTH_TEST);

    // Rendering loop
    while (!window.shouldClose()) {
        // Update camera
        float deltaTime = window.getDeltaTime();
        camera.processMovement(deltaTime);

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up view matrix
        glm::mat4 view = camera.getViewMatrix();


        // Set up shader
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Render the auditorium
        shader.setMat4("model", auditorium_model);
        auditorium.Draw(shader);
        // Update Sound device
        sd->SetLocation(camera.position_[0], camera.position_[1], camera.position_[2]);
        sd->SetOrientation(camera.front_[0], camera.front_[1], camera.front_[2], camera.up_[0], camera.up_[1], camera.up_[2]);

        for (Speaker& speaker : all_speakers) {
            speaker.Update();
            speaker.Draw();
        }

        for (Instrument& instrument : all_instruments) {
            instrument.Update();
            instrument.Draw();
        }
        
        window.swapBuffers();
        window.pollEvents();
    }

    // Clean up
    glfwTerminate();
    return 0;
}
