/*
 * refclock_conf.c - reference clock configuration
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

#ifdef REFCLOCK

static struct refclock refclock_none = {
	noentry, noentry, noentry, noentry, noentry, noentry, NOFLAGS
};

#ifdef CLOCK_LOCAL
extern	struct refclock	refclock_local;
#else
#define	refclock_local	refclock_none
#endif

#ifdef CLOCK_TRAK
extern	struct refclock	refclock_trak;
#else
#define	refclock_trak	refclock_none
#endif

#ifdef CLOCK_PST
extern	struct refclock	refclock_pst;
#else
#define	refclock_pst	refclock_none
#endif

#ifdef CLOCK_CHU
extern	struct refclock	refclock_chu;
#else
#define	refclock_chu	refclock_none
#endif

#ifdef CLOCK_WWVB
extern	struct refclock	refclock_wwvb;
#else
#define	refclock_wwvb	refclock_none
#endif

#ifdef CLOCK_PARSE
extern	struct refclock	refclock_parse;
#else
#define	refclock_parse	refclock_none
#endif

#if defined(CLOCK_MX4200) && defined(PPS)
extern	struct refclock	refclock_mx4200;
#else
#define	refclock_mx4200	refclock_none
#endif

#ifdef CLOCK_AS2201
extern	struct refclock	refclock_as2201;
#else
#define	refclock_as2201	refclock_none
#endif

#ifdef CLOCK_ARBITER
extern  struct refclock refclock_arbiter;
#else
#define refclock_arbiter refclock_none
#endif

#ifdef CLOCK_TPRO
extern	struct refclock	refclock_tpro;
#else
#define	refclock_tpro	refclock_none
#endif

#ifdef CLOCK_LEITCH
extern	struct refclock	refclock_leitch;
#else
#define	refclock_leitch	refclock_none
#endif

#ifdef CLOCK_IRIG
extern	struct refclock	refclock_irig;
#else
#define refclock_irig	refclock_none
#endif

#if defined(CLOCK_MSFEES) && defined(PPS)
extern	struct refclock	refclock_msfees;
#else
#define refclock_msfees	refclock_none
#endif

#ifdef CLOCK_BANC
extern	struct refclock refclock_bancomm;
#else
#define refclock_bancomm refclock_none
#endif

#ifdef CLOCK_TRUETIME
extern	struct refclock	refclock_true;
#else
#define	refclock_true	refclock_none
#endif

#ifdef CLOCK_DATUM
extern	struct refclock	refclock_datum;
#else
#define refclock_datum	refclock_none
#endif

#ifdef CLOCK_ACTS
extern	struct refclock	refclock_acts;
#else
#define refclock_acts	refclock_none
#endif

#ifdef CLOCK_HEATH
extern	struct refclock	refclock_heath;
#else
#define refclock_heath	refclock_none
#endif

#ifdef CLOCK_NMEA
extern	struct refclock refclock_nmea;
#else
#define	refclock_nmea	refclock_none
#endif

#ifdef CLOCK_ATOM
extern	struct refclock	refclock_atom;
#else
#define refclock_atom	refclock_none
#endif

#ifdef CLOCK_PTBACTS
extern	struct refclock	refclock_ptb;
#else
#define refclock_ptb	refclock_none
#endif

#ifdef CLOCK_USNO
extern	struct refclock	refclock_usno;
#else
#define refclock_usno	refclock_none
#endif

#ifdef CLOCK_HPGPS
extern	struct refclock	refclock_hpgps;
#else
#define	refclock_hpgps	refclock_none
#endif

#ifdef CLOCK_GPSVME
extern	struct refclock refclock_gpsvme;
#else
#define refclock_gpsvme refclock_none
#endif

#ifdef CLOCK_ARCRON_MSF
extern	struct refclock refclock_arc;
#else
#define refclock_arc refclock_none
#endif

#ifdef SHM
extern	struct refclock refclock_shm;
#else
#define refclock_shm refclock_none
#endif

#ifdef CLOCK_PALISADE
extern  struct refclock refclock_palisade;
#else
#define refclock_palisade refclock_none
#endif

#ifdef CLOCK_ONCORE
extern	struct refclock refclock_oncore;
#else
#define refclock_oncore refclock_none
#endif

#if defined(CLOCK_JUPITER) && defined(PPS)
extern	struct refclock refclock_jupiter;
#else
#define refclock_jupiter refclock_none
#endif

#if defined(CLOCK_CHRONOLOG)
extern struct refclock refclock_chronolog;
#else
#define refclock_chronolog refclock_none
#endif

#if defined(CLOCK_DUMBCLOCK)
extern struct refclock refclock_dumbclock;
#else
#define refclock_dumbclock refclock_none
#endif

/*
 * Order is clock_start(), clock_shutdown(), clock_poll(),
 * clock_control(), clock_init(), clock_buginfo, clock_flags;
 *
 * Types are defined in ntp.h.  The index must match this.
 */
