/*
 Name:		HID_to_MIDI.ino
 Created:	6/8/2024 6:47:52 AM
 Author:	cybir
mouse and keyboard in boot mode to midi
mouse uses notes 123-127 because most keyboards don't go that high
*/

#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>
#include "lf310.h"
#define REPORT_BUFFER_SIZE 128//best way to log for keyboard report data
#define MOUSE_RESOLUTION 512//higher number means less sensitive mouse
#define MOUSE_DIVISOR 4//should always be mouseresolution/128

#define JOY_OFFSET 102//top channel for keyboards tested so far is 101
//also the mouse is mapped to start at 123, so this fits perfectly once you add the konami code.
int mouseX;
int mouseY;

//joystick analog stick pos data
uint8_t oldX = 128;
uint8_t oldY = 128;
uint8_t oldZ = 128;
uint8_t oldRz = 128;
byte joyState[13];//holds joy button data

USB     Usb;
USBHub  Hub1(&Usb);//probably only need one hub
//USBHub  Hub2(&Usb);
//USBHub  Hub3(&Usb);
//USBHub  Hub4(&Usb);
LF310 lf310(&Usb);//joy class instance, logitech f310 ONLY
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);
HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE > HidComposite(&Usb);//THIS MUST GO AFTER THE OTHER MOUSE AND KBD DECLARATIONS FOR A HUB TO WORK
bool konami[11];

class KbdRptParser : public KeyboardReportParser {
public:
	bool mouseOrJoyDevice = false;
	uint8_t incomingData[REPORT_BUFFER_SIZE];
	uint8_t oldData[REPORT_BUFFER_SIZE];

