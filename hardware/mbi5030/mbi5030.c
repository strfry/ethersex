#include "config.h"
#include "mbi5030.h"
#include <avr/io.h>
#include <util/delay.h>

#define REAL_MBI_COUNT ((uint8_t)MBI5030_CHIP_COUNT)
#define MBI_COUNT REAL_MBI_COUNT
#define CHANNELS_CONNECTED_PER_MBI 16

enum
{
	MBICFG_GCLK_TIMEOUT = 0,
	MBICFG_THERMAL_PROTECTION,
	MBICFG_CURRENT_GAIN,
	MBICFG_PWM_SYNC = 0xA,
	MBICFG_PWM_CTR_RESET,
	MBICFG_PWM_COUNTING,
	MBICFG_PWM_GSCALE_COUNTING,
	MBICFG_THERMAL_ERROR_FLAG,
	MBICFG_DATA_LOADING,
};

void mbi_word_out(uint16_t val, uint8_t latch_at)
{
	for (uint8_t i = 0; i < 16; ++i)
	{	
//	    _delay_us(1);
        PIN_CLEAR(MBI5030_CLK);
		
		if (val & (1 << (15 - i))) {
            PIN_SET(MBI5030_SDO);
		} else {
            PIN_CLEAR(MBI5030_SDO);
	    }
			
		if (i == latch_at) {
			PIN_SET(MBI5030_LE);
	    }

		PIN_SET(MBI5030_CLK);
	}
	PIN_CLEAR(MBI5030_LE);
}

// Latch only on last
void mbi_propagate_to_all(uint16_t val, uint8_t latch_at)
{
	for (uint8_t i = 0; i < (REAL_MBI_COUNT - 1); ++i)
		mbi_word_out(val, MBI_LATCH_NONE);
	mbi_word_out(val, latch_at);
}

void mbi_gscale_data_out(uint16_t* data)
{
	for (int8_t c = CHANNELS_PER_MBI - 1; c >= 0; --c)
	{
		for (int8_t m = REAL_MBI_COUNT - 1; m >= 0; --m)
		{
			uint16_t val = 0;
			if (m < MBI_COUNT && c != (CHANNELS_PER_MBI - 1))
				val = data[m * CHANNELS_CONNECTED_PER_MBI + c];
			mbi_word_out(val, (m == 0) ? MBI_LATCH_DATA : MBI_LATCH_NONE);
			//mbi_word_out(val, 15);
		}
	}
	mbi_word_out(0, MBI_LATCH_GLOBAL);
}

void mbi_config(enum MBIPWMGrayCounterMode pg, enum MBIPWMCounterMode pc, enum MBIPWMSyncMode ps, uint8_t current_gain_adj, enum MBIThermalProtectionMode tp, enum MBIGCLKTimeoutMode to)
{
	uint16_t confval = (uint16_t) (current_gain_adj) << MBICFG_CURRENT_GAIN;
	confval |= _BV(MBICFG_DATA_LOADING); // 16 times data latch + 1 global latch
	if (pg == MBI_12BIT_PWM_COUNTER)
		confval |= _BV(MBICFG_PWM_GSCALE_COUNTING);
	if (pc == MBI_STANDARD_PWM_MODE) {
		confval |= _BV(MBICFG_PWM_COUNTING);
		//confval |= _BV(MBICFG_PWM_CTR_RESET);
	}
	if (ps == MBI_MANUAL_SYNC)
		confval |= _BV(MBICFG_PWM_SYNC);
	if (tp == MBI_ENABLE_THERMAL_PROTECTION)
		confval |= _BV(MBICFG_THERMAL_PROTECTION);
	if (to == MBI_ENABLE_GCLK_TIMEOUT)
		confval |= _BV(MBICFG_GCLK_TIMEOUT);
		
	mbi_propagate_to_all(confval, MBI_LATCH_WRITE_CONFIG);	
}

void mbi_init()
{
    PIN_CLEAR(MBI5030_CLK);
    PIN_CLEAR(MBI5030_SDO);
    PIN_CLEAR(MBI5030_LE);
    
    DDR_CONFIG_OUT(MBI5030_CLK);
    DDR_CONFIG_OUT(MBI5030_SDO);
    DDR_CONFIG_OUT(MBI5030_LE);
    
    mbi_config(MBI_16BIT_PWM_COUNTER, MBI_STANDARD_PWM_MODE, MBI_AUTO_SYNC,
        0b10101011, MBI_ENABLE_THERMAL_PROTECTION, MBI_DISABLE_GCLK_TIMEOUT);
}
