/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sound-system.hpp"
#include <stdexcept>
#include "dr_libs/dr_wav.h"

SoundSystem::SoundSystem(ComponentManager* componentManager):
	System(componentManager),
	entityGroup(componentManager),
	device(nullptr),
	context(nullptr)
{
	device = alcOpenDevice(nullptr);
	if (!device)
	{
		throw std::runtime_error("SoundSystem::SoundSystem(): Failed to open audio device.");
	}

	context = alcCreateContext(device, nullptr);
	if (!alcMakeContextCurrent(context))
	{
		throw std::runtime_error("SoundSystem::SoundSystem(): Failed to create audio context.");
	}

	ALfloat listenerOrientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, listenerOrientation);

	alGenSources((ALuint)1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);

	alGenBuffers((ALuint)1, &buffer);

	// Load wav file
	{
		const char* filename = "/home/cjhoward/projects/antkeeper/modules/antkeeper-data/sounds/shutter.wav";
		unsigned int channels;
		unsigned int sampleRate;
		drwav_uint64 frameCount;
		std::int16_t* sampleData = drwav_open_file_and_read_pcm_frames_s16(filename, &channels, &sampleRate, &frameCount);

		if (sampleData == nullptr)
		{
			throw std::runtime_error("Couldn't load wav file");
			drwav_free(sampleData);
		}
		
		bool stereo = (channels > 1);
		ALenum format = (stereo) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

		std::size_t sampleCount = frameCount * channels;
		std::size_t sampleDataSize = sampleCount * sizeof(drwav_int16);

		alBufferData(buffer, format, sampleData, sampleDataSize, sampleRate);
	}

	alSourcei(source, AL_BUFFER, buffer);
	//alSourcePlay(source);

}

SoundSystem::~SoundSystem()
{
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void SoundSystem::scrot()
{
	alSourcePlay(source);
}

void SoundSystem::update(float t, float dt)
{
	auto members = entityGroup.getMembers();
	for (const SoundSourceEntityGroup::Member* member: *members)
	{
		TransformComponent* transform = std::get<0>(member->components);
		SoundSourceComponent* sound = std::get<1>(member->components);
	}
}

void SoundSystem::memberRegistered(const SoundSourceEntityGroup::Member* member)
{
	TransformComponent* transform = std::get<0>(member->components);
	SoundSourceComponent* sound = std::get<1>(member->components);
}

void SoundSystem::memberUnregistered(const SoundSourceEntityGroup::Member* member)
{
	TransformComponent* transform = std::get<0>(member->components);
	SoundSourceComponent* sound = std::get<1>(member->components);
}

