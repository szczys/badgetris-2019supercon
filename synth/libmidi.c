#include "libmidi.h"
#include "libsynth.h"
#include "midi_note_increments.h"

#include "mach_defines.h"
#include "sdk.h"

// These should probably be somewhere else!
uint32_t time() {
	uint32_t cycles;
	asm volatile ("rdcycle %0" : "=r"(cycles));
	return cycles;
}

#define SECONDS 48000000
#define MILLIS  48000

int scale_major(int i){
	int scale_table_major[] = {0, 2, 4, 5, 7, 9, 11};
	return (i/7)*12 + scale_table_major[i%7];
}

void midi_play_song(uint16_t songArray[][3], uint16_t songLength, uint32_t clocksPerClick){
	static uint16_t song_step     = 0;
	static uint8_t  gates         = 0;
	static uint32_t last_time     = 0;
	static uint32_t time_interval = 0;
	uint32_t delta, voice, note;

	// Update only if enough time has elapsed
	if (time() - last_time >= time_interval ){
		song_step++;
		last_time = time();

		// Reset
		if (song_step > (songLength-1)){
			song_step = 0;
		}

		// Get parameters
		delta = songArray[song_step][0];
		voice = songArray[song_step][1];
		note  = songArray[song_step][2];

		// Set next timer
		time_interval = clocksPerClick*delta; 

		// Play note or turn note off, accordingly
		if (note < 128){ // range valid midi notes
			synth_now->voice[voice].phase_inc = midi_table[note-24];
			gates |= (1 << voice);
		}
		else {
			gates &= ~(1 << voice);
		}
		synth_now->voice_force = gates;
	}
}
