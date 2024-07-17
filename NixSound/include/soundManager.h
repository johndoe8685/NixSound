#pragma once
#include <vector>
#include <unordered_map>
#include <vendor/AL/al.h>
#include <vendor/AL/alc.h>
#include <vendor/AL/alext.h>
#include <vendor/sndfile/sndfile.hh>
#include <vendor/NixTools/System.h>
#include "soundSource.h"

#define BUFFER_FRAME_SIZE 8192
#define BUFFER_AMOUNT 4


class SoundManager : public NixTools::System
{
private:
	struct SoundData
	{
		//Sound file pointer and Sound Data
		SndfileHandle snd_file;

		//OpenAL metadata
		ALint format;
		ALsizei number_of_byte;

		//Buffer
		short* memory_buffer;

		//Related to Sound Data
		ALuint current_source;

		//Buffers
		ALuint ALBuffers[BUFFER_AMOUNT];

		//Metadata
		std::string filePath;
		std::string soundName;

		//Booleans for the sound
		bool isPlaying;
	};

	ALCdevice* m_ALDevice;
	ALCcontext* m_ALContext;

	/*
	std::vector<SoundData> m_soundData;
	std::unordered_map<std::string, size_t> m_soundDataIndexMap;
	std::unordered_map<std::string, SoundData> m_soundData;
	*/

	std::vector<SoundData> m_soundData;
	std::unordered_map<std::string, size_t> m_soundDataIndexMap;
public:
	SoundManager(std::string name);
	~SoundManager();

	void addSound(std::string soundName, std::string filepath);
	void listAllAddedSounds();
	void playSound(SoundSource* source, std::string soundName);
	void stopSound(SoundSource* source);

	void updateBufferStream();
	void devFunc(std::string soundName);
};