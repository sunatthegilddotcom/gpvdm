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

/** @file zxy_int.c
@brief memory functions for 3D arrays
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lang.h>
#include "sim.h"
#include "dump.h"
#include "mesh.h"
#include <math.h>
#include "log.h"
#include <solver_interface.h>
#include "memory.h"



void malloc_zxy_int(struct dimensions *dim, int * (***var))
{
	malloc_3d_int(dim->zlen, dim->xlen, dim->ylen, var);
}

void free_zxy_int(struct dimensions *dim, int * (***var))
{
	free_3d_int(dim->zlen, dim->xlen, dim->ylen, var);
}

void cpy_zxy_int(struct dimensions *dim, int * (***out),int * (***in))
{
	free_3d_int(dim->zlen, dim->xlen, dim->ylen, out);
	malloc_3d_int(dim->zlen, dim->xlen, dim->ylen, out);
	cpy_3d_int(dim->zlen, dim->xlen, dim->ylen, out, in);
}


void memory_flip_1d_int(int *var,int len)
{
	int x=0;
	int y=0;
	int z=0;
	int * data=malloc(sizeof(int)*len);
	for (y=0;y<len;y++)
	{
		data[y]=var[len-1-y];
	}

	for (y=0;y<len;y++)
	{
		var[y]=data[y];
	}

	free(data);

}

void dump_zxy_int(struct dimensions *dim, int ***var)
{
	int x=0;
	int y=0;
	int z=0;
	for (z = 0; z < dim->zlen; z++)
	{
		printf("z=%d:\n",z);

		for (y = 0; y < dim->ylen; y++)
		{

			for (x = 0; x < dim->xlen; x++)
			{
				printf("%d",var[z][x][y]);
			}
			printf("\n");
		}
	}
}
