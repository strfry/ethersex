/*
 * Copyright (c) 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PUremotepowerSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "remotepower.h"

#define NUM_REMOTEPOWER_OUTLETS 4

static int remotepower_state[NUM_REMOTEPOWER_OUTLETS]; 
static int remotepower_sendrepeat[NUM_REMOTEPOWER_OUTLETS]; 

void remotepower_set(int pos, int state)
{
    if (pos >= NUM_REMOTEPOWER_OUTLETS) return;
    remotepower_state[pos] = state != 0;
    remotepower_sendrepeat[pos] = 5;
}

int remotepower_get(int pos)
{
    if (pos >= NUM_REMOTEPOWER_OUTLETS) return -1;
    return remotepower_state[pos];
}

static void reset_pins()
{
    /*
    DDR_CONFIG_OUT(REMOTEPOWER4_ON);
    DDR_CONFIG_OUT(REMOTEPOWER4_OFF);
    DDR_CONFIG_OUT(REMOTEPOWER3_ON);
    DDR_CONFIG_OUT(REMOTEPOWER3_OFF);
    DDR_CONFIG_OUT(REMOTEPOWER2_ON);
    DDR_CONFIG_OUT(REMOTEPOWER2_OFF);
    DDR_CONFIG_OUT(REMOTEPOWER1_ON);
    DDR_CONFIG_OUT(REMOTEPOWER1_OFF);
    */
    
    PIN_SET(REMOTEPOWER4_ON);
    PIN_SET(REMOTEPOWER4_OFF);
    PIN_SET(REMOTEPOWER3_ON);
    PIN_SET(REMOTEPOWER3_OFF);
    PIN_SET(REMOTEPOWER2_ON);
    PIN_SET(REMOTEPOWER2_OFF);
    PIN_SET(REMOTEPOWER1_ON);
    PIN_SET(REMOTEPOWER1_OFF);
}

void remotepower_off(int pos)
{
    switch (pos) {
        case 1:
            PIN_CLEAR(REMOTEPOWER1_OFF);
            break;
        case 2:
            PIN_CLEAR(REMOTEPOWER2_OFF);
            break;
        case 3:
            PIN_CLEAR(REMOTEPOWER3_OFF);
            break;
        case 4:
            PIN_CLEAR(REMOTEPOWER4_OFF);
            break;
    }
    
    _delay_ms(200);
    
    reset_pins();
    _delay_ms(200);
}

void remotepower_on(int pos)
{
    switch (pos) {
        case 1:
            PIN_CLEAR(REMOTEPOWER1_ON);
            break;
        case 2:
            PIN_CLEAR(REMOTEPOWER2_ON);
            break;
        case 3:
            PIN_CLEAR(REMOTEPOWER3_ON);
            break;
        case 4:
            PIN_CLEAR(REMOTEPOWER4_ON);
            break;
    }
    
    _delay_ms(200);
    
    reset_pins();
    _delay_ms(200);
}

void
remotepower_init (void)
{
    int i;
    for (i = 0; i < NUM_REMOTEPOWER_OUTLETS; i++) {
        remotepower_state[i] = 0;
        remotepower_sendrepeat[i] = 5;
    }

    reset_pins();
}

void
remotepower_main(void)
{
/*
    int i;
    for (i = 0; i < NUM_REMOTEPOWER_OUTLETS; i++) {
        if (remotepower_sendrepeat[i] > 0) {
            remotepower_sendrepeat[i]--;
            remotepower_setpin(i, remotepower_state[i]);
        } else {
            remotepower_clearpin(i);
        }
    }
    */
}


/*
  -- Ethersex META --
  header(services/remotepower/remotepower.h)
  init(remotepower_init)
  timer(100, remotepower_main)
*/
