
#pragma once

#include <libspu.h>
#include <sys/types.h>
#include <libds.h>

#define SOUND_MALLOC_MAX 10

SpuCommonAttr l_c_attr;
SpuVoiceAttr  g_s_attr;
unsigned long l_vag1_spu_addr;

void audioInit();
void audioTransferVagToSPU(char* sound, int sound_size, int voice_channel);
void audioPlay(int voice_channel, unsigned short volume, unsigned short pitch);

void audioChannelConfigure();

void audioFree(unsigned long sound_address);
void setVoiceVolumePitch(SpuVoiceAttr * voiceAttributes, int voice_channel, int volume, unsigned short pitch);