#pragma once

#include "defs.h"

class KbdRptParser : public KeyboardReportParser {
public:
	bool mouseOrJoyDevice = false;
	uint8_t incomingData[REPORT_BUFFER_SIZE];
	uint8_t oldData[REPORT_BUFFER_SIZE];

	void sendMidiNote(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
		if (pitch > 127) pitch = 127;
		if (velocity > 127) velocity = 127;
		Serial.write(KEYBOARD_NOTE_ON);
		Serial.write(pitch);
		Serial.write(velocity);
		delay(1);//allow time for note to be sent! prevents crashes.
	}

	virtual void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) {
		for (int i = 0; i < REPORT_BUFFER_SIZE; i++) {
			if (i < len)incomingData[i] = buf[i];
			else incomingData[i] = 0;

			if (incomingData[i] != oldData[i]) {//log the value if it's new, and make sure it gets set to zero when released.
				if (incomingData[i] != 0)sendMidiNote(incomingData[i], 127);//activate note
				else sendMidiNote(oldData[i], 0);//deactivate the note
				oldData[i] = incomingData[i];
			}
		}
	};
};