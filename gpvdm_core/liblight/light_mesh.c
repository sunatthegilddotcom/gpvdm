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

/** @file light_mesh.c
	@brief Performs meshing for the light model.
*/

#include "util.h"
#include "gpvdm_const.h"
#include "light.h"
#include "device.h"
#include "gpvdm_const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "lang.h"
#include "log.h"
#include "memory.h"
#include <light_fun.h>
#include <epitaxy.h>
#include <ray_fun.h>
#include <server.h>

static int unused __attribute__((unused));


void light_build_mesh(struct simulation *sim,struct light *li,struct dimensions *electrical_dim)
{
	int y=0;
	int x=0;
	int z=0;
	int l=0;

	long double pos=0.0;

	struct dim_light *dim=&(li->dim);

	printf_log(sim,"init: mesh\n");
	for (x=0;x<electrical_dim->xlen;x++)
	{
		dim->x[x]=electrical_dim->xmesh[x];
	}

	for (z=0;z<electrical_dim->zlen;z++)
	{
		dim->z[z]=electrical_dim->zmesh[z];
	}

	dim->dy=(epitaxy_get_optical_problem_stop(li->epi)-epitaxy_get_optical_problem_start(li->epi))/((long double)dim->ylen);

	//printf("%Le %Le\n",epitaxy_get_optical_problem_stop(li->epi),epitaxy_get_optical_problem_start(li->epi));
	//getchar();
	pos=epitaxy_get_optical_problem_start(li->epi);

	for (y=0;y<dim->ylen;y++)
	{
		dim->y[y]=pos;
		pos=pos+dim->dy;
		//printf("%Le %Le\n",pos,dim->dy);
		//getchar();
	}

	pos=li->lstart;
	for (l=0;l<dim->llen;l++)
	{
		dim->l[l]=pos;
		pos+=dim->dl;
	}

}

int light_build_obj_pointer_array_z(struct simulation *sim,struct device *dev,struct light *li,int z)
{
	int y;
	int x;

	struct vec v;

	struct dim_light *dim=&li->dim;

	for (x=0;x<dim->xlen;x++)
	{
		for (y=0;y<dim->ylen;y++)
		{
			v.z=dim->z[z];
			v.x=dim->x[x];
			v.y=dim->y[y];

			li->obj[z][x][y]=ray_obj_search_xyz(sim,dev,&v);

		}
	}

	return 0;
}

THREAD_FUNCTION thread_light_build_obj_pointer_array(void * in)
{
	int y;
	int x;
	//int nw;
	struct vec v;

	struct job *j=(struct job *)in;
	//struct worker *my_worker=j->w;
	//nw=my_worker->worker_n;

	struct simulation *sim=(struct simulation *)j->sim;
	struct light *li=(struct light *)j->data0;
	struct device *dev=(struct device *)j->data1;
	struct dim_light *dim=&li->dim;
	//struct shape *s;
	int z=j->data_int0;

	for (x=0;x<dim->xlen;x++)
	{
		for (y=0;y<dim->ylen;y++)
		{
			v.z=dim->z[z];
			v.x=dim->x[x];
			v.y=dim->y[y];

			li->obj[z][x][y]=ray_obj_search_xyz(sim,dev,&v);
		}
	}

	j->data0=NULL;
	j->data1=NULL;
	server2_job_finished(sim,j);

	return 0;
}

void light_build_obj_pointer_array(struct simulation *sim,struct light *li, struct device *dev)
{
	printf_log(sim,"Light: Building object pointer array\n");
	int z=0;
	struct job j;
	struct dim_light *dim=&(li->dim);

	//sim->server.worker_max=1;
	for (z=0;z<dim->zlen;z++)
	{
		job_init(sim,&j);
		sprintf(j.name,"build_light_mesh-%d",z);
		j.fun=&thread_light_build_obj_pointer_array;
		j.sim=(void *)sim;

		j.data0=(void *)li;
		j.data1=(void *)dev;
		j.data_int0=z;

		server2_add_job(sim,&(sim->server),&j);
		//printf("add\n");
	}
	server2_run_until_done(sim,&(sim->server));
	server2_free_finished_jobs(sim,&(sim->server));

}

void light_build_materials_arrays(struct simulation *sim,struct light *li, struct device *dev)
{
	int x=0;
	int y=0;
	int z=0;
	int l=0;
	//long double ypos=0.0;
	long double n=0.0;
	long double alpha=0.0;
	long double lam=0.0;
	struct object *obj;
	struct dim_light *dim=&(li->dim);
	struct shape *s;

	/*for (z=0;z<dim->zlen;z++)
	{
		for (x=0;x<dim->xlen;x++)
		{
			for (y=0;y<dim->ylen;y++)
			{
				obj=li->obj[z][x][y];
				printf("test %d %d %d %p\n",z,x,y,obj);
			}
		}
	}

	printf("here\n");*/
	light_build_obj_pointer_array(sim,li, dev);


	for (l=0;l<dim->llen;l++)
	{
		lam=dim->l[l];

		//printf("%d %d %d\n",dim->zlen,dim->xlen,dim->ylen);
		//getchar();
		for (z=0;z<dim->zlen;z++)
		{
			for (x=0;x<dim->xlen;x++)
			{
				for (y=0;y<dim->ylen;y++)
				{
					//v.z=dim->z[z];
					//v.x=dim->x[x];
					//v.y=dim->y[y];

					obj=li->obj[z][x][y];//ray_obj_search_xyz(sim,dev,&v);
					//printf("test2 %d %d %d %p\n",z,x,y,obj);
					//printf("%d %d %d %s %x\n",z,x,y,li->obj[z][x][y]->name,obj);
					s=obj->s;


					//getchar();

					if (strcmp(s->optical_material,"none")==0)
					{
						ewe(sim,"No optical material defined for object: %s",obj->name);
					}

					n=inter_get_noend(&(s->n),lam);
					alpha=inter_get_noend(&(s->alpha),lam);
					li->alpha[z][x][y][l]=alpha;
					li->alpha0[z][x][y][l]=alpha;
					li->n[z][x][y][l]=n;


				}
				//getchar();
			}
		}
		//
	}

	/*char fname[200];

	for (y=0;y<dim->ylen;y++)
	{
		sprintf(fname,"slice%d.dat",y);
		FILE* out;
		out=fopen(fname,"w");

		for (z=0;z<dim->zlen;z++)
		{
			for (x=0;x<dim->xlen;x++)
			{

				//v.z=dim->z[z];
				//v.x=dim->x[x];
				//v.y=dim->y[y];

				obj=li->obj[z][x][y];//ray_obj_search_xyz(sim,dev,&v);

				s=obj->s;

				fprintf(out,"%Le %Le %d\n",dim->z[z],dim->x[x],obj->uid);

			}

			fprintf(out,"\n");
			//getchar();
		}

		fclose(out);
	}*/

	//exit(0);

	if (li->flip_field==TRUE)
	{
		flip_light_zxyl_long_double_y(sim, dim,li->alpha);
		flip_light_zxyl_long_double_y(sim, dim,li->alpha0);
		flip_light_zxyl_long_double_y(sim, dim,li->n);
	}


	light_calculate_complex_n(li);
	light_load_filter(sim,li);

	for (l=0;l<dim->llen;l++)
	{
		//printf("%Le\n",dim->l[l]);
		//getchar();
		li->sun_norm[l]=inter_get_hard(&(li->sun_read),dim->l[l]);
		if (li->filter_enabled==TRUE)
		{
			li->filter[l]=inter_get_hard(&(li->filter_read),dim->l[l]);
		}else
		{
			li->filter[l]=1.0;
		}
	}


}
