//
// General-purpose Photovoltaic Device Model gpvdm.com - a drift diffusion
// base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// The model can simulate OLEDs, Perovskite cells, and OFETs.
// 
// Copyright (C) 2008-2020 Roderick C. I. MacKenzie
// 
// https://www.gpvdm.com
// r.c.i.mackenzie at googlemail.com
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the GPVDM nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Roderick C. I. MacKenzie BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

/** @file buffer.c
@brief used to save output files to disk with a nice header, so the user knows what was writtne to them
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "dat_file.h"
#include "gpvdm_const.h"
#include "code_ctrl.h"
#include "cal_path.h"
#include "dump.h"
#include <log.h>
#include <cache.h>
//<strip>
#include <lock.h>
//</strip>

#include <triangle.h>
#include <triangles.h>


void dat_file_load_info(struct simulation *sim,struct dat_file *in,char *file_name)
{
char line[10000];
char *token;
char temp2[200];
int i;
FILE *file;
file=fopen(file_name,"r");

if (file == NULL)
{
	ewe(sim,"Can not load the file %s\n",file_name);
}

do
{
	memset(line,0,10000);
	fgets(line, 10000, file);

	if (line[0]=='#')
	{
		if (strcmp_begin(line,"#x ")==0)
		{
			sscanf(line,"%s %d",temp2,&in->x);
		}else
		if (strcmp_begin(line,"#y ")==0)
		{
			sscanf(line,"%s %d",temp2,&in->y);
		}else
		if (strcmp_begin(line,"#z ")==0)
		{
			sscanf(line,"%s %d",temp2,&in->z);
		}else
		if (strcmp_begin(line,"#type ")==0)
		{
			sscanf(line,"%s %s",temp2,in->type);
		}


	}else
	{
		break;
	}


}while(!feof(file));
fclose(file);

}

