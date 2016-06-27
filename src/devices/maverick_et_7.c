/* Maverick ET-7
 *
 * Copyright (C) 2016 Karl Gutwin
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "data.h"
#include "rtl_433.h"
#include "util.h"


static int maverick_et_7_callback(bitbuffer_t *bitbuffer) {
	bitrow_t *bb = bitbuffer->bb;
	data_t *data;
	char time_str[LOCAL_TIME_BUFLEN];
	local_time_str(0, time_str);
	int i, init_code, checksum, flag;
	int16_t raw_temp1, raw_temp2;
	float temp1, temp2;


	if (debug_output) {
	  fprintf(stdout, "bits_per_row[1] = %d\n", bitbuffer->bits_per_row[1]);
	  bitbuffer_print(bitbuffer);
	}
	for (i=1; i<10; i++){
		if (bitbuffer->bits_per_row[i] != 6*8){
			return 0;
		}
	}

	init_code = bb[1][0];
	raw_temp1 = (bb[1][1] << 4) + ((bb[1][2] & 0xf0) >> 4);
	if (raw_temp1 >= 0x800) raw_temp1 |= 0xf000;
	raw_temp2 = ((bb[1][2] & 0x0f) << 8) + bb[1][3];
	if (raw_temp2 >= 0x800) raw_temp2 |= 0xf000;
	if (debug_output) {
	  fprintf(stdout, "raw temp: %d %d\n", raw_temp1, raw_temp2);
	}
	temp1 = raw_temp1 / 10.0;
	temp2 = raw_temp2 / 10.0;
	flag = bb[1][4];
	checksum = bb[1][5];

	data = data_make("time", "", DATA_STRING, time_str,
			 "model", "", DATA_STRING, "Maverick ET-7",
			 "id", "Id", DATA_FORMAT, "\t %d", DATA_INT, init_code,
			 "temperature_C_1", "Temperature", DATA_FORMAT, "%.1f C", DATA_DOUBLE, temp1,
			 "temperature_C_2", "Temperature", DATA_FORMAT, "%.1f C", DATA_DOUBLE, temp2,
			 NULL);
	data_acquired_handler(data);
	
	return 1; 
}

static char *output_fields[] = {
	"time",
	"model",
	"id",
	"temperature_C",
	"temperature_2_C",
	NULL
};

r_device maverick_et_7 = {

  .name          = "Maverick ET-7",
  .modulation    = OOK_PULSE_PPM_RAW,
  .short_limit   = 380*4,
  .long_limit    = 510*4,
  .reset_limit   = 1100*4,
  .json_callback = &maverick_et_7_callback,
  .disabled      = 0,
  .demod_arg     = 0,
  .fields        = output_fields,
};

