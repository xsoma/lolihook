#include <thread>
#include "api.h"
#include "Sounds.h"
using namespace std::chrono_literals;

static std::pair<std::string, char> channels[] = {
	__(" "),
	__("https://streams.ilovemusic.de/iloveradio16.mp3?hadpreroll"), // Greatest Hits
	__("https://streams.ilovemusic.de/iloveradio2.mp3?hadpreroll"), // Dance Hits
	__("https://streams.ilovemusic.de/iloveradio6.mp3?hadpreroll"), // Best German rap
	__("http://streams.ilovemusic.de/iloveradio10.mp3?hadpreroll"), // Chill 
	__("https://streams.ilovemusic.de/iloveradio109.mp3?hadpreroll"), // Top 100
	__("https://streams.ilovemusic.de/iloveradio104.mp3?hadpreroll"), // Top 40 Rap
	__("https://streams.ilovemusic.de/iloveradio3.mp3?hadpreroll") // Hip Hop
};

void playback_loop()
{
	//auto& var = variable::get();

	static bool once = false;

	if (!once)
	{
		BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));

		if (BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL))
		{
			BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
			BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
			once = true;
		}
	}

	static auto bass_needs_reinit = false;

	const auto desired_channel = g_cfg.misc.radiochannel;
	static auto current_channel = 0;

	if (g_cfg.misc.radiochannel == 0)
	{
		current_channel = 0;
		BASS_Stop();
		BASS_STOP_STREAM();
		BASS_StreamFree(BASS::stream_handle);
	}
	else if (once && g_cfg.misc.radiochannel > 0)
	{

		if (current_channel != desired_channel || bass_needs_reinit)
		{
			bass_needs_reinit = false;
			BASS_Start();
			_rt(channel, channels[desired_channel]);
			BASS_OPEN_STREAM(channel);
			current_channel = desired_channel;
		}

		BASS_SET_VOLUME(BASS::stream_handle, radio_muted ? 0 : g_cfg.misc.radiovolume / 100);
		BASS_PLAY_STREAM();
	}
	else if (BASS::bass_init)
	{
		bass_needs_reinit = true;
		BASS_StreamFree(BASS::stream_handle);
	}
}