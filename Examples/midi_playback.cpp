#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#define TSF_IMPLEMENTATION
#include "../third_party/tsf/tsf.h"
#define TML_IMPLEMENTATION
#include "../third_party/tsf/tml.h"

#include "root_directory.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>

namespace ac = Acousent;
using namespace std;

// El soundfont que se utilizara. Esto tiene que ser global para que se pueda ir cambiando entre distintos presets (instrumentos) 
// dentro del mismo archivo .sf2 
static tsf* g_TinySoundFont;

// Lo mismo para los MIDI ya que estos pueden contener varios tracks
static double g_Msec;               // Tiempo actual de playback
static tml_message* g_MidiMessage;  // Proximo mensaje MIDI a leer

// Un entero estilo contador para ver cuantas iteraciones de los loops se estan realizando asi compararlo con la cantidad necesaria.
int test_int = 0;

// Esta es la funcion principal para la lectura de archivos MIDI. Lee los archivos y crea un buffer que lo guarda en el puntero audioStream.
void ProcessMIDIAndRender(float* audioStream, int streamLength)
{
    // El archivo MIDI se tiene que leer por bloques.
    int sampleBlock, sampleCount = streamLength / (1 * sizeof(float)); // La multiplicacion por 1 es por la cantidad de canales (MONO en este caso)

    // Iteracion por bloque
    for (sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; sampleCount > 0; sampleCount -= sampleBlock)
    {
        // Esto para que cuando el bloque sea mayor a los samples restantes no solo utilice un bloque del tamano de los restantes.
        if (sampleBlock > sampleCount)
            sampleBlock = sampleCount;

        // Se procesa el MIDI con los parametros necesarios y para cada senal que entrega el MIDI el soundfont debe realizar una accion.
        for (g_Msec += sampleBlock * (1000.0 / 44100.0); g_MidiMessage && g_Msec >= g_MidiMessage->time; g_MidiMessage = g_MidiMessage->next)
        {
            switch (g_MidiMessage->type)
            {
            case TML_PROGRAM_CHANGE:
                // Si el MIDI tiene algun numero de preset especifico para elegir instrumento se detecta aca y se cambia el preset del soundfont
                tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
                break;
            case TML_NOTE_ON:
                // Nota ON
                tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
                break;
            case TML_NOTE_OFF:
                // Nota OFF
                tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
                break;
            case TML_PITCH_BEND:
                // Si hay cambios de tonos se deben ajustar en el soundfont para tener la nota correcta para esa nota en el MIDI.
                tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
                break;
            case TML_CONTROL_CHANGE:
                // La libreria tiny sound font recibe los mensajes de controllador MIDI si esque hay cambios.
                tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
                break;
            }
        }

        // La siguiente linea de codigo crea el audio a partir de valores float y los guarda en el puntero audioStream.
        // Esto lo realiza.
        tsf_render_float(g_TinySoundFont, audioStream, sampleBlock, 0);
        // Contador para efectos de Debug
        test_int++;
        if (test_int > 2267) {
            //cout << "Test_int es maypr a 2267" << endl;
            //auto float_size = sizeof(float);
            //cout << "Float size: " << float_size << endl;
        }
        // Se deben calcular cuantos bytes se procesaron para asi avanzar el puntero al siguiente lugar en memoria donde se continuara la cancion.
        int blockSizeBytes = sampleBlock * (1 * sizeof(float));

        // Incrementar el puntero por la cantidad de samples procesados.
        audioStream += blockSizeBytes / sizeof(unsigned int);

    }
}

// Esta funcion utiliza el archivo midi y el sample rate para poder dividir el archivo en bloques.
int CalculateStreamLength(double sampleRate)
{
    int maxTimestamp = 0;

    // Itera los mensajes MIDI y encuentra el mayor largo de los tracks. (Esto sera el largo completo ya que los tracks siempre parten del principio).
    for (tml_message* message = g_MidiMessage; message != NULL; message = message->next)
    {
        if (message->time > maxTimestamp)
        {
            maxTimestamp = message->time;
        }
    }

    // Se calcula streamLength (largo del puntero para el buffer) utilizando el sample rate y el mayor tiempo de los tracks 
    // (Dividio por 1000 ya que se requieren segundos)
    int streamLength = (int)(maxTimestamp * sampleRate / 1000.0) * (1 * sizeof(float)); // 1 Channel

    return streamLength;
}



