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
#include <sys/stat.h>

#include "Rendering/Shader.h"
#include "Rendering/ModelManager.h"
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
glm::mat4 projection;

// Set up camera
Camera camera(glm::vec3(-0.5f, 3.0f, 9.0f));
float deltaTime = .01667f;
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
char seatRow = 'A';
int seatColumn = 1;
bool cameraLocked = false;


vector<Speaker> all_speakers;
vector<Instrument> all_instruments;
vector<SoundSource*> all_sources;
vector<Model*> all_models;
shared_ptr<Model> shared_synth_model;
SoundLibrary* AudioLib = SoundLibrary::Get();
bool looping = false;
bool noSourcesPlaying = true;
int sourceNum = 0;

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

void createModel(const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string path = ac::getPath("Resources/Models/Instruments/" + name + "/instruments.obj").string();
    const char* cpath = path.c_str();
    struct stat sb;

    // Use smart pointers for creating models
    std::shared_ptr<Model> model;

    if (stat(cpath, &sb) == 0 && !ModelManager::getInstance().modelExists(name)) {
        // File exists, create model
        model = std::make_shared<Model>(path);
        // Add the model to the manager
        ModelManager::getInstance().addModel(name, model);
    }
    else if (!ModelManager::getInstance().modelExists("Synth")) {
        // File doesn't exist, handle it, maybe create a different model
        model = std::make_shared<Model>(ac::getPath("Resources/Models/Instruments/Synth/instruments.obj").string());
        // Add the model to the manager
        ModelManager::getInstance().addModel("Synth", model);
    }

    // Add the model to the manager
    //ModelManager::getInstance().addModel(name, model);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Create Model took " << diff.count() << " seconds\n";
    
}

