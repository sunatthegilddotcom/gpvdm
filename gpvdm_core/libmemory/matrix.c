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

/** @file matrix.c
@brief A struct for the matrix solver
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
#include "md5.h"
#include "cal_path.h"
#include <timer.h>

void matrix_init(struct matrix *mx)
{
	mx->Ti = NULL;
	mx->Tj = NULL;
	mx->Tx = NULL;
	mx->Txz = NULL;
	mx->b = NULL;
	mx->bz = NULL;
	mx->Tdebug = NULL;
	mx->nz= 0;
	mx->nz_max= 0;
	
	mx->M= 0;

	mx->use_cache=FALSE;
	strcpy(mx->hash,"");
	mx->ittr=0;
	mx->complex_matrix=FALSE;

	mx->build_from_non_sparse=FALSE;
	mx->J=NULL;

	//stats
	mx->tot_time=0;
}

void matrix_stats(struct simulation *sim,struct matrix *mx)
{
	printf("matrix solver time=%d ms \n",mx->tot_time);
	mx->tot_time=0;
}

void matrix_cache_reset(struct simulation *sim,struct matrix *mx)
{
	strcpy(mx->hash,"");
	mx->ittr=0;
}

void matrix_dump(struct simulation *sim,struct matrix *mx)
{
int i;
	printf("J:\n");
	for (i=0;i<mx->nz;i++)
	{
		printf_log(sim,"%ld %ld %Le\n",mx->Tj[i],mx->Ti[i],mx->Tx[i]);
	}

	printf("b:");
	
	for (i=0;i<mx->M;i++)
	{
		printf_log(sim,"%Le\n",mx->b[i]);
	}

}

void matrix_dump_b(struct simulation *sim,struct matrix *mx)
{
int i;
	if (mx->complex_matrix==TRUE)
	{
		for (i=0;i<mx->M;i++)
		{
			printf_log(sim,"%Le %Le\n",mx->b[i],mx->bz[i]);
		}
	}else
	{
		for (i=0;i<mx->M;i++)
		{
			printf_log(sim,"%Le\n",mx->b[i]);
		}
	}
}

void matrix_dump_J(struct simulation *sim,struct matrix *mx)
{
int i;
	if (mx->complex_matrix==TRUE)
	{
		for (i=0;i<mx->nz;i++)
		{
			printf_log(sim,"%ld %ld %Le %Le\n",mx->Ti[i],mx->Tj[i],mx->Tx[i],mx->Txz[i]);
		}
	}else
	{

		for (i=0;i<mx->nz;i++)
		{
			printf_log(sim,"%ld %ld %Le\n",mx->Ti[i],mx->Tj[i],mx->Tx[i]);
		}
	}
}


long double matrix_cal_error(struct simulation *sim,struct matrix *mx)
{
int i;
long double sum=0.0;
	for (i=0;i<mx->M;i++)
	{
		sum+=fabsl(mx->b[i]);
	}

return sum;
}

int matrix_solve(struct simulation *sim,struct matrix_solver_memory *msm,struct matrix *mx)
{
char out[100];
struct md5 hash;
int start_time=timer_get_time_in_ms();

	if (mx->use_cache==TRUE)
	{
		if (mx->ittr==0)
		{
			md5_init(&hash);
			md5_update(&hash,(char*)mx->Ti,mx->nz*sizeof(int));
			md5_update(&hash,(char*)mx->Tj,mx->nz*sizeof(int));
			md5_update(&hash,(char*)mx->Tx,mx->nz*sizeof(long double));
			md5_update(&hash,(char*)mx->b,mx->M*sizeof(long double));
			md5_to_str(mx->hash,&hash);

			join_path(2,mx->cache_file_path,get_cache_path(sim),mx->hash);

			if (isfile(mx->cache_file_path)==0)
			{
				return 0;
			}
		}

	}

	if (mx->complex_matrix==FALSE)
	{
		/*printf("here %d %d\n",mx->M,mx->nz);
		int i;

		for (i=0;i<mx->nz;i++)
		{
			printf("%d %Le\n",i,mx->b[i]);
		}

		for (i=0;i<mx->M;i++)
		{
			printf("%d %Le\n",i,mx->b[i]);
		}

		getchar();*/
		if (sim->dll_matrix_solve==NULL)
		{
			ewe(sim,"The matrix solver dll is not loaded\n");
		} 
		(*sim->dll_matrix_solve)(msm,mx->M,mx->nz,mx->Ti,mx->Tj,mx->Tx,mx->b);
		//printf("fin\n");
	}else
	{
		(*sim->dll_complex_matrix_solve)(msm,mx->M,mx->nz,mx->Ti,mx->Tj,mx->Tx,mx->Txz,mx->b,mx->bz);
	}

	mx->tot_time+=(timer_get_time_in_ms()-start_time);
