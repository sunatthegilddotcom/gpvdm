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




/** @file math_2d_to_triangles_simple.c
	@brief Convert a math 2d structure to triangles in a simple way
*/
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sim_struct.h>
#include <memory.h>

#include "math_2d.h"
#include "util.h"
#include "cal_path.h"
#include "gpvdm_const.h"
#include <log.h>
#include <triangles.h>

void math_2d_to_triangles_simple(struct simulation *sim,struct triangles *tri,struct math_2d *in, int x_steps, int y_steps)
{
	int x=0;
	int y=0;
	int dx=0;
	int dy=0;
	int x_pos=0;
	int y_pos=0;

	int x_pos_next=0;
	int y_pos_next=0;

	double x0=0.0;
	double y0=0.0;
	double z0=0.0;

	double x1=0.0;
	double y1=0.0;
	double z1=0.0;

	double x2=0.0;
	double y2=0.0;
	double z2=0.0;

	double d00=0.0;
	double d10=0.0;
	double d01=0.0;
	double d11=0.0;

	//printf(">>>>>>>>%d %d\n",in->x_len,in->y_len);
	math_2d_save(sim,"yes.dat",in);
	//int x_remain=in->x_len%x_steps;
	//printf("rod: '%ld %ld %d'\n",in->x_len,x_remain,in->x_len-x_remain);
	dx=in->x_len/x_steps;
	dy=in->y_len/y_steps;
	x_steps=in->x_len/dx;
	y_steps=in->y_len/dy;

	//printf("%d %d %d\n",x_steps,dx,x_steps*dx);
	//printf("%d %d\n",y_steps,dy);
	triangles_malloc(tri);
	int uid=0;

	struct triangle temp_tri;
	triangle_init(&temp_tri);

	for (x=0;x<x_steps;x++)
	{
		for (y=0;y<y_steps;y++)
		{
			x_pos=x*dx;
			y_pos=y*dy;
			//printf_log(sim,"%d %d\n",x_pos,y_pos);

			x_pos_next=x_pos+dx;
			y_pos_next=y_pos+dy;

			if (y_pos_next>=in->y_len)
			{
				y_pos_next=in->y_len-1;
			}

			if (x_pos_next>=in->x_len)
			{
				x_pos_next=in->x_len-1;
			}

			if (x==0)
			{
				d00=0.0;
				d01=0.0;
			}else
			{
				d00=in->data[x_pos][y_pos];
				d01=in->data[x_pos][y_pos_next];
			}

			if (x==x_steps-1)
			{
				d10=0.0;
				d11=0.0;
			}else
			{
				d10=in->data[x_pos_next][y_pos];
				if (y==y_steps-1)
				{
					d11=0.0;
				}else
				{
					d11=in->data[x_pos_next][y_pos_next];
				}
			}

			if (y==0)
			{
				d00=0.0;
				d10=0.0;
			}

			if (y==y_steps-1)
			{
				d01=0.0;
				d11=0.0;
			}

			z0=x_pos;
			x0=y_pos;
			y0=d00;

			z1=x_pos;
			x1=y_pos+dy;
			y1=d01;

			z2=x_pos+dx;
			x2=y_pos+dy;
			y2=d11;

			vec_set(&(temp_tri.xy0),x0,y0,z0);
			vec_set(&(temp_tri.xy1),x1,y1,z1);
			vec_set(&(temp_tri.xy2),x2,y2,z2);


			temp_tri.object_uid=uid;
			temp_tri.object_type=0;

			triangles_add_triangle(tri, &(temp_tri));
			uid++;

			z0=x_pos+dx;
			x0=y_pos;
			y0=d10;

			z1=x_pos;
			x1=y_pos;
			y1=d00;

			z2=x_pos+dx;
			x2=y_pos+dy;
			y2=d11;

			vec_set(&(temp_tri.xy0),x0,y0,z0);
			vec_set(&(temp_tri.xy1),x1,y1,z1);
			vec_set(&(temp_tri.xy2),x2,y2,z2);

			temp_tri.object_uid=uid;
			temp_tri.object_type=0;

			triangles_add_triangle(tri, &(temp_tri));
			//triangles_add_triangle(tri, x0,y0,z0,x1,y1,z1,x2,y2,z2,uid,0);
			uid++;
		}

	}

	triangles_save(sim,"yes2.dat",tri);
	//Add btm
	z0=0.0;
	x0=0.0;
	y0=0.0;

	z1=0.0;
	x1=y_steps*dy;
	y1=0.0;

	z2=x_steps*dx;
	x2=y_steps*dy;
	y2=0.0;

	//triangles_add_triangle(tri, x0,y0,z0,x1,y1,z1,x2,y2,z2,uid,0);
	vec_set(&(temp_tri.xy0),x0,y0,z0);
	vec_set(&(temp_tri.xy1),x1,y1,z1);
	vec_set(&(temp_tri.xy2),x2,y2,z2);

	temp_tri.object_uid=uid;
	temp_tri.object_type=0;

	triangles_add_triangle(tri, &(temp_tri));
	uid++;

	z0=0.0;
	x0=0.0;
	y0=0.0;

	z1=x_steps*dx;
	x1=0.0;
	y1=0.0;

	z2=x_steps*dx;
	x2=y_steps*dy;
	y2=0.0;

	//triangles_add_triangle(tri, x0,y0,z0,x1,y1,z1,x2,y2,z2,uid,0);
	vec_set(&(temp_tri.xy0),x0,y0,z0);
	vec_set(&(temp_tri.xy1),x1,y1,z1);
	vec_set(&(temp_tri.xy2),x2,y2,z2);

	temp_tri.object_uid=uid;
	temp_tri.object_type=0;

	triangles_add_triangle(tri, &(temp_tri));
	uid++;
	//

	//in->y_len=y_len;
	//in->x_len=x_len;
	//malloc_2d((void ***)&(in->data), in->x_len, in->y_len, sizeof(long double));
	 triangles_cal_edges(tri);
}



