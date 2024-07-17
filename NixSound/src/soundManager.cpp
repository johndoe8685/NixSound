#include "soundManager.h"

SoundManager::SoundManager(std::string name)
	:System("SoundManager", name)
{
	m_ALDevice = alcOpenDevice(nullptr);
	if (m_ALDevice)
	{
		m_ALContext = alcCreateContext(m_ALDevice, nullptr);
		alcMakeContextCurrent(m_ALContext);
		debugger.giveMessage(NixTools::Debugger::Info, "Device Created!!");
	}
	else
	{
		debugger.giveMessage(NixTools::Debugger::Error, "Device couldnt created.");
	}
}

SoundManager::~SoundManager()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_ALContext);
	alcCloseDevice(m_ALDevice);
}

void SoundManager::addSound(std::string soundName, std::string filepath)
{
	SoundData data;

	bool isQualified = true;

	data.soundName = soundName;
	data.filePath = filepath;
	directory.changePath(filepath);

	SndfileHandle sndFile(directory.getPath().c_str(), SFM_READ);
	data.snd_file = sndFile;
	
	//Check if file is properly loaded
	int error = sf_error(sndFile.rawHandle());
	if (error != 0)
	{
		debugger.giveMessage(NixTools::Debugger::Error, "SndFile Error value", error);
		isQualified = false;
	}

	//Check the frames
	if (sndFile.frames() < 1)
	{
		debugger.giveMessage(NixTools::Debugger::Error, "Bad sample Count");
		isQualified = false;
	}

	//Check if file is vorbis encoded
	if ((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_VORBIS)
	{
		debugger.giveMessage(NixTools::Debugger::Error, "NixSound only works with vorbis encoding");
		isQualified = false;
	}

	//Check channel format
	switch (sndFile.channels())
	{
	case(1):
		data.format = AL_FORMAT_MONO16;
		break;
	case(2):
		data.format = AL_FORMAT_STEREO16;
		break;
	default:
		debugger.giveMessage(NixTools::Debugger::Error, "Sound channel is not supported");
		isQualified = false;
		break;
	}

	//Check if file can be accessed
	if (isQualified)
	{
		//Allocate the part of the audio file into a buffer
		data.memory_buffer = static_cast<short*>(malloc(BUFFER_FRAME_SIZE * data.snd_file.channels() * sizeof(short)));
		alGenBuffers(BUFFER_AMOUNT, data.ALBuffers);

		//Load the Buffers
		for (ALsizei i = 0; i < BUFFER_AMOUNT; i++)
		{
			sf_count_t size = data.snd_file.readf(data.memory_buffer, BUFFER_FRAME_SIZE);
			if (size < 1) break;
			size = size * data.snd_file.channels() * (sf_count_t)sizeof(short);

			alBufferData(data.ALBuffers[i], data.format, data.memory_buffer, size, data.snd_file.samplerate());
		}

		m_soundData.emplace_back(data);
	}
}

void SoundManager::listAllAddedSounds()
{
	
}

void SoundManager::playSound(SoundSource* source, std::string soundName)
{
	m_soundData[m_soundDataIndexMap[soundName]].current_source = source->getSource();
	SoundData data = m_soundData[m_soundDataIndexMap[soundName]];
	ALsizei i;
	for (i = 0; i < BUFFER_AMOUNT; i++)
	{
		sf_count_t size = data.snd_file.read(data.memory_buffer, BUFFER_FRAME_SIZE);
		if (size < 1) break;
		size = size * data.snd_file.channels() * (sf_count_t)sizeof(short);

		alBufferData(data.ALBuffers[i], data.format, data.memory_buffer, size, data.snd_file.samplerate());
	}

	source->play(data.ALBuffers, i);
}

void SoundManager::stopSound(SoundSource* source)
{
}

void SoundManager::updateBufferStream()
{
	for (size_t i = 0; i < m_soundData.size(); i++)
	{
		ALint processed, state;
		
		alGetSourcei(m_soundData[i].current_source, AL_SOURCE_STATE, &state);
		alGetSourcei(m_soundData[i].current_source, AL_BUFFERS_PROCESSED, &processed);
		ALenum err = alGetError();
		if (err != AL_NO_ERROR)
		{
			debugger.giveMessage(NixTools::Debugger::Error, "Error while checking sound source state", err);
		}

		//Unqueue and load each processed buffer
		while (processed > 0)
		{
			ALuint buffer_id;

			alSourceUnqueueBuffers(m_soundData[i].current_source, 1, &buffer_id);
			processed = processed - 1;

			//Read the next chunk of data, refill the buffer, and queue it back to sound source
			ALsizei size = m_soundData[i].snd_file.readf(m_soundData[i].memory_buffer, BUFFER_FRAME_SIZE);
			if (size > 0)
			{
				size = size * m_soundData[i].snd_file.channels() * (sf_count_t)sizeof(short);
				alBufferData(buffer_id, m_soundData[i].format, m_soundData[i].memory_buffer, size, m_soundData[i].snd_file.samplerate());
				alSourceQueueBuffers(m_soundData[i].current_source, 1, &buffer_id);
			}

			if (alGetError() != AL_NO_ERROR)
			{
				debugger.giveMessage(NixTools::Debugger::Error, "Error restarting music playback");
			}
		}

		/* Make sure the source hasn't underrun */
		if (state != AL_PLAYING && state != AL_PAUSED)
		{
			ALint queued;

			/* If no buffers are queued, playback is finished */
			alGetSourcei(m_soundData[i].current_source, AL_BUFFERS_QUEUED, &queued);
			if (queued == 0)
				return;

			alSourcePlay(m_soundData[i].current_source);
			if (alGetError() != AL_NO_ERROR)
			{
				debugger.giveMessage(NixTools::Debugger::Error, "Error restarting music playback");
			}
		}
	}
}

void SoundManager::devFunc(std::string soundName)
{
	SoundData data = m_soundData[m_soundDataIndexMap[soundName]];
	std::cout << "Size of the sound data: " << data.snd_file.frames() * data.snd_file.channels() * sizeof(short) << std::endl;
}
