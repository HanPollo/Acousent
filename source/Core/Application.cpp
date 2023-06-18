#include "Application.h"

Acousent::Application::Application()
{
}

void Acousent::Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        //speaker2.Play();

    //Camera position
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        string Sname = std::to_string(seatRow) + std::to_string(seatColumn);
        Seat seat = Seat(Sname, m_camera.position_);
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
        m_camera.processKeyboard(CameraMovement::FORWARD, false);
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        m_camera.processKeyboard(CameraMovement::BACKWARD, false);
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        m_camera.processKeyboard(CameraMovement::LEFT, false);
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        m_camera.processKeyboard(CameraMovement::RIGHT, false);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        m_camera.processKeyboard(CameraMovement::FORWARD, true);
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        m_camera.processKeyboard(CameraMovement::BACKWARD, true);
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        m_camera.processKeyboard(CameraMovement::LEFT, true);
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        m_camera.processKeyboard(CameraMovement::RIGHT, true);
}

void Acousent::Application::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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

    m_camera.processMouseMovement(xoffset, yoffset, false);
}

void Acousent::Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_camera.processMouseScroll(static_cast<float>(yoffset));
}

void Acousent::Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void Acousent::Application::init()
{
}

json Acousent::Application::readJsonFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    json jsonObject;
    file >> jsonObject;

    return jsonObject;
}

void Acousent::Application::getAllJSONKeys(const json& jsonObject, std::vector<std::string>& keys)
{
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        if (it->is_object()) {
            getAllJSONKeys(it.value(), keys);  // Recursively process nested objects
        }
        keys.push_back(it.key());
    }
}

void Acousent::Application::processDistributionJson()
{
    try {
        json distribution = readJsonFromFile(ac::getPath("Resources/Config/distribution.json").string()); //Se deberia recibir como parametro.
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
                int speaker_sound = p_audio_library->Load(ac::getPath("Resources/Audio/Wav/" + audioFileName).string().c_str());
                //speaker1.setModel(speaker_model);

                //speaker1.addAudioSource(source);
                speaker1.addSound(speaker_sound);
                //speaker1.SetLooping(true);

                // move speaker 1
                speaker1.Translate(speakerX, speakerY, speakerZ);
                speaker1.Update();
                m_app_objects.push_back(speaker1);


                std::cout << "Piano position: X=" << speakerX << ", Y=" << speakerY << ", Z=" << speakerZ << std::endl;
                std::cout << "Audio file name: " << audioFileName << std::endl;
                std::cout << std::endl;
            }
        }



    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

void Acousent::Application::useShader()
{
}

void Acousent::Application::playSource(int object_index)
{
}

void Acousent::Application::createAppMembers()
{
}

Window Acousent::Application::getWindow()
{
	return m_window;
}

Shader Acousent::Application::getShader()
{
	return m_default_shader;
}

vector<Object> Acousent::Application::getAppObjects()
{
	return m_app_objects;
}

vector<SoundSource> Acousent::Application::getSources()
{
	return m_sound_sources;
}

vector<int> Acousent::Application::getSounds()
{
	return m_sounds;
}

SoundDevice* Acousent::Application::getSoundDevice()
{
	return p_sound_device;
}

SoundLibrary* Acousent::Application::getAudioLibrary()
{
	return p_audio_library;
}

ALint Acousent::Application::getAttunation()
{
	return m_attunation;
}

vector<Model> Acousent::Application::getModels()
{
	return m_models;
}

json Acousent::Application::getConfig()
{
	return m_config;
}

Camera Acousent::Application::getCamera()
{
	return m_camera;
}

void Acousent::Application::Update()
{
}

void Acousent::Application::ClearScreen()
{
}

void Acousent::Application::terminate()
{
}
