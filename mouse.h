#pragma once
#include "defs.h"

//also the mouse is mapped to start at 123, so this fits perfectly once you add the konami code.
int mouseX;
int mouseY;

class MouseRptParser : public MouseReportParser {
public:
	//also the mouse is mapped to start at 123, so this fits perfectly once you add the konami code.
	int mouseX;
	int mouseY;

	void sendMidiNote(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
		if (pitch > 127) pitch = 127;
		if (velocity > 127) velocity = 127;
		Serial.write(MOUSE_NOTE_ON);
		Serial.write(pitch);
		Serial.write(velocity);
		delay(1);
	}
protected:
	void OnMouseMove(MOUSEINFO* mi) {//notes 127 and 126
		mouseX += mi->dX;
		mouseY -= mi->dY;//INVERT DIRECTION
		if (mouseX > MOUSE_RESOLUTION) mouseX = MOUSE_RESOLUTION;
		else if (mouseX < 0)mouseX = 0;
		if (mouseY > MOUSE_RESOLUTION) mouseY = MOUSE_RESOLUTION;
		else if (mouseY < 0)mouseY = 0;

		sendMidiNote(127, mouseY / MOUSE_DIVISOR);
		sendMidiNote(126, mouseX / MOUSE_DIVISOR);
	};
	void OnLeftButtonUp(MOUSEINFO* mi) {//note 124
		sendMidiNote(123, 0);
	};
	void OnLeftButtonDown(MOUSEINFO* mi) {
		sendMidiNote(123, 127);
	};
	void OnRightButtonUp(MOUSEINFO* mi) {//note 126
		sendMidiNote(125, 0);
	};
	void OnRightButtonDown(MOUSEINFO* mi) {
		sendMidiNote(125, 127);
	};
	void OnMiddleButtonUp(MOUSEINFO* mi) {//note 125
		sendMidiNote(124, 0);
	};
	void OnMiddleButtonDown(MOUSEINFO* mi) {
		sendMidiNote(124, 127);
	};
};