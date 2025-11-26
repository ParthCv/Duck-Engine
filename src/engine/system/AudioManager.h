//
// Created by jeff_ on 2025-11-26.
//

#pragma once
#include <../dependencies/OpenAL/include/al.h>
#include <../dependencies/OpenAL/include/alc.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>

class AudioManager {
public:
    static AudioManager& Get() {
        static AudioManager instance;
        return instance;
    }

    void Init();
    void CleanUp();

    // Loads a wav file into memory and gives it a name
    void LoadSound(const std::string& name, const std::string& filepath);

    // Plays a sound effect
    void PlaySound(const std::string& name, float volume = 1.0f);

    // Plays music
    void PlayMusic(const std::string& name);

    // Stop music
    void StopMusic();

private:
    AudioManager() {}

    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

    std::map<std::string, ALuint> buffers;
    std::vector<ALuint> sfxSources;
    ALuint musicSource;

    std::map<std::string, short*> loadedPCMData;
};