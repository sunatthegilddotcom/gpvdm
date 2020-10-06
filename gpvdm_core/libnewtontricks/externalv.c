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

/** @file newton_externalv.c
	@brief Run the newton solver for an external voltage.
*/


#include <exp.h>
#include "dump.h"
#include "sim.h"
#include <newton_tricks.h>
#include <contacts.h>


//    ----=----
//    |       |-----
//    |       |    |
//    |       |   ---
//    |       SC
//    |       |   ---
//    |       |    |
//  Vtot      |-----
//    |       R
//    |	      |
//    ----=----



long double newton_externv(struct simulation *sim,struct device *in,long double Vtot)
{
	long double Vapplied_last=0.0;
	long double Vapplied=0.0;
	long double C=in->C;

	if (in->go_time==FALSE)
	{
		C=0.0;
	}
	contact_set_wanted_active_contact_voltage(sim,in,Vtot);
	Vapplied_last=contact_get_active_contact_voltage_last(sim,in);

		sim_externalv_ittr(sim,in,Vtot);

	Vapplied=contact_get_active_contact_voltage(sim,in);
	//printf("%Le %Le %Le\n",Vapplied,Vtot,Vapplied_last);
return get_I(in)+Vapplied/in->Rshunt+C*(Vapplied-Vapplied_last)/in->dt;
}


long double newton_externalv_simple(struct simulation *sim,struct device *in,gdouble V)
{

contact_set_active_contact_voltage(sim,in,V);
in->kl_in_newton=FALSE;
solver_realloc(sim,in);
solve_all(sim,in);
return get_I(in);
}





//////////////////////From misc file//////////////////////////////////////////

gdouble sim_externalv_ittr(struct simulation *sim,struct device *in,gdouble wantedv)
{
	//printf("Enter %Le\n",wantedv);
	gdouble Vapplied=0.0;
	gdouble clamp=0.1;
	gdouble step=0.001;
	gdouble e0;
	gdouble e1;
	gdouble i0;
	gdouble i1;
	gdouble deriv;
	gdouble Rs=in->Rcontact;

	in->kl_in_newton=FALSE;
	solver_realloc(sim,in);

	Vapplied=contact_get_active_contact_voltage(sim,in);
	//printf("ok %Le %Le\n",wantedv,Vapplied);

	solve_all(sim,in);

	i0=get_I(in);

	//printf("done %Le %Le\n",wantedv,Vapplied);


	gdouble itot=i0+Vapplied/in->Rshunt;

	e0=fabs(itot*Rs+Vapplied-wantedv);
	Vapplied+=step;
	contact_set_active_contact_voltage(sim,in,Vapplied);

	solve_all(sim,in);

	i1=get_I(in);
	itot=i1+Vapplied/in->Rshunt;

	e1=fabs(itot*Rs+Vapplied-wantedv);

	deriv=(e1-e0)/step;
	step= -e1/deriv;
	//step=step/(1.0+fabs(step/clamp));
	Vapplied+=step;
	contact_set_active_contact_voltage(sim,in,Vapplied);
	int count=0;
	int max=1000;
	do
	{
		e0=e1;
		solve_all(sim,in);
		itot=i1+Vapplied/in->Rshunt;
		e1=fabs(itot*Rs+Vapplied-wantedv);

		deriv=(e1-e0)/step;
		step= -e1/deriv;
		//gdouble clamp=0.01;
		//if (e1<clamp) clamp=e1/100.0;
		//step=step/(1.0+fabs(step/clamp));
		step=step/(1.0+fabs(step/clamp));
		Vapplied+=step;
		//printf("%Le %Le\n",Vapplied,e1);
		contact_set_active_contact_voltage(sim,in,Vapplied);
		if (count>max) break;
		count++;
	}while(e1>1e-8);

	gdouble ret=get_I(in)+Vapplied/in->Rshunt;
	//getchar();
return ret;
}

gdouble sim_externalv(struct simulation *sim,struct device *in,gdouble wantedv)
{

	sim_externalv_ittr(sim,in,wantedv);

return 0.0;
}