int main(int argc, char* argv[])
{

    // Sample rate es la cantidad de samples que se requieren. Mientras mas samples mejor la calidad del audio pero mas memoria se utiliza.
    // En general se utiliza 44100 samples o 44.1kHz de Sample Rate ya que este es el sample rate de los CDs clasicos.
    // En caso de que se requiera una mejor calidad de audio o peor, si se pierden notas se debe incrementar el sample Rate.
    const auto sampleRate = 44100.0;
    // Inicio la libreria de TinyMidiLoader.
    tml_message* TinyMidiLoader = nullptr;

    // Inicializacion de OpenAL
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device)
    {
        fprintf(stderr, "Could not open OpenAL device\n");
        return 1;
    }
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context)
    {
        fprintf(stderr, "Could not create OpenAL context\n");
        return 1;
    }
    // Le entregamos al contexto de OpenAL el sampleRate que utilizaremos.
    ALCint attributes[] = { ALC_FREQUENCY, sampleRate, 0 };
    alcMakeContextCurrent(context);

    // Cargar el soundfont desde el archivo.
    g_TinySoundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str());
    if (!g_TinySoundFont)
    {
        fprintf(stderr, "Could not load SoundFont\n");
        return 1;
    }

    // Inicializa el preset en el canal especial numero 10 del MIDI para utilizar el banco de sonidos de percucion del soundfont (128) si lo tiene
    tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

    // Le indicamos a tsf los parametros de rendering del audio.
    tsf_set_output(g_TinySoundFont, TSF_MONO, sampleRate, 0.0f); // Hay que 

    // Cargamos el archivo MIDI
    TinyMidiLoader = tml_load_filename(ac::getPath("Resources/Audio/MIDI/venture.mid").string().c_str());
    if (!TinyMidiLoader)
    {
        fprintf(stderr, "Could not load MIDI file\n");
        return 1;
    }

    // Inicializamos el midi loader global en el primer mensaje de nuestro archivo cargado.
    // Esto para que la funcion pueda utilizar el midi correcto.
    g_MidiMessage = TinyMidiLoader;

    // Calculamos el largo necesario del buffer de audio.
    auto streamLength = CalculateStreamLength(sampleRate);
    // Alocamos la memoria necesaria para el buffer completo en forma de arreglo.
    float* audioStream = new float[streamLength / sizeof(float)];

    // Procesamos el midi con el soundfont elegido y lo guardamos en audioStream
    ProcessMIDIAndRender(audioStream, streamLength);

    // Revisar los cast para hacerlo solo con Float.

    // Logs para efectos de Debug
    cout << "Iteraciones: " << test_int << endl;
    auto tamano = streamLength / sizeof(ALuint);
    cout << "Tamano audioStream: " << tamano << endl;
    cout << "streamLength: " << streamLength << endl;

    // Creamos el buffer de OpenAL utilizando el buffer audioStream para la informacion del audio.
    ALuint bufferID;
    alGenBuffers(1, &bufferID);
    alBufferData(bufferID, AL_FORMAT_MONO_FLOAT32, audioStream, static_cast<ALsizei>(streamLength), sampleRate);

    // Creamos una fuente de sonido de OpenAL para que haga sonar el Buffer.
    ALuint sourceID;
    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, bufferID);
    //alSourcei(sourceID, AL_LOOPING, AL_TRUE);

    // Hacemos sonar la fuente de sonido que tiene el buffer deseado.
    alSourcePlay(sourceID);

    // Revisamos si la fuente de audio esta sonando
    ALint sourceState;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &sourceState);
    // Mientras este sonando no debemos cerrar el probrama.
    while (true) {
        alGetSourcei(sourceID, AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING) {
            // Playback completo
            break;
        }
        // Esto hace busy waiting pero y se puede optimizar
    }

    // Terminado el playback

    // Cleanup de fuente de audio y de buffer.
    
    alDeleteSources(1, &sourceID);
    alDeleteBuffers(1, &bufferID);

    // Cleanup de los otros recursos utilizados
    delete[] audioStream;
    tsf_close(g_TinySoundFont);
    tml_free(TinyMidiLoader);

    // Cleanup de OpenAL
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
