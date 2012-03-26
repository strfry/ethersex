/*
 * Copyright (c) 2009 by David Gräff <david.graeff@web.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "core/debug.h"
#include "core/eeprom.h"

#include "protocols/ecmd/ecmd-base.h"

#ifdef REMOTEPOWER_SUPPORT

#include "remotepower.h"

int16_t parse_cmd_remotepower_on (char *cmd, char *output, uint16_t len)
{
	uint8_t value=0;
	uint16_t ret = 0; // must be 16 bit; because the answer length may be > 255
	if (cmd[0]!=0) ret = sscanf_P(cmd, PSTR("%u"),  &value);
	
/*	
	remotepower_on(1);
	remotepower_on(2);
	remotepower_on(3);
	remotepower_on(4);
*/	
	if (ret == 1)
	{
	    remotepower_on(value);
		return ECMD_FINAL_OK;
	}
	else
		return ECMD_ERR_PARSE_ERROR;
}


int16_t parse_cmd_remotepower_off (char *cmd, char *output, uint16_t len)
{
	uint8_t value=0;
	uint16_t ret = 0; // must be 16 bit; because the answer length may be > 255
	if (cmd[0]!=0) ret = sscanf_P(cmd, PSTR("%u"),  &value);
/*	
	remotepower_off(1);
	remotepower_off(2);
	remotepower_off(3);
	remotepower_off(4);
*/	
	if (ret == 1)
	{
	    remotepower_off(value);
		return ECMD_FINAL_OK;
	}
	else
		return ECMD_ERR_PARSE_ERROR;
}

#endif  /* REMOTEPOWER_SUPPORT */

/*
-- Ethersex META --
block([[RemotePower]] commands)
ecmd_feature(remotepower_on, "rpon", POS, urn Remote Power Outlet on)
ecmd_feature(remotepower_off, "rpoff", POS, Turn Remote Power Outlet off)
*/
