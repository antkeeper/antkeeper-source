// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <AL/al.h>
#include <dr_wav.h>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#include <engine/audio/sound-wave.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/utility/sized-types.hpp>
#include <bit>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace engine::audio
{
	sound_wave::sound_wave
	(
		u32 channels,
		u32 sample_rate,
		u32 bits_per_sample,
		std::span<const std::byte> samples
	)
	{
		// Determine OpenAL format
		ALenum al_format = AL_NONE;
		if (channels == 1)
		{
			if (bits_per_sample == 8)
			{
				al_format = AL_FORMAT_MONO8;
			}
			else if (bits_per_sample == 16)
			{
				al_format = AL_FORMAT_MONO16;
			}
		}
		else if (channels == 2)
		{
			if (bits_per_sample == 8)
			{
				al_format = AL_FORMAT_STEREO8;
			}
			else if (bits_per_sample == 16)
			{
				al_format = AL_FORMAT_STEREO16;
			}
		}
	
		if (al_format == AL_NONE)
		{
			throw std::runtime_error(std::format("OpenAL does not support sound wave format ({}-channel, {} bps)", channels, bits_per_sample));
		}
	
		// Generate buffer
		alGenBuffers(1, &m_al_buffer);
		if (auto error = alGetError(); error != AL_NO_ERROR)
		{
			throw std::runtime_error(std::format("OpenAL failed to generate buffer: {}", alGetString(error)));
		}
	
		// Write data to buffer
		alBufferData(m_al_buffer, al_format, samples.data(), static_cast<ALsizei>(samples.size()), static_cast<ALsizei>(sample_rate));
		if (auto error = alGetError(); error != AL_NO_ERROR)
		{
			alDeleteBuffers(1, &m_al_buffer);
			throw std::runtime_error(std::format("OpenAL failed to write data to buffer: {}", alGetString(error)));
		}
	
		// Set sound wave info
		m_channels = channels;
		m_sample_rate = sample_rate;
		m_bits_per_sample = bits_per_sample;
		m_size = samples.size();
		m_duration = static_cast<float>(static_cast<double>(m_size) / (m_sample_rate * m_channels * (m_bits_per_sample >> 3)));
	}

	sound_wave::~sound_wave()
	{
		alDeleteBuffers(1, &m_al_buffer);
	}
}

namespace engine::resources
{
	using namespace engine::audio;

