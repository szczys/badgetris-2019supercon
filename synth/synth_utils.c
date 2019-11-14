#include "synth_utils.h"
#include "midi_note_increments.h"

// Bunch of defaults that render the synth engine immediately playable on any of the voices
// If you know what you want, you might not need this.  
// But if you just want something to start with, this ia a good bet.
// synth_init(4) will set you up with four sawtooth-wave voices.  Bam!
void synth_init(uint8_t numvoices){
	synth_now->samplerate_div = (1000 - 2);	/* 48M / 1000 = 48 kHz */
	synth_now->volume = 255;			/* Max volume */

	// Sensible defaults?
	for (uint8_t j=0; j<numvoices; j++) {
		synth_now->voice[j].ctrl     = SYNTH_VOICE_CTRL_ENABLE | SYNTH_VOICE_CTRL_SAWTOOTH;
		synth_now->voice[j].volume   = SYNTH_VOICE_VOLUME(255,255);
		synth_now->voice[j].duration = 46; // ~ 250 ms 
		synth_now->voice[j].attack   = 0xFFFF; // instantaneous
		synth_now->voice[j].decay    = 0xFFFF;
		synth_now->voice[j].phase_cmp=  (1<<9); // not relevant for sawtooth, only pulse 
		synth_now->voice[j].phase_inc = midi_table[60]; // middle C
	}
}


void synth_all_off(){
	synth_now->voice_force = 0;	
}


uint32_t time() {
        uint32_t cycles;
        asm volatile ("rdcycle %0" : "=r"(cycles));
        return cycles;
}

void synth_play(uint8_t voice, uint8_t note, uint16_t duration){
	synth_now->voice[voice].phase_inc = midi_table[note]; 
	// 0-127 : 60 is middle C
	synth_now->voice[voice].duration = duration; 
	// about 5.35 ms/tick
	synth_now->voice_start |= (1 << voice); 
	// plays as soon as it gets the start
}

void synth_play_queued(uint8_t voice, uint8_t note, uint16_t duration, uint32_t wait_until){
	synth_queue->cmd_wait = wait_until * 256; // factor to line up sample clocks and duration counts
	synth_queue->voice[voice].phase_inc = midi_table[note];
	synth_queue->voice[voice].duration = duration; 
	synth_queue->voice_start = (1 << voice);
}

int scale_major(uint8_t scale_degree){
	// Note that the 1st degree of the scale is the zero'th entry in the table.
	// You get what you deserve if you type in 0.
	uint8_t index = scale_degree - 1;
	uint8_t scale_table_major[] = {0, 2, 4, 5, 7, 9, 11};
	return (index/7)*12 + scale_table_major[index % 7];
}

