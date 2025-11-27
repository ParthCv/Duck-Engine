#include "AudioManager.h"

#define DR_WAV_IMPLEMENTATION
#include "../../dependencies/OpenAL/libs/Win64/dr_wav.h"

void AudioManager::Init() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device\n";
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!alcMakeContextCurrent(context)) {
        std::cerr << "Failed to make audio context current\n";
        return;
    }

    // Apply the stored master volume immediately upon init, recover settings applied
    alListenerf(AL_GAIN, masterVolume);

    // Generate a pool of 16 sources for SFX
    for (int i = 0; i < 16; i++) {
        ALuint source;
        alGenSources(1, &source);
        sfxSources.push_back(source);
    }

    // Generate 1 source for music
    alGenSources(1, &musicSource);
    alSourcei(musicSource, AL_LOOPING, AL_TRUE); // Music loops
    alSourcef(musicSource, AL_GAIN, 0.5f);       // Music at 50% volume

    //Load music into buffer
    Get().LoadSound("shoot", "../assets/audio/shoot.wav");
    Get().LoadSound("quack", "../assets/audio/quack.wav");
    Get().LoadSound("music", "../assets/audio/music.wav");
    Get().LoadSound("chirpingbirds", "../assets/audio/chirpingbirds.wav");

    // play menu music right away
    Get().PlayMusic("music");
}

void AudioManager::LoadSound(const std::string& name, const std::string& filepath) {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;

    short* pSampleData = drwav_open_file_and_read_pcm_frames_s16(
        filepath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);

    if (!pSampleData) {
        std::cerr << "[Audio] Failed to load file: " << filepath << "\n";
        return;
    }

    ALuint buffer;
    alGenBuffers(1, &buffer);

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    alBufferData(buffer, format, pSampleData,
                 static_cast<ALsizei>(totalPCMFrameCount * channels * sizeof(short)),
                 sampleRate);

    // Check for OpenAL errors
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        std::cerr << "[Audio] OpenAL error after alBufferData: " << err << "\n";
        drwav_free(pSampleData, nullptr);
        alDeleteBuffers(1, &buffer);
        return;
    }

    buffers[name] = buffer;
    loadedPCMData[name] = pSampleData;
}

void AudioManager::PlaySound(const std::string& name, float volume) {
    if (buffers.find(name) == buffers.end()) return;

    for (ALuint source : sfxSources) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING) {
            alSourceStop(source);

            alSourcei(source, AL_BUFFER, 0);

            alSourcei(source, AL_LOOPING, AL_FALSE);
            alSourcef(source, AL_PITCH, 1.0f);
            alSourcef(source, AL_GAIN, volume);
            alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);

            alSourcei(source, AL_BUFFER, buffers[name]);

            alSourcePlay(source);
            return;
        }
    }
    std::cout << "All audio sources are busy! Sound dropped: " << name << "\n";
}

void AudioManager::PlayMusic(const std::string& name) {
    if (buffers.find(name) == buffers.end()) return;

    alSourceStop(musicSource);
    alSourcei(musicSource, AL_BUFFER, buffers[name]);
    alSourcePlay(musicSource);
}

void AudioManager::StopMusic() {
    alSourceStop(musicSource);
}

void AudioManager::SetMasterVolume(float volume) {
    // Clamp volume between 0 and 1
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    // Storing the volume for later retrieval
    masterVolume = volume;

    // AL_GAIN on the listener acts as a master volume scaler
    alListenerf(AL_GAIN, volume);
}

void AudioManager::CleanUp() {
    for (auto& entry : buffers)
        alDeleteBuffers(1, &entry.second);
    buffers.clear();

    for (auto& pcm : loadedPCMData)
        drwav_free(pcm.second, nullptr);
    loadedPCMData.clear();

    for (ALuint source : sfxSources)
        alDeleteSources(1, &source);
    alDeleteSources(1, &musicSource);

    sfxSources.clear();

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}