struct refclock *refclock_conf[] = {
	&refclock_none,		/* 0 REFCLK_NONE */
	&refclock_local,	/* 1 REFCLK_LOCAL */
	&refclock_trak,		/* 2 REFCLK_GPS_TRAK */
	&refclock_pst,		/* 3 REFCLK_WWV_PST */
	&refclock_wwvb, 	/* 4 REFCLK_WWVB_SPECTRACOM */
	&refclock_true,		/* 5 REFCLK_TRUETIME */
	&refclock_irig,		/* 6 REFCLK_IRIG_AUDIO */
	&refclock_chu,		/* 7 REFCLK_CHU */
	&refclock_parse,	/* 8 REFCLK_PARSE */
	&refclock_mx4200,	/* 9 REFCLK_GPS_MX4200 */
	&refclock_as2201,	/* 10 REFCLK_GPS_AS2201 */
	&refclock_arbiter,	/* 11 REFCLK_GPS_ARBITER */
        &refclock_tpro,		/* 12 REFCLK_IRIG_TPRO */
	&refclock_leitch,	/* 13 REFCLK_ATOM_LEITCH */
	&refclock_msfees,	/* 14 REFCLK_MSF_EES */
	&refclock_true,		/* 15 alias for REFCLK_TRUETIME */
	&refclock_bancomm,	/* 16 REFCLK_IRIG_BANCOMM */
	&refclock_datum,	/* 17 REFCLK_GPS_DATUM */
	&refclock_acts,		/* 18 REFCLK_NIST_ACTS */
	&refclock_heath,	/* 19 REFCLK_WWV_HEATH */
	&refclock_nmea,		/* 20 REFCLK_GPS_NMEA */
	&refclock_gpsvme,	/* 21 REFCLK_GPS_VME */
	&refclock_atom,		/* 22 REFCLK_ATOM_PPS */
	&refclock_ptb,		/* 23 REFCLK_PTB_ACTS */
	&refclock_usno,		/* 24 REFCLK_USNO */
	&refclock_true,		/* 25 alias for REFCLK_TRUETIME */
	&refclock_hpgps,	/* 26 REFCLK_GPS_HP */
	&refclock_arc, 		/* 27 REFCLK_ARCRON_MSF */
	&refclock_shm,		/* 28 REFCLK_SHM */
	&refclock_palisade,     /* 29 REFCLK_PALISADE */
	&refclock_oncore,	/* 30 REFCLK_ONCORE */
	&refclock_jupiter,	/* 31 REFCLK_GPS_JUPITER */
	&refclock_chronolog,	/* 32 REFCLK_CHRONOLOG */
	&refclock_dumbclock,	/* 33 REFCLK_DUMBCLOCK */
};

u_char num_refclock_conf = sizeof(refclock_conf)/sizeof(struct refclock *);

#else
int refclock_conf_bs;
#endif
