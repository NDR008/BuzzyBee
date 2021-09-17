#include <libspu.h>

#define SOUND_MALLOC_MAX 10

void audioInit();
void audioTransferVagToSPU(char* sound, int sound_size, int voice_channel);
void audioPlay(int voice_channel);

void audioChannelConfigure();

void audioFree(unsigned long sound_address);
void audioSilence();
static void SPUResetVoice(int voidID);

static void SPUInit();