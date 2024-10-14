#include "tw_faces/face_Watch_CustomBinary.h"
#include "bitmaps/bitmaps_general.h"
#include "fonts/Clock_Digits.h"
#include "fonts/RobotoMono_Regular_All.h"
#include "peripherals/battery.h"
#include "peripherals/imu.h"
#include "settings/settings.h"
#include "tinywatch.h"

void FaceWatch_CustomBinary::setup()
{
	if (!is_setup)
	{
		is_setup = true;
	}
}

void FaceWatch_CustomBinary::draw(bool force)
{
	if (force || millis() - next_update > update_period)
	{
		setup();

		next_update = millis();

		if (!is_dragging || !is_cached)
		{
			if (is_dragging)
				is_cached = true;

			// Blank the display
			canvas[canvasid].fillSprite(RGB(0x00, 0x0, 0x0));
			canvas[canvasid].setTextColor(TFT_WHITE);

			uint8_t secs = rtc.get_seconds();

			// Only fetch the mins, hrs, and date once a minute.
			// Also do it for first run (year cant be 0, or 2000)
			if (secs == 0 || year == 0 || year == 2000)
			{
				mins = rtc.get_mins();
				hours = rtc.get_hours();
				day = rtc.get_day();
				month = rtc.get_month();
				year = rtc.get_year();

				// Offset the Date for single/multiple digits
				if (day > 9)
					day_offset = 0;
			}

			uint8_t posmul = 32;
			uint8_t x_space = 5;
			uint8_t y_space = 5;
			uint8_t digit_xoffset = 14;
			uint8_t digit_yoffset = 16;

			uint8_t text_xoffset = (display.width - (6 * (posmul + x_space))) / 2;
			uint8_t y_offset = 176; // i cant for the life of me figure out how to fix this!

			// Set Colours
			int16_t on_color = on_colors[settings.config.custom_binary.binary_clockcolour];
			int16_t off_color = off_colors[settings.config.custom_binary.binary_clockcolour];
			int16_t bdr_color = RGB(0x0A, 0x0A, 0x0A);
			int16_t tim_color = RGB(0xff, 0xff, 0xff);
			int8_t clock_style = settings.config.custom_binary.binary_clockstyle;

			uint8_t box_positions[6] = {5, 4, 3, 2, 1, 0};												   // Start with seconds, then minutes, then hours
			uint8_t box_counts[6] = {4, 3, 4, 3, 4, 2};													   // Not all columns need 4 boxes
			int8_t digit_num[6] = {	static_cast<int8_t>(secs % 10), static_cast<int8_t>(secs / 10), 
								   	static_cast<int8_t>(mins % 10), static_cast<int8_t>(mins / 10), 
									static_cast<int8_t>(hours % 10), static_cast<int8_t>(hours / 10)};     // this pre-calculates the individual digit psoition for each column

			uint8_t half_posmul = posmul / 2;

			int16_t digit_backcolor[6] = {off_color, off_color, off_color, off_color, off_color, off_color};
			canvas[canvasid].setFreeFont(Clock_Digit_7SEG[0]);

			for (uint8_t digit = 0; digit < 6; ++digit) {
    			uint8_t xOffset = text_xoffset + (posmul + x_space) * box_positions[digit];

    			for (uint8_t box_y = 0; box_y < box_counts[digit]; ++box_y) {
       				bool is_on = (digit_num[digit] & (1 << box_y)) != 0;
        			uint8_t yPos = y_offset - (box_y * (posmul + y_space));

        			if (clock_style == 0) {
            			canvas[canvasid].fillRect(xOffset, yPos, posmul, posmul, is_on ? on_color : off_color);
            			if (show_borders) canvas[canvasid].drawRect(xOffset, yPos, posmul, posmul, bdr_color);
        			} else {
            			canvas[canvasid].fillSmoothCircle(xOffset + half_posmul, yPos + half_posmul, half_posmul, is_on ? on_color : off_color);
            			if (show_borders) canvas[canvasid].drawCircle(xOffset + half_posmul, yPos + half_posmul, half_posmul, bdr_color);
        			}
    			}
			
			    // Set background color for the bottom box based on the least significant bit
    			digit_backcolor[digit] = (digit_num[digit] & 1) ? on_color : off_color;

			    // Draw the digit string
    			canvas[canvasid].setTextColor(tim_color, digit_backcolor[digit]);
    			canvas[canvasid].drawString(String(digit_num[digit]), xOffset + digit_xoffset, y_offset + digit_yoffset);
			}

			// Show date below the clock
			canvas[canvasid].setFreeFont(Clock_Digit_7SEG[2]);
			canvas[canvasid].setTextColor(tim_color, RGB(0x00, 0x00, 0x00));
			canvas[canvasid].drawString(String(day), display.center_x - day_offset, display.height - 35);

			// Display Year and Month
			canvas[canvasid].setFreeFont(RobotoMono_Regular[16]);
			canvas[canvasid].setTextColor(tim_color, RGB(0x00, 0x00, 0x00));
			canvas[canvasid].drawString(String(year), display.center_x + 72, display.height - 50);
			canvas[canvasid].drawString(months[month - 1], display.center_x + 64, display.height - 25);

			draw_children(false, 0);

			draw_navigation(canvasid);
		}

		update_screen();
	}
}

bool FaceWatch_CustomBinary::process_touch(touch_event_t touch_event)
{
	if (touch_event.type == TOUCH_TAP)
	{
		// Cycle through the colour pallette, after one full cycle, switch the style (square/circle)
		settings.config.custom_binary.binary_clockcolour++;
		if (settings.config.custom_binary.binary_clockcolour >= 3)
		{
			settings.config.custom_binary.binary_clockstyle = !(settings.config.custom_binary.binary_clockstyle);
			settings.config.custom_binary.binary_clockcolour = 0;
		}
		draw(true);
		return true;
	}
	else if (touch_event.type == TOUCH_DOUBLE)
	{
		display.cycle_clock_face();
		is_dragging = false;
		// draw(true);
		return true;
	}
	else if (touch_event.type == TOUCH_LONG)
	{
		// TODO: Add display of watch specific settings here when the user long presses
	}

	return false;
}

FaceWatch_CustomBinary face_watch_custom_binary;
