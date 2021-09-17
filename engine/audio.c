/*
 * Adapted from constants.h from Wituz
 */

#include "audio.h"

SpuCommonAttr l_c_attr;
SpuVoiceAttr  g_s_attr;
unsigned long l_vag1_spu_addr;
static char spuMallocArea[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)];

void audioInit() {
	//SpuInitMalloc (SOUND_MALLOC_MAX, SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1));
    SpuInitMalloc(SOUND_MALLOC_MAX, spuMallocArea);
    l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);  // Mask which attributes to set
    l_c_attr.mvol.left  = 0x3fff;                           // Master volume left
    l_c_attr.mvol.right = 0x3fff;                           // see libref47.pdf, p.1058
    SpuSetCommonAttr(&l_c_attr);                            // set attributes
    SpuSetIRQ(SPU_OFF);


}

void audioTransferVagToSPU(char* sound, int sound_size, int voice_channel) {
	SpuSetTransferMode (SpuTransByDMA); // set transfer mode to DMA
	//l_vag1_spu_addr = SpuMalloc(sound_size); // allocate SPU memory for sound 1
	//SpuSetTransferStartAddr(l_vag1_spu_addr); // set transfer starting address to malloced area
	SpuWrite (sound + 0x30, sound_size); // perform actual transfer
	SpuIsTransferCompleted (SPU_TRANSFER_WAIT); // wait for DMA to complete
}

void setVoiceAttr(unsigned int pitch, long channel, unsigned long soundAddr ){
    g_s_attr.mask=                                   //~ Attributes (bit string, 1 bit per attribute)
    (
      SPU_VOICE_VOLL |
      SPU_VOICE_VOLR |
      SPU_VOICE_PITCH |
      SPU_VOICE_WDSA |
      SPU_VOICE_ADSR_AMODE |
      SPU_VOICE_ADSR_SMODE |
      SPU_VOICE_ADSR_RMODE |
      SPU_VOICE_ADSR_AR |
      SPU_VOICE_ADSR_DR |
      SPU_VOICE_ADSR_SR |
      SPU_VOICE_ADSR_RR |
      SPU_VOICE_ADSR_SL
    );
    g_s_attr.voice        = channel;                 //~ Voice (low 24 bits are a bit string, 1 bit per voice )
    g_s_attr.volume.left  = 0x1000;                  //~ Volume 
    g_s_attr.volume.right = 0x1000;                  //~ Volume
    g_s_attr.pitch        = pitch;                   //~ Interval (set pitch)
    g_s_attr.addr         = soundAddr;               //~ Waveform data start address
    g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;    //~ Attack rate mode  = Linear Increase - see libref47.pdf p.1091
    g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;    //~ Sustain rate mode = Linear Increase
    g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;    //~ Release rate mode = Linear Decrease
    g_s_attr.ar           = 0x0;                     //~ Attack rate
    g_s_attr.dr           = 0x0;                     //~ Decay rate
    g_s_attr.rr           = 0x0;                     //~ Release rate
    g_s_attr.sr           = 0x0;                     //~ Sustain rate
    g_s_attr.sl           = 0xf;                     //~ Sustain level
    SpuSetVoiceAttr(&g_s_attr);                      // set attributes
}

void audioPlay(int voice_channel) {
	SpuSetKey(SpuOn, voice_channel);
}

void audioChannelConfigure() {
	// mask which specific voice attributes are to be set
}

void audioFree(unsigned long sound_address) {
	SpuFree(sound_address);
}
