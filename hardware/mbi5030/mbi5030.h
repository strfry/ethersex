#pragma once

#include <stdint.h>

enum
{
	CHANNELS_PER_MBI = 16
};

enum // Cycles to switch latch on at
{
	MBI_LATCH_NONE = 16,
	
	MBI_LATCH_DATA = 15,
	MBI_LATCH_GLOBAL = 13,
	MBI_LATCH_READ_CONFIG = 11,
	MBI_LATCH_ENABLE_ERRDETECT = 9,
	MBI_LATCH_READ_ERRORSTATUS = 7,
	MBI_LATCH_WRITE_CONFIG = 5,
	MBI_LATCH_RESET_PWMCOUNTER = 3,
};

enum MBIPWMGrayCounterMode
{
	MBI_16BIT_PWM_COUNTER,
	MBI_12BIT_PWM_COUNTER,
};

enum MBIPWMCounterMode
{
	MBI_SPWM_MODE,
	MBI_STANDARD_PWM_MODE,
};

enum MBIPWMSyncMode
{
	MBI_AUTO_SYNC,
	MBI_MANUAL_SYNC,
};

enum MBIThermalProtectionMode
{
	MBI_ENABLE_THERMAL_PROTECTION,
	MBI_DISABLE_THERMAL_PROTECTION,
};

enum MBIGCLKTimeoutMode
{
	MBI_ENABLE_GCLK_TIMEOUT,
	MBI_DISABLE_GCLK_TIMEOUT,
};

void mbi_init();
//void mbi_config(MBIPWMGrayCounterMode pg = MBI_16BIT_PWM_COUNTER, MBIPWMCounterMode pc = MBI_SPWM_MODE, MBIPWMSyncMode = MBI_AUTO_SYNC,
//					uint8_t current_gain_adj = 0b10101011, MBIThermalProtectionMode tp = MBI_ENABLE_THERMAL_PROTECTION,
//					MBIGCLKTimeoutMode to = MBI_ENABLE_GCLK_TIMEOUT);
void mbi_gscale_data_out(uint16_t* data);
void mbi_word_out(uint8_t chain, uint16_t val, uint8_t latch_at) /*__attribute__((noinline))*/;
void mbi_propagate_to_all(uint16_t val, uint8_t latch_at);
