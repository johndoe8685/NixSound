#include "soundSource.h"


SoundSource::SoundSource(std::string sourceName)
	:System("SoundSource", sourceName), m_State(AL_INITIAL)
{
	alGenSources(1, &m_Source);
	alSourcef(m_Source, AL_PITCH, m_Pitch);
	alSourcef(m_Source, AL_GAIN, m_Gain);
	alSource3f(m_Source, AL_POSITION, m_Position[0], m_Position[1], m_Position[2]);
	alSource3f(m_Source, AL_VELOCITY, m_Velocity[0], m_Velocity[1], m_Velocity[2]);
	alSourcei(m_Source, AL_LOOPING, m_LoopSound);
	alSourcei(m_Source, AL_BUFFER, m_Buffer);
}

SoundSource::~SoundSource()
{
	alSourcei(m_Source, AL_BUFFER, NULL);
	alDeleteSources(1, &m_Source);
}

void SoundSource::setVolume(float volume)
{
	m_Gain = volume;
	alSourcef(m_Source, AL_GAIN, m_Gain);
}

void SoundSource::play(ALuint* buffers, ALsizei numberOfBuffers)
{
	// clear any al errors
	alGetError();

	//Rewind the source position and clear the buffer queue
	alSourceRewind(m_Source);
	alSourcei(m_Source, AL_BUFFER, 0);

	//Queue and start play
	alSourceQueueBuffers(m_Source, numberOfBuffers, buffers);
	alSourcePlay(m_Source);
}