/*
 Created:	6/8/2024 6:47:52 AM
 Author:	Nick Marstol

 ////
 uses libaries under this license:
 This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

////
mouse and keyboard in boot mode to midi
mouse uses notes 123-127 
joy uses notes 102 thru 122
most keyboards in boot mode appear to go up to 101 for output values
uses libaries:
https://www.arduino.cc/reference/en/libraries/usb-host-shield-library-2.0/
https://github.com/AlanFord/Logitech_F310_and_Arduino
the second includes a patch for the usb shield 2.0 library, June 2024 it still needed patching
everything seems to work better with a hub as a filter if connected to a PC

*/

#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>
//#include "joy.h"
#include "mouse.h"
#include "keyboard.h"
#include "lf310.h"
#include "defs.h"

//joystick analog stick pos data
uint8_t oldX = 128;
uint8_t oldY = 128;
uint8_t oldZ = 128;
uint8_t oldRz = 128;
byte joyState[14];//holds joy button data

USB     Usb;
USBHub  Hub1(&Usb);//probably only need one hub but what the heck, someone might plug in multiple keypads
USBHub  Hub2(&Usb);
USBHub  Hub3(&Usb);
USBHub  Hub4(&Usb);
KbdRptParser KbdPrs;
MouseRptParser MousePrs;
LF310 lf310(&Usb);//joy class instance, logitech f310 ONLY
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);
HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE > HidComposite(&Usb);//THIS MUST GO AFTER THE OTHER MOUSE AND KBD DECLARATIONS FOR A HUB TO WORK
bool konami[11];


void sendMidiNoteJoy(byte pitch, byte velocity) {//send noteOn command 0x90 plus data
	if (pitch > 127) pitch = 127;
	if (velocity > 127) velocity = 127;
	if (konamiCheck(lf310.lf310Data, velocity)) {
		//sendMidiNoteJoy(122, 127);//check to see if someone did this
		Serial.write(JOY_NOTE_ON);
		Serial.write(122);
		Serial.write(127);
		delay(20);
		Serial.write(JOY_NOTE_ON);
		Serial.write(122);
		Serial.write(0);
	}
	else {
		Serial.write(JOY_NOTE_ON);
		Serial.write(pitch);
		Serial.write(velocity);
		delay(1);
	}
	//Serial.println("Note: " + String(pitch) + "Vel: " + String(velocity));
}

void konamiReset() {
	for (int i = 0; i < sizeof(konami); i++) {
		konami[i] = 0;
	}
}

bool konamiCheck(LF310Data data, byte vel) {
	if (vel == 0) return;
	//code is up up dn dn L R L R B A START
	if (konami[0] == false) {
		if (data.btn.dPad == DPAD_UP) { konami[0] = true; return false; }
		else {
			konamiReset();
		return false;
		}
	}
	else if (konami[1] == false) {
		if (data.btn.dPad == DPAD_UP) {konami[1] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[2] == false) {
		if (data.btn.dPad == DPAD_DOWN){ konami[2] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[10] == false) {
		if (data.btn.dPad == DPAD_DOWN) {konami[10] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[3] == false) {
		if (data.btn.dPad == DPAD_LEFT) {konami[3] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[4] == false) {
		if (data.btn.dPad == DPAD_RIGHT) {konami[4] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[5] == false) {
		if (data.btn.dPad == DPAD_LEFT) {konami[5] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[6] == false) {
		if (data.btn.dPad == DPAD_RIGHT){ konami[6] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[7] == false) {
		if (data.btn.Bbutton >0) {konami[7] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[8] == false) {
		if (data.btn.Abutton > 0){ konami[8] = true; return false;
	}
		else {
			konamiReset();
			return false;
		}
	}
	else if (konami[9] == false) {
		if (data.btn.Startbutton > 0) {
			konami[9] = true;
		}
		else {
			konamiReset();
			return false;
		}
	}
	//Serial.println("Konami!");
	konamiReset(); 
	return true;//sendMidiNoteJoy(122, 127);
	delay(25);//delay one dmx framesendMidiNoteJoy(122, 0);
}

void lf310JoystickParse() {
	//Serial.println(lf310.lf310Data.nonfunc);
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

		//byte mode = lf310.lf310Data.Extra & B00001000;//gets the mode bit from the "extra" data
		//if (mode != joyState[13]) {//
		//	//Serial.println("mode: " + mode);
		//	if(mode>0)sendMidiNoteJoy(JOY_OFFSET + 20, 127);
		//	else sendMidiNoteJoy(JOY_OFFSET + 20, 0);
		//	joyState[13] = mode;
		//}

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
		
	}
}

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
