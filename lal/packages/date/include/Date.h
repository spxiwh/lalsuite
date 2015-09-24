/*
*  Copyright (C) 2007 Duncan Brown, David Chin, Jolien Creighton, Kipp Cannon, Reinhard Prix, Stephen Fairhurst
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/** \file
 * \ingroup std
 * \author Chin, D. W. and Creighton, J. D. E.
 * \brief Routines for converting between various time representations.
 *
 */
/* <lalVerbatim file="DateHV">

Author: David Chin <dwchin@umich.edu> +1-734-709-9119

</lalVerbatim> */

/* <lalLaTeX>

\section{Header \texttt{Date.h}}
\label{s:Date.h}

Provides routines for manipulating date and time information.

\subsection*{Synopsis}
\begin{verbatim}
#include <lal/Date.h>
\end{verbatim}

This header covers routines for manipulating date and time
information.  The various time systems are discussed in~\cite{esaa:1992}.


</lalLaTeX> */

#ifndef _DATE_H
#define _DATE_H

/* the following two preprocessor defines are to include the prototypes for
 * gmtime_r() and asctime_r() from /usr/include/time.h
 * HOWEVER, they do no good if -ansi is used in gcc: warnings are generated
 * that the prototypes have not been seen */

/* HP-UX and Solaris */
#ifndef _REENTRANT
#   define _REENTRANT
#endif

#ifndef _POSIX_PTHREAD_SEMANTICS
#   define _POSIX_PTHREAD_SEMANTICS
#endif

/* Linux */
#ifndef __USE_POSIX
#   define __USE_POSIX
#endif

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <lal/LALConstants.h>
#include <lal/LALAtomicDatatypes.h>
#include <lal/LALDatatypes.h>
#include <lal/LALStdlib.h>

#include <lal/DetectorSite.h>