return -1;
}

void matrix_malloc(struct simulation *sim,struct matrix *mx)
{
	malloc_1d((void **)(&mx->Ti), mx->nz, sizeof(int));

	malloc_1d((void **)(&mx->Tj), mx->nz, sizeof(int));

	malloc_1d((void **)(&mx->Tx), mx->nz, sizeof(long double));

	malloc_1d((void **)(&mx->b), mx->M, sizeof(long double));


	if (mx->complex_matrix==TRUE)
	{
		malloc_1d((void **)(&mx->Txz), mx->nz, sizeof(long double));

		malloc_1d((void **)(&mx->bz), mx->M, sizeof(long double));
	}

	if (mx->build_from_non_sparse==TRUE)
	{
		malloc_2d((void ***)(&mx->J), mx->M, mx->M, sizeof(long double));
	}

}

void matrix_convert_J_to_sparse(struct simulation *sim,struct matrix *mx)
{
	int x;
	int y;
	for (y=0;y<mx->M;y++)
	{
			for (x=0;x<mx->M;x++)
			{
				if (mx->J[y][x]!=0.0)
				{
					if (mx->nz>=mx->nz_max)
					{
						mx->nz_max+=1000;
						mx->Ti=realloc(mx->Ti,mx->nz_max*sizeof(int));
						mx->Tj=realloc(mx->Tj,mx->nz_max*sizeof(int));
						mx->Tx=realloc(mx->Tx,mx->nz_max*sizeof(long double));
					}

					mx->Ti[mx->nz]=y;
					mx->Tj[mx->nz]=x;
					mx->Tx[mx->nz]=mx->J[y][x];
					mx->nz++;
				}
			}
	}


}

