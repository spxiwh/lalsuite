# $Id$
#
# Copyright (C) 2007  Kipp C. Cannon
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


#
# =============================================================================
#
#                                   Preamble
#
# =============================================================================
#


import math
import numpy
import sys


from pylal import date
from pylal import inject
from pylal import rate
from pylal import SnglBurstUtils


__author__ = "Kipp Cannon <kipp@gravity.phys.uwm.edu>"
__version__ = "$Revision$"[11:-2]
__date__ = "$Date$"[7:-2]


#
# =============================================================================
#
#                                    Misc.
#
# =============================================================================
#


def injection_was_made(sim, seglist, instruments):
	"""
	Return True if the peak times for sim for all of the instruments
	lie within the segment list.
	"""
	for instrument in instruments:
		if sim.get_peak(instrument) not in seglist:
			return False
	return True


#
# =============================================================================
#
#                            h_{rss} in Instrument
#
# =============================================================================
#


def hrss_in_instrument(sim, instrument):
	"""
	Given an injection and an instrument, compute and return the h_rss
	of the injection as should be observed in the instrument.  That is,
	project the waveform onto the instrument, and return the root
	integrated strain squared.
	"""
	if sim.coordinates != "EQUATORIAL":
		raise ValueError, sim.coordinates
	fplus, fcross = inject.XLALComputeDetAMResponse(
		inject.cached_detector[inject.prefix_to_name[instrument]].response,
		sim.longitude,	# ra
		sim.latitude,	# dec
		sim.polarization,
		date.XLALGreenwichMeanSiderealTime(sim.get_peak(instrument))
	)
	return abs(fplus) * sim.hrss


#
# =============================================================================
#
#                             Efficiency Contours
#
# =============================================================================
#


def burst_matches_injection(h_peak, h_peak_ns, l_peak, l_peak_ns, start, start_ns, duration, ifo):
	"""
	A burst "matches an injection" if it lies within 2 s of the
	injection.  In other words, for the purpose of efficiency
	measurements an injection will have been said to be recovered if a
	triple coincident event survives to the end of the pipeline with at
	least one burst in the coinc lieing within 2 s of the injection.
	This is not the same as asking for bursts that match injections
	well.
	"""
	start = LIGOTimeGPS(start, start_ns)
	seg = segments.segment(start, start + duration).protract(2)
	if ifo in ("H1", "H2"):
		return LIGOTimeGPS(h_peak, h_peak_ns) in seg
	elif ifo in ("L1",):
		return LIGOTimeGPS(l_peak, l_peak_ns) in seg
	raise ValueError, ifo


#
# h_{rss} vs. peak frequency
#


class Efficiency_hrss_vs_freq(object):
	def __init__(self, instrument, hrss_func, error):
		self.instrument = instrument
		self.hrss_func = hrss_func
		self.error = error
		self.injected_x = []
		self.injected_y = []
		self.found_x = []
		self.found_y = []


	def add_contents(self, contents):
		seglist = contents.seglists[self.instrument]
		for values in contents.connection.cursor().execute("""
SELECT
	sim_burst.*,
	EXISTS (
		-- Find a link through the coinc_event_map table to a row
		-- in the sngl_burst table with the correct ifo value.
		SELECT
			*
		FROM
			coinc_event_map AS a
			JOIN coinc_event_map AS b ON (
				a.coinc_event_id == b.coinc_event_id
			)
			JOIN sngl_burst ON (
				b.table_name == 'sngl_burst'
				AND b.event_id == sngl_burst.event_id
			)
		WHERE
			a.table_name == 'sim_burst'
			AND a.event_id == sim_burst.simulation_id
			AND sngl_burst.ifo == ?
	)
FROM
	sim_burst
		""", (self.instrument,)):
			sim = contents.sim_burst_table._row_from_cols(values)
			found = values[-1]
			# FIXME:  this assumes all injections are done at
			# zero lag (which is correct, for now, but watch
			# out for this)
			if injection_was_made(sim, seglist, (self.instrument,)):
				sim.hrss = self.hrss_func(sim, self.instrument)
				self.injected_x.append(sim.freq)
				self.injected_y.append(sim.hrss)
				if found:
					self.found_x.append(sim.freq)
					self.found_y.append(sim.hrss)
			elif found:
				print >>sys.stderr, "odd, injection %s was found but not injected..." % sim.simulation_id

	def finish(self, binning = None):
		if binning is None:
			minx, maxx = min(self.injected_x), max(self.injected_x)
			miny, maxy = min(self.injected_y), max(self.injected_y)
			binning = rate.NDBins((rate.LogarithmicBins(minx, maxx, 256), rate.LogarithmicBins(miny, maxy, 256)))

		self.efficiency = rate.BinnedRatios(binning)

		map(self.efficiency.incdenominator, zip(self.injected_x, self.injected_y))
		map(self.efficiency.incnumerator, zip(self.found_x, self.found_y))

		# 1 / error^2 is the number of injections that need to be
		# within the window in order for the uncertainty in that
		# number to be = error.  multiplying by bins_per_inj tells
		# us how many bins the window needs to cover, and taking
		# the square root translates that into the window's length
		# on a side in bins.  because the contours tend to run
		# parallel to the x axis, the window is dilated in that
		# direction to improve resolution.
		bins_per_inj = self.efficiency.used() / float(len(self.injected_x))
		self.window_size_x = self.window_size_y = (bins_per_inj / self.error**2)**0.5
		self.window_size_x *= math.sqrt(2)
		self.window_size_y /= math.sqrt(2)
		if self.window_size_x > 100 or self.window_size_y > 100:
			# program will take too long to run
			raise ValueError, "smoothing filter too large (not enough injections)"

		print >>sys.stderr, "The smoothing window for %s is %g x %g bins" % (self.instrument, self.window_size_x, self.window_size_y),
		print >>sys.stderr, "which is %g%% x %g%% of the binning" % (100.0 * self.window_size_x / binning[0].n, 100.0 * self.window_size_y / binning[1].n)

		# smooth the efficiency data.
		rate.filter_binned_ratios(self.efficiency, rate.gaussian_window2d(self.window_size_x, self.window_size_y))

		# regularize to prevent divide-by-zero errors
		self.efficiency.regularize()


def plot_Efficiency_hrss_vs_freq(efficiency):
	"""
	Generate a plot from an Efficiency_hrss_vs_freq instance.
	"""
	plot = SnglBurstUtils.BurstPlot("Frequency (Hz)", r"$h_{\mathrm{rss}}$")
	plot.axes.loglog()

	xcoords, ycoords = efficiency.efficiency.centres()
	zvals = efficiency.efficiency.ratio()
	cset = plot.axes.contour(xcoords, ycoords, numpy.transpose(zvals), (.1, .2, .3, .4, .5, .6, .7, .8, .9))
	cset.clabel(inline = True, fontsize = 5, fmt = r"$%%g \pm %g$" % efficiency.error, colors = "k")
	plot.axes.set_title(r"%s Injection Detection Efficiency (%d of %d Found)" % (efficiency.instrument, len(efficiency.found_x), len(efficiency.injected_x)))
	return plot.fig


#
# =============================================================================
#
#                             Source Co-ordinates
#
# =============================================================================
#


#
# Location of the galactic core
#	ra = 27940.04 s = 7 h 45 m 40.04 s
#	dec = -29o 00' 28.1"
#


MW_CENTER_J2000_RA_RAD = 2.0318570464121519
MW_CENTER_J2000_DEC_RAD = -0.50628171572274738