#ifdef  __cplusplus
extern "C"
{
#endif

/** The UNIX time of the GPS origin epoch.
 *
 * 1980 6 JAN 0h UTC is 3657 days after 1970 1 JAN 0h UTC:
 * 8 standard years of 365 days = 2920 days
 * 2 leap years of 366 days = 734 days
 * 5 extra days.
 * Hence 3657*86400=315964800.
 *
 * Note that this deliberately does not account for any leap seconds in the
 * interval.  POSIX:2001 defines the relation between the UNIX time
 * \c time_t \c t and a broken down time \c struct \c tm \c utc as
 * \code
 * t = utc.tm_sec + utc.tm_min*60 + utc.tm_hour*3600
 *     + utc.tm_yday*86400 + (utc.tm_year-70)*31536000
 *     + ((utc.tm_year-69)/4)*86400 - ((utc.tm_year-1)/100)*86400
 *     + ((utc.tm_year+299)/400)*86400;
 * \endcode
 * so if you were to set \c utc.tm_sec=utc.tm_min=utc.tm_hour=0,
 * \c utc.tm_yday=5, and \c utc.tm_year=80, then you get
 * \c t=315964800.  That is what this is.
 */
#define XLAL_EPOCH_UNIX_GPS 315964800
#define XLAL_EPOCH_J2000_0_JD 2451545.0 /**< Julian Day of the J2000.0 epoch (2000 JAN 1 12h UTC). */
#define XLAL_EPOCH_J2000_0_TAI_UTC 32 /**< Leap seconds (TAI-UTC) on the J2000.0 epoch (2000 JAN 1 12h UTC). */
#define XLAL_EPOCH_J2000_0_GPS 630763213 /**< GPS seconds of the J2000.0 epoch (2000 JAN 1 12h UTC). */
#define XLAL_EPOCH_GPS_JD 2444244.5 /**< Julian Day of the GPS epoch (1980 JAN 6 0h UTC) */
#define XLAL_EPOCH_GPS_TAI_UTC 19 /**< Leap seconds (TAI-UTC) on the GPS epoch (1980 JAN 6 0h UTC) */
#define XLAL_MJD_REF 2400000.5 /**< Reference Julian Day for Mean Julian Day. */
#define XLAL_MODIFIED_JULIEN_DAY(utc) (XLALJulianDay(utc)-XLAL_MJD_REF) /**< Modified Julian Day for specified civil time structure. */

/** Converts GPS time to nano seconds stored as an INT8. */
INT8 XLALGPSToINT8NS( const LIGOTimeGPS *epoch );

/** Converts nano seconds stored as an INT8 to GPS time. */
LIGOTimeGPS * XLALINT8NSToGPS( LIGOTimeGPS *epoch, INT8 ns );

/** Sets GPS time given GPS integer seconds and residual nanoseconds. */
LIGOTimeGPS * XLALGPSSet( LIGOTimeGPS *epoch, INT4 gpssec, INT4 gpsnan );

/** Sets GPS time given GPS seconds as a REAL8. */
LIGOTimeGPS * XLALGPSSetREAL8( LIGOTimeGPS *epoch, REAL8 t );

/** Returns GPS time as a REAL8. */
REAL8 XLALGPSGetREAL8( const LIGOTimeGPS *epoch );

/** Adds dt to a GPS time. */
LIGOTimeGPS * XLALGPSAdd( LIGOTimeGPS *epoch, REAL8 dt );

/** Adds two GPS times. */
LIGOTimeGPS * XLALGPSAddGPS( LIGOTimeGPS *epoch, const LIGOTimeGPS *dt );

/** Difference between two GPS times. */
REAL8 XLALGPSDiff( const LIGOTimeGPS *t1, const LIGOTimeGPS *t0 );

/** Compares two GPS times.
 * Returns:
 *  - -1 if t0 < t1
 *  - 0 if t0 == t1
 *  - 1 if t0 > t1.
 */
int XLALGPSCmp( const LIGOTimeGPS *t0, const LIGOTimeGPS *t1 );

/** Multiply a GPS time by a REAL8 */
LIGOTimeGPS *XLALGPSMultiply( LIGOTimeGPS *gps, REAL8 x );

/** Divide a GPS time by a REAL8 */
LIGOTimeGPS *XLALGPSDivide( LIGOTimeGPS *gps, REAL8 x );

/** Returns the leap seconds TAI-UTC at a given GPS second. */
int XLALLeapSeconds( INT4 gpssec /**< [In] Seconds relative to GPS epoch.*/ );

/** Returns the leap seconds GPS-UTC at a given GPS second. */
int XLALGPSLeapSeconds( INT4 gpssec /**< [In] Seconds relative to GPS epoch.*/ );

/** Returns the leap seconds TAI-UTC for a given UTC broken down time. */
int XLALLeapSecondsUTC( const struct tm *utc /**< [In] UTC as a broken down time.*/ );

/** Returns the GPS seconds since the GPS epoch for a
 * specified UTC time structure. */
INT4 XLALUTCToGPS( const struct tm *utc /**< [In] UTC time in a broken down time structure. */ );

/** Returns a pointer to a tm structure representing the time
 * specified in seconds since the GPS epoch.  */
struct tm * XLALGPSToUTC(
    struct tm *utc, /**< [Out] Pointer to tm struct where result is stored. */
    INT4 gpssec /**< [In] Seconds since the GPS epoch. */
    );

/** Returns the Julian Day (JD) corresponding to the date given in a broken
 * down time structure. */
REAL8 XLALJulianDay( const struct tm *utc /**< [In] UTC time in a broken down time structure. */ );

/** Returns the Modified Julian Day (MJD) corresponding to the date given
 * in a broken down time structure.
 *
 * Note:
 *   - By convention, MJD is an integer.
 *   - MJD number starts at midnight rather than noon.
 *
 * If you want a Modified Julian Day that has a fractional part, simply use
 * the macro:
 *
 * #define XLAL_MODIFIED_JULIAN_DAY(utc) (XLALJulianDay(utc)-XLAL_MJD_REF)
 */
INT4 XLALModifiedJulianDay( const struct tm *utc /**< [In] UTC time in a broken down time structure. */ );

/** Returns the Greenwich mean or aparent sideral time in radians.
 */
REAL8 XLALGreenwichSiderealTime(
	const LIGOTimeGPS *gpstime,
	REAL8 equation_of_equinoxes
);

/** Returns the Greenwich Mean Sidereal Time in RADIANS for a specified GPS
 * time. */
REAL8 XLALGreenwichMeanSiderealTime(
	const LIGOTimeGPS *gpstime
);

/** Returns the GPS time for the given Greenwich mean sidereal time (in
 * radians). */
LIGOTimeGPS *XLALGreenwichMeanSiderealTimeToGPS(
	REAL8 gmst,
	LIGOTimeGPS *gps
);

/** Returns the GPS time for the given Greenwich sidereal time (in
 * radians). */
LIGOTimeGPS *XLALGreenwichSiderealTimeToGPS(
	REAL8 gmst,
	REAL8 equation_of_equinoxes,
	LIGOTimeGPS *gps
);


/* <lalLaTeX>

\subsection*{Structures}


\vfill{\footnotesize\input{DateHV}}


\subsubsection*{Structure \texttt{LALUnixDate}}
\idx[Type]{LALUnixDate}

This structure is just the standard Unix \texttt{tm} structure, described
in the man page for \texttt{ctime(3)}.  We shall
{\em always} ignore the daylight savings time field, \verb+tm_isdst+.

</lalLaTeX> */

/*
 * The standard Unix tm structure
 */
typedef struct
tm
LALUnixDate;


/* <lalLaTeX>


\subsubsection{Structure \texttt{LALPlaceAndGPS}}
\idx[Type]{LALPlaceAndGPS}

This structure stores pointers to a \texttt{LALDetector} and a
\texttt{LIGOTimeGPS}. Its sole purpose is to aggregate these
structures for passing to functions.  The fields are:

\begin{description}
\item{\verb+LALDetector *p_detector+} Pointer to a detector
\item{\verb+LIGOTimeGPS *p_gps+} Pointer to a GPS time structure
\end{description}

</lalLaTeX> */

/*
 * Place and time structures
 */
/* First, with GPS */
typedef struct
tagLALPlaceAndGPS
{
    LALDetector *p_detector;   /* pointer to a detector */
    LIGOTimeGPS *p_gps;        /* pointer to GPS time */
}
LALPlaceAndGPS;


/*
 * Function prototypes
 */

int XLALStrToGPS(LIGOTimeGPS *t, const char *nptr, char **endptr);
char *XLALGPSToStr(char *, const LIGOTimeGPS *t);


/* This function returns the current GPS time according to the system clock */
LIGOTimeGPS *
XLALGPSTimeNow (
    LIGOTimeGPS *gpstime
    );

/* <lalLaTeX>
\newpage\input{PlaygroundC}
</lalLaTeX> */
int
XLALINT8NanoSecIsPlayground (
    INT8        ns
    );

#ifdef  __cplusplus
}
#endif

#endif /* _DATE_H */