void matrix_add_nz_item(struct simulation *sim,struct matrix *mx,int x,int y,long double val)
{
	int i;
	//(printf("search =%d %d\n",x,y);
	mx->J[y][x]+=val;
	/*
	for (i=0;i<mx->nz;i++)
	{
		if ((x==mx->Tj[i])&&(y==mx->Ti[i]))
		{
			//printf("found\n");
			mx->Tx[i]+=val;
			return;
		}
	}

	if (mx->nz>=mx->nz_max)
	{
		mx->nz_max+=1000;
		mx->Ti=realloc(mx->Ti,mx->nz_max*sizeof(int));
		mx->Tj=realloc(mx->Tj,mx->nz_max*sizeof(int));
		mx->Tx=realloc(mx->Tx,mx->nz_max*sizeof(long double));
	}

	mx->Ti[mx->nz]=y;
	mx->Tj[mx->nz]=x;
	mx->Tx[mx->nz]=val;
	mx->nz++;*/

}
void matrix_realloc(struct simulation *sim,struct matrix *mx)
{
	gdouble *dtemp=NULL;
	int *itemp=NULL;
	itemp = realloc(mx->Ti,mx->nz*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"mx->Ti - memory error\n");
	}else
	{
		mx->Ti=itemp;
		memset(mx->Ti, 0, mx->nz*sizeof(int));
	}

	itemp = realloc(mx->Tj,mx->nz*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"mx->Tj - memory error\n");
	}else
	{
		mx->Tj=itemp;
		memset(mx->Tj, 0, mx->nz*sizeof(int));
	}

	dtemp = realloc(mx->Tx,mx->nz*sizeof(long double));
	if (dtemp==NULL)
	{
		ewe(sim,"mx->Tx - memory error\n");
	}else
	{
		mx->Tx=dtemp;
		memset(mx->Tx, 0, mx->nz*sizeof(long double));
	}

	dtemp = realloc(mx->b,mx->M*sizeof(long double));

	if (dtemp==NULL)
	{
		ewe(sim,"in->b - memory error\n");
	}else
	{
		mx->b=dtemp;
		memset(mx->b, 0, mx->M*sizeof(long double));
	}

	if (mx->complex_matrix==TRUE)
	{
		dtemp = realloc(mx->Txz,mx->nz*sizeof(long double));
		if (dtemp==NULL)
		{
			ewe(sim,"mx->Txz - memory error\n");
		}else
		{
			mx->Txz=dtemp;
			memset(mx->Txz, 0, mx->nz*sizeof(long double));
		}

		dtemp = realloc(mx->bz,mx->M*sizeof(long double));

		if (dtemp==NULL)
		{
			ewe(sim,"in->bz - memory error\n");
		}else
		{
			mx->bz=dtemp;
			memset(mx->bz, 0, mx->M*sizeof(long double));
		}

	}

	if (mx->build_from_non_sparse==TRUE)
	{
		int y;
		for (y=0;y<mx->M;y++)
		{
			free(mx->J[y]);
		}

		free(mx->J);

		mx->J=malloc(mx->M*sizeof(long double*));
		for (y=0;y<mx->M;y++)
		{
			mx->J[y]=malloc(mx->M*sizeof(long double));
			memset(mx->J[y], 0, mx->M*sizeof(long double));
		}
	}
}

void matrix_zero_b(struct simulation *sim,struct matrix *mx)
{
	memset(mx->b, 0, mx->M*sizeof(long double));

	if (mx->complex_matrix==TRUE)
	{
		memset(mx->bz, 0, mx->M*sizeof(long double));
	}

	if (mx->build_from_non_sparse==TRUE)
	{
		int y;

		for (y=0;y<mx->M;y++)
		{
			memset(mx->J[y], 0, mx->M*sizeof(long double));
		}
	}
}

void matrix_save(struct simulation *sim,struct matrix *mx)
{
	char cache_file[PATH_MAX];
	FILE *file;

	join_path(2, cache_file,get_cache_path(sim),mx->hash);

	file=fopen(cache_file,"wb");

	fwrite ((char*)mx->Ti,mx->nz*sizeof(int),1,file);
	fwrite ((char*)mx->Tj,mx->nz*sizeof(int),1,file);
	fwrite ((char*)mx->Tx,mx->nz*sizeof(long double),1,file);
	fwrite ((char*)mx->b,mx->M*sizeof(long double),1,file);

	if (mx->complex_matrix==TRUE)
	{
		fwrite ((char*)mx->Txz,mx->nz*sizeof(long double),1,file);
		fwrite ((char*)mx->bz,mx->M*sizeof(long double),1,file);
	}

	fclose(file);

}

int matrix_load(struct simulation *sim,struct matrix *mx)
{
	char cache_file[PATH_MAX];
	FILE *file;

	join_path(2, cache_file,get_cache_path(sim),mx->hash);

	file=fopen(cache_file,"rb");
	if (file==NULL)
	{
		return -1;
	}

	fread((int*)mx->Ti, mx->nz*sizeof(int), 1, file);
	fread((int*)mx->Tj, mx->nz*sizeof(int), 1, file);
	fread((long double *)mx->Tx, mx->nz*sizeof(long double), 1, file);
	fread((long double *)mx->b, mx->M*sizeof(long double), 1, file);

	if (mx->complex_matrix==TRUE)
	{
		fread((long double *)mx->Txz, mx->nz*sizeof(long double), 1, file);
		fread((long double *)mx->bz, mx->M*sizeof(long double), 1, file);
	}
	fclose(file);

	return 0;

}

