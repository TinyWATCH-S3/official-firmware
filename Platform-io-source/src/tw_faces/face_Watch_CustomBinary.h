#pragma once

#include "tw_faces/tw_face.h"

class FaceWatch_CustomBinary : public tw_face
{
	public:
		void setup(void);
		void draw(bool force);

		bool process_touch(touch_event_t touch_event);

	private:
		String version = "1.0";

		// Binary Watch Colour Pallette - On
		const uint32_t on_colors[3] = {
			RGB(127, 0, 0),
			RGB(0, 127, 0),
			RGB(0, 0, 191),
		};

		// Binary Watch Colour Pallette - Off
		const uint32_t off_colors[3] = {
			RGB(32, 0, 0),
			RGB(0, 32, 0),
			RGB(0, 0, 32),
		};

		const String months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

		// Settings
		bool show_borders = true;

		// Kept for Caching
		uint8_t hours = 0;
		uint8_t mins = 0;
		uint16_t day = 0;
		uint16_t month = 0;
		uint16_t year = 0;
		int8_t day_offset = -16;
		uint8_t text_xoffset = 0;
};

extern FaceWatch_CustomBinary face_watch_custom_binary;
