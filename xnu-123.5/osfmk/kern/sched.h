/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	sched.h
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1985
 *
 *	Header file for scheduler.
 *
 */

#ifndef	_KERN_SCHED_H_
#define _KERN_SCHED_H_

#include <cpus.h>
#include <simple_clock.h>
#include <stat_time.h>

#include <mach/policy.h>
#include <kern/kern_types.h>
#include <kern/queue.h>
#include <kern/lock.h>
#include <kern/macro_help.h>

#if	STAT_TIME

/*
 *	Statistical timing uses microseconds as timer units.  16 bit shift
 *	yields priorities.  PRI_SHIFT_2 isn't needed.
 */
#define PRI_SHIFT	(16 - SCHED_TICK_SHIFT)

#else	/* STAT_TIME */

/*
 *	Otherwise machine provides shift(s) based on time units it uses.
 */
#include <machine/sched_param.h>

#endif	/* STAT_TIME */

#define	NRQS		128				/* 128 run queues per cpu */
#define NRQBM		(NRQS / 32)		/* number of run queue bit maps */

#define MAXPRI		(NRQS-1)
#define MINPRI		IDLEPRI			/* lowest legal priority schedulable */
#define	IDLEPRI		0				/* idle thread priority */
#define DEPRESSPRI	MINPRI			/* depress priority */

/*
 *	High-level priority assignments
 *
 *************************************************************************
 * 127		Reserved (real-time)
 *				A
 *				+
 *			(32 levels)
 *				+
 *				V
 * 96		Reserved (real-time)
 * 95		Kernel mode only
 *				A
 *				+
 *			(16 levels)
 *				+
 *				V
 * 80		Kernel mode only
 * 79		High priority
 *				A
 *				+
 *			(16 levels)
 *				+
 *				V
 * 64		High priority
 * 63		Elevated priorities
 *				A
 *				+
 *			(12 levels)
 *				+
 *				V
 * 52		Elevated priorities
 * 51		Elevated priorities (incl. BSD +nice)
 *				A
 *				+
 *			(20 levels)
 *				+
 *				V
 * 32		Elevated priorities (incl. BSD +nice)
 * 31		Default (default base for threads)
 * 30		Lowered priorities (incl. BSD -nice)
 *				A
 *				+
 *			(20 levels)
 *				+
 *				V
 * 11		Lowered priorities (incl. BSD -nice)
 * 10		Lowered priorities (aged pri's)
 *				A
 *				+
 *			(11 levels)
 *				+
 *				V
 * 0		Lowered priorities (aged pri's / idle)
 *************************************************************************
 */

#define BASEPRI_REALTIME	(MAXPRI - (NRQS / 4) + 1)			/* 96 */

#define MAXPRI_STANDARD		(BASEPRI_REALTIME - 1)				/* 95 */

#define MAXPRI_KERNBAND		MAXPRI_STANDARD						/* 95 */
#define MINPRI_KERNBAND		(MAXPRI_KERNBAND - (NRQS / 8) + 1)	/* 80 */

#define MAXPRI_HIGHBAND		(MINPRI_KERNBAND - 1)				/* 79 */
#define MINPRI_HIGHBAND		(MAXPRI_HIGHBAND - (NRQS / 8) + 1)	/* 64 */

#define MAXPRI_MAINBAND		(MINPRI_HIGHBAND - 1)				/* 63 */
#define BASEPRI_DEFAULT		(MAXPRI_MAINBAND - (NRQS / 4))		/* 31 */
#define MINPRI_MAINBAND		MINPRI								/*  0 */

#define MINPRI_STANDARD		MINPRI_MAINBAND						/*  0 */

/*
 *	Macro to check for invalid priorities.
 */
#define invalid_pri(pri) ((pri) < MINPRI || (pri) > MAXPRI)

struct run_queue {
	queue_head_t		queues[NRQS];		/* one for each priority */
	decl_simple_lock_data(,lock)			/* one lock for all queues */
	int					bitmap[NRQBM];		/* run queue bitmap array */
	int					highq;				/* highest runnable queue */
	int					count;				/* # of runnable threads */
};

typedef struct run_queue	*run_queue_t;
#define RUN_QUEUE_NULL		((run_queue_t) 0)

#define csw_needed(thread, processor) (										\
	((thread)->state & TH_SUSP)										||		\
	((processor)->first_quantum?											\
	 ((processor)->runq.highq > (thread)->sched_pri				||			\
	  (processor)->processor_set->runq.highq > (thread)->sched_pri) :		\
	 ((processor)->runq.highq >= (thread)->sched_pri			||			\
	  (processor)->processor_set->runq.highq >= (thread)->sched_pri))	)

/*
 *	Scheduler routines.
 */

/* Remove thread from its run queue */
extern run_queue_t	rem_runq(
					thread_t	thread);

/* Mach factor computation (in mach_factor.c) */
extern void		compute_mach_factor(void);

/* Update threads quantum (in priority.c) */
extern void		thread_quantum_update(
					int			mycpu,
					thread_t	thread,
					int			nticks,
					int			state);

extern int		min_quantum;	/* defines max context switch rate */

/*
 *	Shift structures for holding update shifts.  Actual computation
 *	is  usage = (usage >> shift1) +/- (usage >> abs(shift2))  where the
 *	+/- is determined by the sign of shift 2.
 */
struct shift {
	int	shift1;
	int	shift2;
};

typedef	struct shift	*shift_t, shift_data_t;

/*
 *	Age usage (1 << SCHED_TICK_SHIFT) times per second.
 */

extern unsigned	sched_tick;

#define SCHED_TICK_SHIFT	3

#define SCHED_SCALE		128
#define SCHED_SHIFT		7

/*
 *	thread_timer_delta macro takes care of both thread timers.
 */

#define thread_timer_delta(thread)  						\
MACRO_BEGIN													\
	register unsigned	delta;								\
															\
	delta = 0;												\
	TIMER_DELTA((thread)->system_timer,						\
					(thread)->system_timer_save, delta);	\
	TIMER_DELTA((thread)->user_timer,						\
					(thread)->user_timer_save, delta);		\
	(thread)->cpu_delta += delta;							\
	(thread)->sched_delta += (delta * 						\
					(thread)->processor_set->sched_load);	\
MACRO_END

#if	SIMPLE_CLOCK
/*
 *	sched_usec is an exponential average of number of microseconds
 *	in a second for clock drift compensation.
 */

extern int	sched_usec;
#endif	/* SIMPLE_CLOCK */

#endif	/* _KERN_SCHED_H_ */
