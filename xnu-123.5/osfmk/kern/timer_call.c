/*
 * Copyright (c) 1993-1995, 1999-2000 Apple Computer, Inc.
 * All rights reserved.
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
 * Timer interrupt callout module.
 *
 * HISTORY
 *
 * 20 December 2000 (debo)
 *	Created.
 */

#include <mach/mach_types.h>

#include <kern/clock.h>

#include <kern/timer_call.h>
#include <kern/call_entry.h>

decl_simple_lock_data(static,timer_call_lock)

static
queue_head_t
	delayed_call_queues[NCPUS];

static struct {
	int		pending_num,
			pending_hiwat;
	int		delayed_num,
			delayed_hiwat;
} timer_calls;

static boolean_t
	timer_call_initialized = FALSE;

static void
timer_call_interrupt(
	AbsoluteTime		timestamp);

#define qe(x)		((queue_entry_t)(x))
#define TC(x)		((timer_call_t)(x))

void
timer_call_initialize(void)
{
	spl_t				s;
	int					i;

	if (timer_call_initialized)
		panic("timer_call_initialize");

	simple_lock_init(&timer_call_lock, ETAP_MISC_TIMER);

	s = splclock();
	simple_lock(&timer_call_lock);

	for (i = 0; i < NCPUS; i++)
		queue_init(&delayed_call_queues[i]);

	clock_set_timer_func((clock_timer_func_t)timer_call_interrupt);

	timer_call_initialized = TRUE;

	simple_unlock(&timer_call_lock);
	splx(s);
}

void
timer_call_setup(
	timer_call_t			call,
	timer_call_func_t		func,
	timer_call_param_t		param0)
{
	call_entry_setup(call, func, param0);
}

static __inline__
void
_delayed_call_enqueue(
	queue_t					queue,
	timer_call_t			call)
{
	timer_call_t	current;

	current = TC(queue_first(queue));

	while (TRUE) {
		if (	queue_end(queue, qe(current))						||
					CMP_ABSOLUTETIME(&call->deadline,
											&current->deadline) < 0		) {
			current = TC(queue_prev(qe(current)));
			break;
		}

		current = TC(queue_next(qe(current)));
	}

	insque(qe(call), qe(current));
	if (++timer_calls.delayed_num > timer_calls.delayed_hiwat)
		timer_calls.delayed_hiwat = timer_calls.delayed_num;

	call->state = DELAYED;
}

static __inline__
void
_delayed_call_dequeue(
	timer_call_t			call)
{
	(void)remque(qe(call));
	timer_calls.delayed_num--;

	call->state = IDLE;
}

static __inline__
void
_pending_call_enqueue(
	queue_t					queue,
	timer_call_t			call)
{
	enqueue_tail(queue, qe(call));
	if (++timer_calls.pending_num > timer_calls.pending_hiwat)
		timer_calls.pending_hiwat = timer_calls.pending_num;

	call->state = PENDING;
}

static __inline__
void
_pending_call_dequeue(
	timer_call_t			call)
{
	(void)remque(qe(call));
	timer_calls.pending_num--;

	call->state = IDLE;
}

static __inline__
void
_set_delayed_call_timer(
	timer_call_t			call)
{
	clock_set_timer_deadline(call->deadline);
}

boolean_t
timer_call_enter(
	timer_call_t			call,
	AbsoluteTime			deadline)
{
	boolean_t		result = TRUE;
	queue_t			delayed;
	spl_t			s;

	s = splclock();
	simple_lock(&timer_call_lock);

	if (call->state == PENDING)
		_pending_call_dequeue(call);
	else if (call->state == DELAYED)
		_delayed_call_dequeue(call);
	else if (call->state == IDLE)
		result = FALSE;

	call->param1	= 0;
	call->deadline	= deadline;

	delayed = &delayed_call_queues[cpu_number()];

	_delayed_call_enqueue(delayed, call);

	if (queue_first(delayed) == qe(call))
		_set_delayed_call_timer(call);

	simple_unlock(&timer_call_lock);
	splx(s);

	return (result);
}

boolean_t
timer_call_enter1(
	timer_call_t			call,
	timer_call_param_t		param1,
	AbsoluteTime			deadline)
{
	boolean_t		result = TRUE;
	queue_t			delayed;
	spl_t			s;

	s = splclock();
	simple_lock(&timer_call_lock);

	if (call->state == PENDING)
		_pending_call_dequeue(call);
	else if (call->state == DELAYED)
		_delayed_call_dequeue(call);
	else if (call->state == IDLE)
		result = FALSE;

	call->param1	= param1;
	call->deadline	= deadline;

	delayed = &delayed_call_queues[cpu_number()];

	_delayed_call_enqueue(delayed, call);

	if (queue_first(delayed) == qe(call))
		_set_delayed_call_timer(call);

	simple_unlock(&timer_call_lock);
	splx(s);

	return (result);
}

boolean_t
timer_call_cancel(
	timer_call_t			call)
{
	boolean_t		result = TRUE;
	spl_t			s;

	s = splclock();
	simple_lock(&timer_call_lock);

	if (call->state == PENDING)
		_pending_call_dequeue(call);
	else if (call->state == DELAYED)
		_delayed_call_dequeue(call);
	else
		result = FALSE;

	simple_unlock(&timer_call_lock);
	splx(s);

	return (result);
}

boolean_t
timer_call_is_delayed(
	timer_call_t			call,
	AbsoluteTime			*deadline)
{
	boolean_t		result = FALSE;
	spl_t			s;

	s = splclock();
	simple_lock(&timer_call_lock);

	if (call->state == DELAYED) {
		if (deadline != NULL)
			*deadline = call->deadline;
		result = TRUE;
	}

	simple_unlock(&timer_call_lock);
	splx(s);

	return (result);
}

static
void
timer_call_interrupt(
	AbsoluteTime			timestamp)
{
	timer_call_t		call;
	queue_t				delayed = &delayed_call_queues[cpu_number()];

	simple_lock(&timer_call_lock);

	call = TC(queue_first(delayed));

	while (!queue_end(delayed, qe(call))) {
		if (CMP_ABSOLUTETIME(&call->deadline, &timestamp) <= 0) {
			timer_call_func_t		func;
			timer_call_param_t		param0, param1;

			_delayed_call_dequeue(call);

			func = call->func;
			param0 = call->param0;
			param1 = call->param1;

			simple_unlock(&timer_call_lock);

			(*func)(param0, param1);

			simple_lock(&timer_call_lock);
		}
		else
			break;

		call = TC(queue_first(delayed));
	}

	if (!queue_end(delayed, qe(call)))
		_set_delayed_call_timer(call);

	simple_unlock(&timer_call_lock);
}
