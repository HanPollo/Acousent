#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"
#include "filesystem.h"
#include "utils.h"
#include "../root_directory.h"
#include "../Audio/SoundDevice.h"
#include "../Audio/SoundLibrary.h"
#include "../Audio/SoundSource.h"
#include "../Objects/Object.h"
#include "../Objects/Speaker.h"
#include "../Rendering/Camera.h"
#include "../Rendering/Model.h"
#include "../Rendering/Shader.h"

#include <nlohmann/json.hpp>


using json = nlohmann::json;

namespace Acousent {

	class Application {

	public:
		Application();
		//Posible destructor?

		// Variables
		float deltaTime = .01667f;
		bool firstMouse = true;
		float lastX = SCR_WIDTH / 2.0f;
		float lastY = SCR_HEIGHT / 2.0f;
		char seatRow = 'A';
		int seatColumn = 1;
		const unsigned int SCR_WIDTH = 1000;
		const unsigned int SCR_HEIGHT = 1000;

		// Callbacks
			//	Callbacks
		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		/*			Init Functions			*/
		void init();

		json readJsonFromFile(const std::string& filename);
		void getAllJSONKeys(const json& jsonObject, std::vector<std::string>& keys);
		void processDistributionJson();
		void useShader();
		void playSource(int object_index);
		
		void createAppMembers(); //Podrian ser creados en constructor
		/*			End Init Functions			*/

		/*			Getters			*/
		Window getWindow();
		Shader getShader();
		vector<Object> getAppObjects();
		vector<SoundSource> getSources();
		vector<int> getSounds();
		SoundDevice* getSoundDevice();
		SoundLibrary* getAudioLibrary();
		ALint getAttunation();
		vector<Model> getModels();
		json getConfig();
		Camera getCamera();
		/*			End Getters			*/

		/*			Setters			*/
		/*			End Setters			*/
		
		/*			Loop functions			*/
		void Update();
		void ClearScreen();
		/*			End Loop functions			*/

		//Destructor
		void terminate();

		



	private:
		


		Window m_window;
		Shader m_default_shader;
		vector<Object> m_app_objects;
		vector<SoundSource> m_sound_sources;
		vector<int> m_sounds;
		SoundDevice* p_sound_device = LISTENER->Get();
		SoundLibrary* p_audio_library = SoundLibrary::Get();
		ALint m_attunation;
		vector<Model> m_models;
		json m_config;
		Camera m_camera;

	};





}

#endif // APPLICATION_H
