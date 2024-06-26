Code for an arduino based device to take usb device input data and output MIDI data for control of any hardware or software that accepts MIDI as an input protocol.

Currently only supports basic MIDI note-on/note/velocity packets
Most or all keyboards should work, but some media control keys may not work.
Most or all mice should work in boot mode, but this limits wheel functionality (not working) for now.
Logitech F310 gamepad support using https://github.com/AlanFord/Logitech_F310_and_Arduino This library doesn't yet support analog functions of L/R triggers.

More gamepads will probably only be supported if someone else writes a driver and adds it


ToDo: 
- split into files for classes
- mouse wheel support
- fix lf310 LR trigger values