// Process speakers from json
void processSpeakers(vector<Speaker> speakers, json distribution) {
    try {
        // Access the speaker objects in the array
        if (distribution.contains("audio_instruments") && distribution["audio_instruments"].is_array()) {
            for (const auto& speaker : distribution["audio_instruments"]) {
                // Access the properties of each piano object
                float speakerX = speaker["position"]["x"];
                float speakerY = speaker["position"]["y"];
                float speakerZ = speaker["position"]["z"];
                std::string audioFileName = speaker["audio_file"];
                std::string name = speaker["name"];


                // Process the piano properties...
                
                //Model speaker_model(ac::getPath("Resources/Models/Speaker/scene.gltf").string());
                //all_models.push_back(&speaker_model);

                Speaker speaker1; // revisar in placeback

                //SoundSource source;
                //all_sources.push_back(&source);
                int speaker_sound = AudioLib->Load(ac::getPath("Resources/Audio/Wav/"+audioFileName).string().c_str());
                if (ModelManager::getInstance().modelExists(name)) {
                    speaker1.setModel(*ModelManager::getInstance().getModel(name));
                    std::cout << "Model " << name << " existe" << std::endl;
                }
                else {
                    std::cout << "Model " << name << " no existe" << std::endl;
                    createModel(name);
                    speaker1.setModel(*ModelManager::getInstance().getModel(name));
                }
                

                //speaker1.addAudioSource(source);
                speaker1.addSound(speaker_sound);
                //speaker1.SetLooping(true);
                
                // move speaker 1
                speaker1.Translate(speakerX, speakerY+2, speakerZ-5);
                speaker1.Update();
                all_speakers.push_back(speaker1);
                
               
                std::cout << "Speaker position: X=" << speakerX << ", Y=" << speakerY << ", Z=" << speakerZ << std::endl;
                glm::vec3 pos = speaker1.getPosition();
                std::cout << "Speaker real position: X=" << pos[0] << ", Y=" << pos[1] << ", Z=" << pos[2] << std::endl;
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
        if (distribution.contains("midi_instruments") && distribution["midi_instruments"].is_array()) {
            for (const auto& instrument : distribution["midi_instruments"]) {
                // Access the properties of each piano object
                float instrumentX = instrument["position"]["x"];
                float instrumentY = instrument["position"]["y"];
                float instrumentZ = instrument["position"]["z"];
                std::string midiFileName = instrument["midi_file"];
                std::string sfFileName = instrument["soundfont_file"];
                std::string name = instrument["name"];
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
                if (ModelManager::getInstance().modelExists(name)) {
                    instrument1.setModel(*ModelManager::getInstance().getModel(name));
                    std::cout << "Model " << name << " existe" << std::endl;
                }
                else {
                    std::cout << "Model " << name << " no existe" << std::endl;
                    createModel(name);
                    instrument1.setModel(*ModelManager::getInstance().getModel(name));
                }

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

                // move instrument 1
                instrument1.Translate(instrumentX, instrumentY+2, instrumentZ-5);
                instrument1.Update();
                all_instruments.push_back(instrument1);
                


                std::cout << "Piano position: X=" << instrumentX << ", Y=" << instrumentY << ", Z=" << instrumentZ << std::endl;
                glm::vec3 pos = instrument1.getPosition();
                std::cout << "Instrument real position: X=" << pos[0] << ", Y=" << pos[1] << ", Z=" << pos[2] << std::endl;
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
        instrument->SetLooping();
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        //speaker2.Play();

    //Play sounds
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        for (Speaker& speaker : all_speakers) {
            if (noSourcesPlaying || !speaker.isStopped()) {
                speaker.Play();
            }
            
        }

        for (Instrument& instrument : all_instruments) {
            if (noSourcesPlaying || !instrument.isStopped()) {
                instrument.Play();
            }
        }
        noSourcesPlaying = false;
    }
    // Loop sounds
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        /*
        for (Speaker& speaker : all_speakers) {
            speaker.SetLooping();
        }

        for (Instrument& instrument : all_instruments) {
            instrument.SetLooping();
        }
        */
        looping = !looping;
    }

    //Camera controls
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::FORWARD, true);
        camera.processKeyboard(CameraMovement::BACKWARD, true);
        camera.processKeyboard(CameraMovement::LEFT, true);
        camera.processKeyboard(CameraMovement::RIGHT, true);
        cameraLocked = !cameraLocked;
    }
        
    if (!cameraLocked) {
        if (key == GLFW_KEY_W && action == GLFW_PRESS)
            camera.processKeyboard(CameraMovement::FORWARD, false);
        if (key == GLFW_KEY_S && action == GLFW_PRESS)
            camera.processKeyboard(CameraMovement::BACKWARD, false);
        if (key == GLFW_KEY_A && action == GLFW_PRESS)
            camera.processKeyboard(CameraMovement::LEFT, false);
        if (key == GLFW_KEY_D && action == GLFW_PRESS)
            camera.processKeyboard(CameraMovement::RIGHT, false);
        if (key == GLFW_KEY_W && action == GLFW_RELEASE)
            camera.processKeyboard(CameraMovement::FORWARD, true);
        if (key == GLFW_KEY_S && action == GLFW_RELEASE)
            camera.processKeyboard(CameraMovement::BACKWARD, true);
        if (key == GLFW_KEY_A && action == GLFW_RELEASE)
            camera.processKeyboard(CameraMovement::LEFT, true);
        if (key == GLFW_KEY_D && action == GLFW_RELEASE)
            camera.processKeyboard(CameraMovement::RIGHT, true);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!cameraLocked) {
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
    else
        firstMouse = true;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!cameraLocked) {
        camera.processMouseScroll(static_cast<float>(yoffset));
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::radians(camera.getZoom()),
        (float)width / (float)height, 0.1f, 100.0f);
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
    //Model speaker_model(ac::getPath("Resources/Models/Speaker/scene.gltf").string());
    //shared_ptr<Model> shared_speaker_model = make_shared<Model>(speaker_model);
    processSpeakers(all_speakers, distribution);
    /*
    for (int i = 0; i < all_speakers.size(); i++) {
        all_speakers[i].setModel(speaker_model);
    }
    */
    for (int i = 0; i < all_speakers.size(); i++) {
        all_speakers[i].setShader(shader);
    }

    
    vector<SoundSource> sources_speakers;
    for (int i = 0; i < all_speakers.size(); i++) {
        SoundSource* source_speaker = new SoundSource;
        
        all_speakers[i].addAudioSource(*source_speaker);
    }

    //Instruments;
    //Model instrument_model(ac::getPath("Resources/Models/Piano3/instrument.obj").string());
    //shared_ptr<Model> shared_instrument_model = make_shared<Model>(instrument_model);
    processInstruments(all_instruments, distribution);
    /*
    for (int i = 0; i < all_instruments.size(); i++) {
        all_instruments[i].setModel(instrument_model);
    }
    */
    for (int i = 0; i < all_instruments.size(); i++) {
        all_instruments[i].setShader(shader);
    }
    vector<SoundSource> sources_instrum;
    for (int i = 0; i < all_instruments.size(); i++) {
        SoundSource* source_instrum = new SoundSource;

        all_instruments[i].addAudioSource(*source_instrum);
    }
    sourceNum = all_instruments.size() + all_sources.size();

    glfwSetKeyCallback(window.getGLFWWindow(), key_callback);
    glfwSetCursorPosCallback(window.getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);
    glfwSetFramebufferSizeCallback(window.getGLFWWindow(), framebuffer_size_callback);

    // Set up projection matrix
    projection = glm::perspective(glm::radians(camera.getZoom()),
        (float)window.getWidth() / (float)window.getHeight(),
        0.1f, 100.0f);

    glm::mat4 auditorium_model = glm::mat4(1.f);
    //auditorium_model = glm::scale(auditorium_model, glm::vec3(16.0f, 16.0f, 16.0f));
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
        noSourcesPlaying = true;
        for (Speaker& speaker : all_speakers) {
            speaker.Update();
            speaker.Draw();
            noSourcesPlaying = (noSourcesPlaying && speaker.isStopped());
        }

        for (Instrument& instrument : all_instruments) {
            instrument.Update();
            instrument.Draw();
            noSourcesPlaying = (noSourcesPlaying && instrument.isStopped());
        }
        for (Speaker& speaker : all_speakers) {
            if (looping && noSourcesPlaying) {
                speaker.Play();
            }
        }
        for (Instrument& instrument : all_instruments) {
            if (looping && noSourcesPlaying) {
                instrument.Play();
            }
        }
        
        window.swapBuffers();
        window.pollEvents();
    }

    // Clean up
    glfwTerminate();
    return 0;
}
