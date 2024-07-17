#include <iostream>
#include <conio.h>
#include <vendor/NixTools/debugger.h>
#include "soundManager.h"


int main()
{
	SoundManager snd_manager("Default");
	
	NixTools::Debugger dbg("main", "test");
	snd_manager.addSound("wopwop", "/res/Webinar - 12 Zetta.ogg");
	SoundSource snd_source("test");
	snd_manager.playSound(&snd_source, "wopwop");
	snd_source.setVolume(1.0f);
	ALint state = AL_PLAYING;
	while (state == AL_PLAYING)
	{
		alGetSourcei(snd_source.getSource(), AL_SOURCE_STATE, &state);
		dbg.giveMessage(NixTools::Debugger::Info, "Currently playing sound. Press any key to stop.");
		if (_kbhit())
		{
				break;
		}
		snd_manager.updateBufferStream();
	}
	snd_manager.listAllAddedSounds();
}