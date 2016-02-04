/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "ui_alphanum.hpp"

#include "ch.h"

#include "ff.h"
#include "portapack.hpp"
#include "radio.hpp"

#include "hackrf_hal.hpp"
#include "portapack_shared_memory.hpp"

#include <cstring>

using namespace hackrf::one;

namespace ui {
	
AlphanumView::AlphanumView(
	NavigationView& nav,
	char txt[],
	uint8_t max_len
) {
	_max_len = max_len;
	_lowercase = false;
	
	static constexpr Style style_alpha {
		.font = font::fixed_8x16,
		.background = Color::red(),
		.foreground = Color::black(),
	};
	
	static constexpr Style style_num {
		.font = font::fixed_8x16,
		.background = Color::yellow(),
		.foreground = Color::black(),
	};
	
	txtidx = 0;
	memcpy(txtinput, txt, max_len+1);
	
	add_child(&text_input);

	const auto button_fn = [this](Button& button) {
		this->on_button(button);
	};

	size_t n = 0;
	for(auto& button : buttons) {
		add_child(&button);
		button.on_select = button_fn;
		button.set_parent_rect({
			static_cast<Coord>((n % 5) * button_w),
			static_cast<Coord>((n / 5) * button_h + 18),
			button_w, button_h
		});
		if ((n < 10) || (n == 39))
			button.set_style(&style_num);
		else
			button.set_style(&style_alpha);
		n++;
	}
	set_uppercase();
	
	add_child(&button_lowercase);
	button_lowercase.on_select = [this, &nav, txt, max_len](Button&) {
		if (_lowercase == true) {
			_lowercase = false;
			button_lowercase.set_text("UC");
			set_uppercase();
		} else {
			_lowercase = true;
			button_lowercase.set_text("LC");
			set_lowercase();
		}
	};

	add_child(&button_done);
	button_done.on_select = [this, &nav, txt, max_len](Button&) {
		memcpy(txt, txtinput, max_len+1);
		on_changed(this->value());
		nav.pop();
	};

	update_text();
}

void AlphanumView::set_uppercase() {
	const char* const key_caps = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ. !<";

	size_t n = 0;
	for(auto& button : buttons) {
		//add_child(&button);
		const std::string label {
			key_caps[n]
		};
		button.set_text(label);
		n++;
	}
}


void AlphanumView::set_lowercase() {
	const char* const key_caps = "0123456789abcdefghijklmnopqrstuvwxyz. !<";

	size_t n = 0;
	for(auto& button : buttons) {
		//add_child(&button);
		const std::string label {
			key_caps[n]
		};
		button.set_text(label);
		n++;
	}
}

void AlphanumView::focus() {
	button_done.focus();
}

char * AlphanumView::value() {
	return txtinput;
}

void AlphanumView::on_button(Button& button) {
	const auto s = button.text();
	if( s == "<" ) {
		char_delete();
	} else {
		char_add(s[0]);
	}
	update_text();
}

void AlphanumView::char_add(const char c) {
	if (txtidx < _max_len) {
		txtinput[txtidx] = c;
		txtidx++;
	}
}

void AlphanumView::char_delete() {
	if (txtidx) {
		txtidx--;
		txtinput[txtidx] = ' ';
	}
}

void AlphanumView::update_text() {
	text_input.set(txtinput);
}

}
