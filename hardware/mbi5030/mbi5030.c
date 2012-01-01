#include "config.h"
#include "mbi5030.h"
#include <avr/io.h>
#include <util/delay.h>

#define REAL_MBI_COUNT ((uint8_t)MBI5030_CHIP_COUNT)
#define MBI_COUNT REAL_MBI_COUNT
#define CHANNELS_CONNECTED_PER_MBI 16

#define MBI_COUNT_CHAIN0 2
#define MBI_COUNT_CHAIN1 3


#define PIN_SET_CHAIN(chain, pin) (chain == 0 ? PIN_SET(pin ## 0) : PIN_SET(pin ## 1))
#define PIN_CLEAR_CHAIN(chain, pin) (chain == 0 ? PIN_CLEAR(pin ## 0) : PIN_CLEAR(pin ## 1))
//#define PIN_SET_CHAIN(chain, pin) (chain == 0 ? PIN_SET(pin ## 0) : PIN_SET(pin ## 1))

#define PIN_SET_CLK(chain) if (chain == 0) PIN_SET(MBI5030_CLK0); else PIN_SET(MBI5030_CLK1);
#define PIN_SET_LE(chain) if (chain == 0) PIN_SET(MBI5030_LE0); else PIN_SET(MBI5030_LE1);
#define PIN_SET_SDO(chain) if (chain == 0) PIN_SET(MBI5030_SDO0); else PIN_SET(MBI5030_SDO1);

#define PIN_CLEAR_CLK(chain) if (chain == 0) PIN_CLEAR(MBI5030_CLK0); else PIN_CLEAR(MBI5030_CLK1);
#define PIN_CLEAR_LE(chain) if (chain == 0) PIN_CLEAR(MBI5030_LE0); else PIN_CLEAR(MBI5030_LE1);
#define PIN_CLEAR_SDO(chain) if (chain == 0) PIN_CLEAR(MBI5030_SDO0); else PIN_CLEAR(MBI5030_SDO1);

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

void mbi_word_out(uint8_t chain, uint16_t val, uint8_t latch_at)
{
    for (uint8_t i = 0; i < 16; ++i)
    {	
        PIN_CLEAR_CLK(chain);
        if (val & (1 << (15 - i))) {
            PIN_SET_SDO(chain);
        } else {
            PIN_CLEAR_SDO(chain);
        }
            
        if (i == latch_at) {
            PIN_SET_LE(chain);
        }

        PIN_SET_CLK(chain);
    }
    PIN_CLEAR_LE(chain);
}

// Latch only on last
void mbi_propagate_to_all(uint16_t val, uint8_t latch_at)
{
	for (uint8_t i = 0; i < (MBI_COUNT_CHAIN0 - 1); ++i)
		mbi_word_out(0, val, MBI_LATCH_NONE);
	mbi_word_out(0, val, latch_at);
	
	for (uint8_t i = 0; i < (MBI_COUNT_CHAIN1 - 1); ++i)
		mbi_word_out(1, val, MBI_LATCH_NONE);
	mbi_word_out(1, val, latch_at);
}

void mbi_gscale_data_out_chain(uint8_t chain, uint8_t mbi_count, uint16_t* data)
{
    for (int8_t c = CHANNELS_PER_MBI - 1; c >= 0; --c)
	{
		for (int8_t m = mbi_count - 1; m >= 0; --m)
		{
			uint16_t val = 0;
			if (m < mbi_count && c != (CHANNELS_PER_MBI - 1))
				val = data[m * CHANNELS_CONNECTED_PER_MBI + c];
			mbi_word_out(chain, val, (m == 0) ? MBI_LATCH_DATA : MBI_LATCH_NONE);
			//mbi_word_out(val, 15);
		}
	}
	mbi_word_out(chain, 0, MBI_LATCH_GLOBAL);
}

void mbi_gscale_data_out(uint16_t* data)
{
    mbi_gscale_data_out_chain(0, MBI_COUNT_CHAIN0, data);
    
    data += CHANNELS_PER_MBI * MBI_COUNT_CHAIN0;
    
    mbi_gscale_data_out_chain(1, MBI_COUNT_CHAIN1, data);
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

    PIN_CLEAR(MBI5030_CLK0);
    PIN_CLEAR(MBI5030_SDO0);
    PIN_CLEAR(MBI5030_LE0);
    
    PIN_CLEAR(MBI5030_CLK1);
    PIN_CLEAR(MBI5030_SDO1);
    PIN_CLEAR(MBI5030_LE1);
    
    mbi_config(MBI_16BIT_PWM_COUNTER, MBI_SPWM_MODE, MBI_AUTO_SYNC,
        0b10101011, MBI_ENABLE_THERMAL_PROTECTION, MBI_ENABLE_GCLK_TIMEOUT);
}
