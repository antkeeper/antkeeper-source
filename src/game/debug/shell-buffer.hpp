// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SHELL_BUFFER_HPP
#define ANTKEEPER_SHELL_BUFFER_HPP

#include <engine/scene/text.hpp>
#include <memory>
#include <sstream>

/// Shell output string buffer.
class shell_buffer: public std::stringbuf
{
public:
	void set_text_object(std::shared_ptr<scene::text> object);
	
	int sync() override;
	
private:
	std::shared_ptr<scene::text> m_text_object;
};

#endif // ANTKEEPER_SHELL_BUFFER_HPP
