/*
 * Adapted from constants.h from Wituz
 */

#include "audio.h"

void audioInit() {
	//SpuInitMalloc (SOUND_MALLOC_MAX, SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1));
    DsInit();
    SpuInit();
    static char spuMallocArea[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)];
    SpuInitMalloc (SOUND_MALLOC_MAX, spuMallocArea);
    l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);
    // Master volume should be in range 0x0000 - 0x3fff
    l_c_attr.mvol.left  = 0x3fff;
    l_c_attr.mvol.right = 0x3fff;
    // Cd volume should be in range 0x0000 - 0x7fff
    l_c_attr.cd.volume.left = 0x3fff;
    l_c_attr.cd.volume.right = 0x3fff;
    // Enable CD input ON
    l_c_attr.cd.mix = SPU_ON;

    SpuSetCommonAttr (&l_c_attr);
    //SpuSetIRQ(SPU_OFF);
    //SpuSetKey(SpuOff, SPU_ALLCH);
    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
}

void audioTransferVagToSPU(char* sound, int sound_size, int voice_channel) {
	SpuSetTransferMode (SpuTransByDMA);         // set transfer mode to DMA
	l_vag1_spu_addr = SpuMalloc(sound_size);    // allocate SPU memory for sound 1
	SpuSetTransferStartAddr(l_vag1_spu_addr);   // set transfer starting address to malloced area
	SpuWrite (sound + 0x30, sound_size);        // perform actual transfer
	SpuIsTransferCompleted (SPU_TRANSFER_WAIT); // wait for DMA to complete
    
    // mask which specific voice attributes are to be set
	g_s_attr.mask =
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

	g_s_attr.voice = (voice_channel);

    g_s_attr.volume.left  = 0x0;
    g_s_attr.volume.right = 0x0;

	g_s_attr.pitch        = 0x1000;
	g_s_attr.addr         = l_vag1_spu_addr;
	g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	g_s_attr.ar           = 0x0;
	g_s_attr.dr           = 0x0;
	g_s_attr.sr           = 0x0;
	g_s_attr.rr           = 0x0;
	g_s_attr.sl           = 0xf;

    SpuSetVoiceAttr (&g_s_attr);  // this is causing the sound to play
}

void audioPlay(int voice_channel, unsigned short volume, unsigned short pitch) {
    setVoiceVolumePitch(&g_s_attr, voice_channel, volume, pitch);
    SpuSetKey(SpuOn, voice_channel);
}

void audioChannelConfigure() {
	// mask which specific voice attributes are to be set
}

void audioFree(unsigned long sound_address) {
	SpuFree(sound_address);
}

void setVoiceVolumePitch(SpuVoiceAttr * voiceAttributes, int voice_channel, int volume, unsigned short pitch){
    voiceAttributes->mask= ( SPU_VOICE_VOLL | SPU_VOICE_VOLR );
    g_s_attr.pitch = pitch;
    SpuSetVoiceAttr (&g_s_attr);
    voiceAttributes->voice = voice_channel;
    voiceAttributes->volume.left =  voiceAttributes->volume.right  = volume;
    SpuSetVoiceAttr(voiceAttributes);  
}