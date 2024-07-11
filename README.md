Code for an arduino based device to take usb device input data and output MIDI data for control of any hardware or software that accepts MIDI as an input protocol.

![IMG_20240626_141500898](https://github.com/Cybirbug/Arduino-USB-to-MIDI/assets/41763278/6c9e35b6-fa16-49a8-8ebd-f4a89f08e6a5)

Limited intent is for control over a lighting or sound console. Support will probably be limited to one keyboard, mouse, and gamepad, BUT smaller keyboards with fewer keys may be acceptable.

Keyboards output notes based on the ascii value of the key pressed

mice output midi values 123-127

the logitech F310 fits (barely) between the two

there are some gaps in keyboard values that COULD be filled... No reason to do so yet.

Currently only supports basic MIDI note-on/note/velocity packets

Most or all keyboards should work, but some media control keys may not work.

Most or all mice should work in boot mode, but this limits wheel functionality (not working) for now.

Logitech F310 gamepad support using a custom version of this library https://github.com/AlanFord/Logitech_F310_and_Arduino Thanks to Mr Alan Ford for his work!

This library doesn't yet support analog functions of L/R triggers.

More gamepads will probably only be supported if someone else writes a driver and adds it


ToDo: 
- split lf310 into its own class outside of the main file
- mouse wheel support
- fix lf310 LR trigger values