	/// Loads a sound wave with dr_wav.
	std::unique_ptr<sound_wave> load_sound_wave_dr_wav(std::shared_ptr<deserialize_context> ctx)
	{
		// Read file into file buffer
		std::vector<std::byte> file_buffer(ctx->size());
		ctx->read8(file_buffer.data(), file_buffer.size());
		
		// Open WAV file from buffer
		drwav wav;
		if (!drwav_init_memory(&wav, file_buffer.data(), ctx->size(), nullptr))
		{
			throw deserialize_error("Failed to open WAV file with dr_wav");
		}
		
		// Get WAV file info
		auto channels = static_cast<u32>(wav.channels);
		auto sample_rate = static_cast<u32>(wav.sampleRate);
		auto bits_per_sample = static_cast<u32>(wav.bitsPerSample);
		
		if (bits_per_sample == 8)
		{
			// Allocate unsigned 8-bit PCM samples
			std::vector<u8> samples(wav.channels * wav.totalPCMFrameCount);
			
			// Decode samples
			if (drwav_read_pcm_frames(&wav, wav.totalPCMFrameCount, samples.data()) != wav.totalPCMFrameCount)
			{
				// Unitialize wave file
				drwav_uninit(&wav);
				
				throw deserialize_error("dr_wav failed to decode 8-bit WAV file");
			}
			
			// Unitialize wave file
			drwav_uninit(&wav);
			
			// Construct OpenAL sound wave
			return std::make_unique<sound_wave>
			(
				channels,
				sample_rate,
				bits_per_sample,
				std::as_bytes(std::span{samples})
			);
		}
		else
		{
			if (bits_per_sample != 16)
			{
				log_trace("{}-bit WAV data will be converted to 16-bit", bits_per_sample);
			}
			
			// Allocate signed 16-bit PCM samples
			std::vector<i16> samples(wav.channels * wav.totalPCMFrameCount);
			
			// Decode and convert samples to signed 16-bit
			if (drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, samples.data()) != wav.totalPCMFrameCount)
			{
				// Unitialize wave file
				drwav_uninit(&wav);
				
				throw deserialize_error(std::format("dr_wav failed to decode {}-bit WAV data and convert to 16-bit", bits_per_sample));
			}
			
			// Unitialize wave file
			drwav_uninit(&wav);
			
			// Construct OpenAL sound wave
			return std::make_unique<sound_wave>
			(
				channels,
				sample_rate,
				16,
				std::as_bytes(std::span{samples})
			);
		}
	}
	
	/// Vorbisfile I/O read callback.
	size_t vorbisfile_io_read(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		if (size && nmemb)
		{
			deserialize_context& ctx = *static_cast<deserialize_context*>(datasource);
			return ctx.read8(reinterpret_cast<std::byte*>(ptr), size * nmemb) / size;
		}
		
		return 0;
	}
	
	/// Vorbisfile I/O seek callback.
	int vorbisfile_io_seek(void* datasource, ogg_int64_t offset, int whence)
	{
		deserialize_context& ctx = *static_cast<deserialize_context*>(datasource);
		
		if (whence == SEEK_SET)
		{
			ctx.seek(static_cast<usize>(offset));
			return 0;
		}
		else if (whence == SEEK_CUR)
		{
			ctx.seek(ctx.tell() + offset);
			return 0;
		}
		else if (whence == SEEK_END)
		{
			ctx.seek(ctx.size() + offset);
			return 0;
		}
		
		return 1;
	}
	
	/// Vorbisfile I/O tell callback.
	long vorbisfile_io_tell(void* datasource)
	{
		deserialize_context& ctx = *static_cast<deserialize_context*>(datasource);
		return static_cast<long>(ctx.tell());
	}
	
	/// Loads a sound wave with Vorbisfile.
	std::unique_ptr<sound_wave> load_sound_wave_vorbisfile(std::shared_ptr<deserialize_context> ctx)
	{
		// Setup Vorbisfile I/O callbacks
		static const ov_callbacks vorbisfile_io_callbacks
		{
			&vorbisfile_io_read,
			&vorbisfile_io_seek,
			nullptr,
			&vorbisfile_io_tell
		};
		
		// Open Ogg/Vorbis file
		OggVorbis_File vf;
		if (auto error = ov_open_callbacks(ctx.get(), &vf, nullptr, 0, vorbisfile_io_callbacks); error != 0)
		{
			throw deserialize_error(std::format("Vorbisfile failed to open Ogg/Vorbis file: error code {}", error));
		}
		
		// Get Ogg/Vorbis file info
		vorbis_info* vf_info = ov_info(&vf, -1);
		if (!vf_info)
		{
			// Close Ogg/Vorbis file
			ov_clear(&vf);
			
			throw deserialize_error("Vorbisfile failed to provide Ogg/Vorbis file information");
		}
		
		auto channels = static_cast<u32>(vf_info->channels);
		auto sample_rate = static_cast<u32>(vf_info->rate);
		
		// Set sample chunk size
		const usize chunk_size = 1024 * 64;
		
		std::vector<char> samples;
		usize total_bytes_read = 0;
		long bytes_read = 0;
		int bitstream = 0;
		
		do
		{
			// Allocate a chunk of samples
			samples.resize(total_bytes_read + chunk_size);
			
			// Read samples
			bytes_read = ov_read
			(
				&vf,
				samples.data() + total_bytes_read,
				chunk_size,
				std::endian::native == std::endian::big,
				sizeof(i16),
				1,
				&bitstream
			);
			
			if (bytes_read < 0)
			{
				// Close Ogg/Vorbis file
				ov_clear(&vf);
				
				throw deserialize_error(std::format("Vorbis failed to read Ogg/Vorbis file: error code {}", bytes_read));
			}
			
			total_bytes_read += bytes_read;
		}
		while (bytes_read > 0);
		
		// Close Ogg/Vorbis file
		ov_clear(&vf);
		
		// Construct OpenAL sound wave
		return std::make_unique<sound_wave>
		(
			channels,
			sample_rate,
			16,
			std::as_bytes(std::span{samples}).subspan(0, total_bytes_read)
		);
	}

	template <>
	std::unique_ptr<sound_wave> resource_loader<sound_wave>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		if (ctx->path().extension() == ".wav")
		{
			// Load WAV with dr_wav
			return load_sound_wave_dr_wav(ctx);
		}
		else if (ctx->path().extension() == ".ogg")
		{
			// Load Ogg/Vorbis with Vorbisfile
			return load_sound_wave_vorbisfile(ctx);
		}
		else
		{
			throw std::runtime_error(std::format("Sound wave file extension not recognized ({})", ctx->path().extension().string()));
		}
	}
}
