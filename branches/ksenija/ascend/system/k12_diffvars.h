/*	ASCEND modelling environment
	Copyright (C) 2007 Carnegie Mellon University

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*//** @defgroup system_diffvars System Differential variables
	Derivative chains for ASCEND solver systems. This structure holds a
	reorganised form of the solver lists as for example as follows:

	VARS:
 	  - y1
	  - y2 -> y2'
	  - y3
	  - y4 -> y4' -> y4''
	INDEPS:
      - t
      - x
    OBS:
	  - y2
	  - y4'

	This structure will allow efficient construction of integration systems
	without any need for the integration engine to 'talk to' the compiler.
	It's important that the integration engine need not talk to the compiler,
	as we want all this solver-side stuff to work with other 'problem providers'
 	one day off in the future.

	Note that this stuff is living here in the *solver* and not the *integrator*
	directory. This is because the integrator views the problem via a slv_system
	structure; in that sense the integrator deals with the ASCEND API through
	the solver API, although it interfaces back with the user through a different
	API that includes sample timesteps, different reporter mechanisms, etc.
*/

#ifndef ASC_DIFFVARS_H
#define ASC_DIFFVARS_H

#include <ascend/utilities/config.h>
#include "analyze.h"
#include "slv_client.h"
#include "system.h"

/**	@addtogroup system_diffvars
	@{
*/

/*------------------------------------------------------------------------------
  Stuff for retrieving differential and derivative variables from the system
*/

/**
	populate chains
*/
int k12_system_generate_diffvars(slv_system_t sys, struct problem_t *prob);

#endif