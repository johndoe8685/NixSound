#pragma once
#include <vendor/NixTools/system.h>
#include <vendor/AL/al.h>

class SoundManager;

class SoundSource : public NixTools::System
{
private:
	ALuint m_Source;
	float m_Pitch = 1.f;
	float m_Gain = 0.5f;
	float m_Position[3] = { 0,0,0 };
	float m_Velocity[3] = { 0,0,0 };
	bool m_LoopSound = false;
	ALuint m_Buffer = 0;
	ALint m_State;
public:
	SoundSource(std::string sourceName);
	~SoundSource();

	ALuint getSource() { return m_Source; }

	void setVolume(float volume);
private:
	void play(ALuint* buffers, ALsizei numberOfBuffers);

	friend SoundManager;
};