	void sendMidiNote(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
		if (pitch > 127) pitch = 127;
		if (velocity > 127) velocity = 127;
		Serial.write(0x90);
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

class MouseRptParser : public MouseReportParser {
public:
	void sendMidiNote(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
		if (pitch > 127) pitch = 127;
		if (velocity > 127) velocity = 127;
		Serial.write(0x90);
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

void sendMidiNoteJoy(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
	if (pitch > 127) pitch = 127;
	if (velocity > 127) velocity = 127;
	Serial.write(0x90);
	Serial.write(pitch);
	Serial.write(velocity);
	//Serial.println("Note: " + String(pitch) + "Vel: " + String(velocity));
	delay(1);
}

void konamiReset() {
	for (int i = 0; i < sizeof(konami); i++) {
		konami[i] = 0;
	}
}

void konamiCheck(LF310Data data) {
	bool itHappened = false;
	//code is up up dn dn L R L R B A START
	if (konami[0] == false) {
		if (data.btn.dPad == DPAD_UP) konami[0] = true;
		else {
			konamiReset();
		return;
		}
	}
	else if (konami[1] == false) {
		if (data.btn.dPad == DPAD_UP) konami[1] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[2] == false) {
		if (data.btn.dPad == DPAD_DOWN) konami[2] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[10] == false) {
		if (data.btn.dPad == DPAD_DOWN) konami[10] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[3] == false) {
		if (data.btn.dPad == DPAD_LEFT) konami[3] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[4] == false) {
		if (data.btn.dPad == DPAD_RIGHT) konami[4] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[5] == false) {
		if (data.btn.dPad == DPAD_LEFT) konami[5] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[6] == false) {
		if (data.btn.dPad == DPAD_RIGHT) konami[6] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[7] == false) {
		if (data.btn.Bbutton >0) konami[7] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[8] == false) {
		if (data.btn.Abutton > 0) konami[8] = true;
		else {
			konamiReset();
			return;
		}
	}
	else if (konami[9] == false) {
		if (data.btn.Startbutton > 0) {
			konami[9] = true;
			itHappened = true;
		}
		else {
			konamiReset();
			return;
		}
	}

	if (itHappened)sendMidiNoteJoy(122, 127);
	delay(25);
	if (itHappened)sendMidiNoteJoy(122, 0);
	konamiReset();
}

void lf310JoystickParse() {
	if (lf310.connected()) {
		if (lf310.lf310Data.X != oldX) {
			sendMidiNoteJoy(JOY_OFFSET, lf310.lf310Data.X / 2);
			oldX = lf310.lf310Data.X;
		}

		if (lf310.lf310Data.Y != oldY) {
			sendMidiNoteJoy(JOY_OFFSET + 1, lf310.lf310Data.Y / 2);
			oldY = lf310.lf310Data.Y;
		}

		if (lf310.lf310Data.Z != oldZ) {
			sendMidiNoteJoy(JOY_OFFSET + 2, lf310.lf310Data.Z / 2);
			oldZ = lf310.lf310Data.Z;
		}

		if (lf310.lf310Data.Rz != oldRz) {
			sendMidiNoteJoy(JOY_OFFSET + 3, lf310.lf310Data.Rz / 2);
			oldRz = lf310.lf310Data.Rz;
		}

		if (lf310.buttonClickState.Xbutton != joyState[0]) {//
			sendMidiNoteJoy(JOY_OFFSET + 4, lf310.buttonClickState.Xbutton * 127);
			joyState[0] = lf310.buttonClickState.Xbutton;
		}

		if (lf310.buttonClickState.Abutton != joyState[1]) {//
			sendMidiNoteJoy(JOY_OFFSET + 5, lf310.buttonClickState.Abutton * 127);
			joyState[1] = lf310.buttonClickState.Abutton;
		}

		if (lf310.buttonClickState.Bbutton != joyState[2]) {//
			sendMidiNoteJoy(JOY_OFFSET + 6, lf310.buttonClickState.Bbutton * 127);
			joyState[2] = lf310.buttonClickState.Bbutton;
		}

		if (lf310.buttonClickState.Ybutton != joyState[3]) {//
			sendMidiNoteJoy(JOY_OFFSET + 7, lf310.buttonClickState.Ybutton * 127);
			joyState[3] = lf310.buttonClickState.Ybutton;
		}

		if (lf310.buttonClickState.LBbutton != joyState[4]) {//
			sendMidiNoteJoy(JOY_OFFSET + 8, lf310.buttonClickState.LBbutton * 127);
			joyState[4] = lf310.buttonClickState.LBbutton;
		}

		if (lf310.buttonClickState.RBbutton != joyState[5]) {//
			sendMidiNoteJoy(JOY_OFFSET + 9, lf310.buttonClickState.RBbutton * 127);
			joyState[5] = lf310.buttonClickState.RBbutton;
		}

		if (lf310.buttonClickState.LTbutton != joyState[6]) {//
			sendMidiNoteJoy(JOY_OFFSET + 10, lf310.buttonClickState.LTbutton * 127);
			joyState[6] = lf310.buttonClickState.LTbutton;
		}

		if (lf310.buttonClickState.RTbutton != joyState[7]) {//
			sendMidiNoteJoy(JOY_OFFSET + 11, lf310.buttonClickState.RTbutton * 127);
			joyState[7] = lf310.buttonClickState.RTbutton;
		}

		if (lf310.buttonClickState.Backbutton != joyState[8]) {//
			sendMidiNoteJoy(JOY_OFFSET + 12, lf310.buttonClickState.Backbutton * 127);
			joyState[8] = lf310.buttonClickState.Backbutton;
		}

		if (lf310.buttonClickState.Startbutton != joyState[9]) {//
			sendMidiNoteJoy(JOY_OFFSET + 13, lf310.buttonClickState.Startbutton * 127);
			joyState[9] = lf310.buttonClickState.Startbutton;
		}

		if (lf310.buttonClickState.LJSP != joyState[10]) {//
			sendMidiNoteJoy(JOY_OFFSET + 14, lf310.buttonClickState.LJSP*127);
			joyState[10] = lf310.buttonClickState.LJSP;
		}

		if (lf310.buttonClickState.RJSP != joyState[11]) {//
			sendMidiNoteJoy(JOY_OFFSET + 15, lf310.buttonClickState.RJSP * 127);
			joyState[11] = lf310.buttonClickState.RJSP;
		}

		if (lf310.lf310Data.btn.dPad != joyState[12]) {//
			switch (lf310.lf310Data.btn.dPad) {//send new data
			case DPAD_UP:
				sendMidiNoteJoy(JOY_OFFSET + 16, 127);
				break;
			case DPAD_RIGHT:
				sendMidiNoteJoy(JOY_OFFSET + 17, 127);
				break;
			case DPAD_DOWN:
				sendMidiNoteJoy(JOY_OFFSET + 18, 127);
				break;
			case DPAD_LEFT:
				sendMidiNoteJoy(JOY_OFFSET + 19, 127);
				break;
			case DPAD_OFF:
				break;
			default:

				break;
			}
			switch (joyState[12]) {//turn old data off
			case DPAD_UP:
				sendMidiNoteJoy(JOY_OFFSET + 16, 0);
				break;
			case DPAD_RIGHT:
				sendMidiNoteJoy(JOY_OFFSET + 17, 0);
				break;
			case DPAD_DOWN:
				sendMidiNoteJoy(JOY_OFFSET + 18, 0);
				break;
			case DPAD_LEFT:
				sendMidiNoteJoy(JOY_OFFSET + 19, 0);
				break;
			case DPAD_OFF:
				break;
			default:

				break;
			}
			joyState[12] = lf310.lf310Data.btn.dPad;
		}
		konamiCheck(lf310.lf310Data);//check to see if someone did this
	}
}
KbdRptParser KbdPrs;
MouseRptParser MousePrs;


void setup() {
	Serial.begin(31250);//midi baud rate is 31250
	Usb.Init();
	delay(200);
	HidComposite.SetReportParser(0, &KbdPrs);
	HidComposite.SetReportParser(1, &MousePrs);
	HidKeyboard.SetReportParser(0, &KbdPrs);
	HidMouse.SetReportParser(0, &MousePrs);
}

void loop() {
	Usb.Task();
	lf310JoystickParse();
}
