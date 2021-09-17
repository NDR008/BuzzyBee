/*
 * Adapted from constants.h from Wituz
 * and magic from Nicolas
 */

#include "audio.h"
#include "spu.h"

SpuCommonAttr l_c_attr;
SpuVoiceAttr  g_s_attr;
unsigned long l_vag1_spu_addr;

void audioInit() {
	SpuInit();
	//SpuInitMalloc (SOUND_MALLOC_MAX, SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1));
    static char spuMallocArea[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)];

    SpuInitMalloc(SOUND_MALLOC_MAX, spuMallocArea);
	l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
	l_c_attr.mvol.left  = 0x3fff; // set master left volume
	l_c_attr.mvol.right = 0x3fff; // set master right volume
	SpuSetCommonAttr (&l_c_attr);
    SpuSetIRQ(SPU_OFF);

    SpuSetVoiceAttr (&g_s_attr);
    
    // from nic
    DPCR |= 0x000b0000;
    SPU_VOL_MAIN_LEFT = 0x3800;
    SPU_VOL_MAIN_RIGHT = 0x3800;
    SPU_CTRL = 0;
    SPU_KEY_ON_LOW = 0;
    SPU_KEY_ON_HIGH = 0;
    SPU_KEY_OFF_LOW = 0xffff;
    SPU_KEY_OFF_HIGH = 0xffff;
    SPU_RAM_DTC = 4;
    SPU_VOL_CD_LEFT = 0;
    SPU_VOL_CD_RIGHT = 0;
    SPU_PITCH_MOD_LOW = 0;
    SPU_PITCH_MOD_HIGH = 0;
    SPU_NOISE_EN_LOW = 0;
    SPU_NOISE_EN_HIGH = 0;
    SPU_REVERB_EN_LOW = 0;
    SPU_REVERB_EN_HIGH = 0;
    SPU_VOL_EXT_LEFT = 0;
    SPU_VOL_EXT_RIGHT = 0;
    SPU_CTRL = 0x8000;
}

void audioTransferVagToSPU(char* sound, int sound_size, int voice_channel) {
	SpuSetTransferMode (SpuTransByDMA); // set transfer mode to DMA
	l_vag1_spu_addr = SpuMalloc(sound_size); // allocate SPU memory for sound 1
	SpuSetTransferStartAddr(l_vag1_spu_addr); // set transfer starting address to malloced area
	SpuWrite (sound + 0x30, sound_size); // perform actual transfer
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

	g_s_attr.volume.left  = 0x1fff;
	g_s_attr.volume.right = 0x1fff;

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

void audioSilence() {
    SPUInit();
    for (unsigned i = 0; i < 24; i++) {
        SPUResetVoice(i);
    }
}

static void SPUResetVoice(int voiceID) {
    SPU_VOICES[voiceID].volumeLeft = 0;
    SPU_VOICES[voiceID].volumeRight = 0;
    SPU_VOICES[voiceID].sampleRate = 0;
    SPU_VOICES[voiceID].sampleStartAddr = 0;
    SPU_VOICES[voiceID].ad = 0x000f;
    SPU_VOICES[voiceID].currentVolume = 0;
    SPU_VOICES[voiceID].sampleRepeatAddr = 0;
    SPU_VOICES[voiceID].sr = 0x0000;
}

static void SPUInit() {
    DPCR |= 0x000b0000;
    SPU_VOL_MAIN_LEFT = 0x3800;
    SPU_VOL_MAIN_RIGHT = 0x3800;
    SPU_CTRL = 0;
    SPU_KEY_ON_LOW = 0;
    SPU_KEY_ON_HIGH = 0;
    SPU_KEY_OFF_LOW = 0xffff;
    SPU_KEY_OFF_HIGH = 0xffff;
    SPU_RAM_DTC = 4;
    SPU_VOL_CD_LEFT = 0;
    SPU_VOL_CD_RIGHT = 0;
    SPU_PITCH_MOD_LOW = 0;
    SPU_PITCH_MOD_HIGH = 0;
    SPU_NOISE_EN_LOW = 0;
    SPU_NOISE_EN_HIGH = 0;
    SPU_REVERB_EN_LOW = 0;
    SPU_REVERB_EN_HIGH = 0;
    SPU_VOL_EXT_LEFT = 0;
    SPU_VOL_EXT_RIGHT = 0;
    SPU_CTRL = 0x8000;
}