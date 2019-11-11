#pragma once

#include <stdint.h>

// Our brand of MIDI song data has three uint16_ts per event
// Delay in clocks per quarter note (480 / quarter note standard)
// Which voice to act on
// What MIDI note to play: 0-127 are note on events, 255 is note off
// See the Python routine in app-midi for a converter
//   and lots of examples

#define SONGLENGTH(x) (sizeof(x)/sizeof(uint16_t)/3)
// 48000000 clock speed / 120 BPM / 480 clocks per quarter note
#define BPM(x)    (48000000 / (x / 60) / 480)

// Takes degrees of the scale, turns them into midi notes
// Ex: scale_major(3) -> 4, b/c a major 3rd is 4 semitones
// Use it like this: midi_table[root + scale_major(j)]; 
// Gives you the phase increment corresponding to the j'th note in the major scale based on "root"
int scale_major(int i);

// Takes song data, plays it.  
// This needs to be polled at least as often 
// as the fastest time interval between events.
// If not, it'll come in late, but it's not buffered.
// I.e. if these stack up, things drift out of sync
// For most music, this is OK.  But only you know for sure.
// Given an array with the music for Tetris in it:
// midi_play_song(tetris, SONGLENGTH(tetris), BPM(120)); 
void midi_play_song(uint16_t songArray[][3], uint16_t length, uint32_t clocksPerClick);



