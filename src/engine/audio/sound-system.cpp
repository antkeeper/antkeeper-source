// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio/sound-system.hpp>
#include <format>
#include <stdexcept>

namespace engine::audio
{
	sound_system::sound_system()
	{
		// Open audio device
		ALCdevice* alc_device = alcOpenDevice(nullptr);
		if (!alc_device)
		{
			throw std::runtime_error("OpenAL failed to open playback device.");
		}
	
		// Get playback device name
		if (alcIsExtensionPresent(alc_device, "ALC_ENUMERATE_ALL_EXT"))
		{
			m_playback_device_name = alcGetString(alc_device, ALC_ALL_DEVICES_SPECIFIER);
		}
		if (m_playback_device_name.empty() || alcGetError(alc_device) != AL_NO_ERROR)
		{
			m_playback_device_name = alcGetString(alc_device, ALC_DEVICE_SPECIFIER);
		}
	
		// Create OpenAL context
		ALCcontext* alc_context = alcCreateContext(alc_device, nullptr);
		if (!alc_context)
		{
			alcCloseDevice(alc_device);
			throw std::runtime_error(std::format("OpenAL failed to create context: ALC error code {}", alcGetError(alc_device)));
		}
	
		// Make OpenAL context current
		if (alcMakeContextCurrent(alc_context) == ALC_FALSE)
		{
			alcDestroyContext(alc_context);
			alcCloseDevice(alc_device);
			throw std::runtime_error(std::format("OpenAL failed to make context current: ALC error code {}", alcGetError(alc_device)));
		}
	
		// Save pointers to ALC device and context
		m_alc_device = alc_device;
		m_alc_context = alc_context;
	
		// Construct listener
		m_listener = std::make_unique<listener>();
	}

	sound_system::~sound_system()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(reinterpret_cast<ALCcontext*>(m_alc_context));
		alcCloseDevice(reinterpret_cast<ALCdevice*>(m_alc_device));
	}
}
