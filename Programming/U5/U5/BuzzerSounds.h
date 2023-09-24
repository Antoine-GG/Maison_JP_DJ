#ifndef BUZZER_H
#define BUZZER_H

// Include necessary libraries or header files
#include <avr/io.h>
#include <util/delay.h>
//#include "BuzzerSounds.c"

// Function declarations
void playTone(char note);
void sound_success();
void sound_input();
void sound_fail();
void sound_inputLost();

#endif // BUZZER_H