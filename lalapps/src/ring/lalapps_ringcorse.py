#
# Copyright (C) 2009  Kipp Cannon
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


import bisect
import numpy
from new import instancemethod
from optparse import OptionParser
try:
	import sqlite3
except ImportError:
	# pre 2.5.x
	from pysqlite2 import dbapi2 as sqlite3
import sys

sqlite3.enable_callback_tracebacks(True)

from glue import segments
from glue.ligolw import lsctables
from glue.ligolw import dbtables
from glue.ligolw import utils
from pylal import rate
from pylal import db_thinca_rings
from pylal import git_version
from pylal.xlal.datatypes.ligotimegps import LIGOTimeGPS
from glue.ligolw import table
from pylal import llwapp
from pylal import farutils
from glue.lal import Cache, CacheEntry
from glue import segmentsUtils
from glue.ligolw.utils import process

lsctables.LIGOTimeGPS = LIGOTimeGPS


__author__ = "Kipp Cannon <kcannon@ligo.org>"
__version__ = "git id %s" % git_version.id
__date__ = git_version.date

#
# =============================================================================
#
#                                 Command Line
#
# =============================================================================
#


def parse_command_line():
	parser = OptionParser(
		version = "Name: %%prog\n%s" % git_version.verbose_msg,
		usage = "%prog [options] [file ...]",
		description = "%prog does blah blah blah."
	)
	parser.add_option("-p", "--live-time-program", metavar = "name", default="lalapps_ring", help = "Set the name of the program whose entries in the search_summary table will set the search live time.  Required.")
	parser.add_option("--veto-segments-name", help = "Set the name of the segments to extract from the segment tables and use as the veto list.")
	parser.add_option("--categories", metavar = "{\"frequency-ifos-oninstruments\",\"oninstruments\"}", default="oninstruments", help = "Select the event categorization algorithm.  Default oninstruments")
	parser.add_option("-b", "--frequency-bins", metavar = "frequency,frequency[,frequency,...]", help = "Set the boundaries of the frequency bins in Hz.  The lowest and highest bounds must be explicitly listed.  Example \"0,5,inf\".  Required if frequency-based categorization algorithm has been selected.")
	parser.add_option("--rank-by", metavar = "{\"snr\",\"uncombined-ifar\",\"likelihood\"}", default="snr", help = "Select the event ranking method.  Default is snr")
	parser.add_option("-t", "--tmp-space", metavar = "path", help = "Path to a directory suitable for use as a work area while manipulating the database file.  The database file will be worked on in this directory, and then moved to the final location when complete.  This option is intended to improve performance when running in a networked environment, where there might be a local disk with higher bandwidth than is available to the filesystem on which the final output will reside.")
	parser.add_option("-n","--extrapolation-num",action="store",type="int",default=0, metavar="num",help="number of time-slide points to use in FAR extrapolation" )
	parser.add_option("-g", "--input-cache", help="cache of sqlite files")
	parser.add_option("-v", "--verbose", action = "store_true", help = "Be verbose.")
	options, filenames = parser.parse_args()
	if options.input_cache is not None: filenames.extend([CacheEntry(c).path() for c in open(options.input_cache)])

	#
	# categories and ranking
	#

	if options.categories not in ("frequency-ifos-oninstruments", "oninstruments"):
		raise ValueError, "missing or unrecognized --categories option"
	if options.rank_by not in ("snr", "uncombined-ifar", "likelihood"):
		raise ValueError, "missing or unrecognized --rank-by option"


	options.populate_column = "false_alarm_rate"

	#
	# parse frequency bins
	#

	if options.categories in ("frequency-ifos-oninstruments",):
		if options.frequency_bins is None:
			raise ValueError, "--frequency-bins required with category algorithm \"%s\"" % options.categories
		options.frequency_bins = sorted(map(float, options.frequency_bins.split(",")))
		if len(options.frequency_bins) < 2:
			raise ValueError, "must set at least two frequency bin boundaries (i.e., define at least one frequency bin)"
		options.frequency_bins = rate.IrregularBins(options.frequency_bins)

	#
	# other
	#

	if options.live_time_program is None:
		raise ValueError, "missing required option -p or --live-time-program"

	#
	# done
	#

	return options, (filenames or [None])


#
# =============================================================================
#
#                               Helper Functions
#
# =============================================================================
#


def extrapolation_coeffs(rank_by, N, reputations, t):
	#
	# calculate the FAR extrapolation coefficients
	#

	#
	# get just the loudest N in reputations
	#

	if len(reputations) < N and len(reputations) >= 2:
		print >>sys.stderr,"Warning: found a category with less than N=%i background triggers, using the number available: %i"%(N,len(reputations))
	elif len(reputations) < 2:
		print >>sys.stderr,"Warning: found a category with less than 2 background triggers, cannot perform extrapolation for this category"
		return (0, (0, 0))

	x = reputations[-N:]

	if rank_by == "snr":
		y = range(1, len(x) + 1, 1)
		y.reverse()

		Y0 = 0.
		X0 = 0.

		for idx in range(len(x)):
			sigmasq = 1. / y[idx]
			Y0 += numpy.log(1. * y[idx] / y[0]) * (x[idx]**2. - x[0]**2.) / sigmasq
			X0 += (x[idx]**2. - x[0]**2.)**2. / sigmasq

		B = Y0 / X0
		A = y[0] * numpy.exp(-B * x[0]**2.)
		return (len(x), (A, B))

	elif rank_by == "uncombined-ifar":
		A = float(len(x)) * x[0] / t
		return (len(x), (A, 0))

def extrapolate_reputation(rank_by, coeffs, reputation, t):
	#
	# calculate the FAN from the reputation and the extrapolation coefficients
	#

	if rank_by == "snr":
		return coeffs[0] * numpy.exp(coeffs[1] * reputation**2.)
	if rank_by == "uncombined-ifar":
		return coeffs[0] * t / reputation


#
# =============================================================================
#
#                                 Book-Keeping
#
# =============================================================================
#


class Summaries(object):
	def __init__(self, category_algorithm, rank_by, frequency_bins = None):
		if category_algorithm == "frequency-ifos-oninstruments":
			self.category_func = lambda self, on_instruments, participating_instruments, frequency: (on_instruments, participating_instruments, self.frequency_bins[frequency])
		elif category_algorithm == "oninstruments":
			self.category_func = lambda self, on_instruments, participating_instruments, frequency: on_instruments
		else:
			raise ValueError, category_algorithm
		self.category_func = instancemethod(self.category_func, self, self.__class__)

		if rank_by == "snr":
			self.reputation_func = lambda self, snr, uncombined_ifar, likelihood: snr
		elif rank_by == "uncombined-ifar":
			# cast to float so this does the right thing when
			# we get "inf" as a string
			self.reputation_func = lambda self, snr, uncombined_ifar, likelihood: float(uncombined_ifar)
		elif rank_by == "likelihood":
			self.reputation_func = lambda self, snr, uncombined_ifar, likelihood: likelihood
		else:
			raise ValueError, rank_by
		self.rank_by = rank_by
		self.reputation_func = instancemethod(self.reputation_func, self, self.__class__)

		self.frequency_bins = frequency_bins
		self.reputations = {}
		self.cached_livetime = {}
		self.extrapolate = False
		self.extrapolation_coeffs = {}
		self.categories = []

	def add_livetime(self, connection, live_time_program, veto_segments_name = None, verbose = False):
		if verbose:
			print >>sys.stderr, "\tcomputing livetimes:",

		xmldoc = dbtables.get_xml(connection)
		veto_segments = farutils.get_veto_segments(connection, live_time_program, xmldoc, veto_segments_name=veto_segments_name)
		segs = farutils.get_segments(connection, xmldoc, "lalapps_ring").coalesce()
		bglivetime=farutils.add_background_livetime(connection, live_time_program, segs, veto_segments, coinc_segments=None, verbose=verbose)

		if verbose: print >>sys.stderr, "\n\tbackground livetime %s\n" % (str(bglivetime))
		for key in bglivetime.keys():
			try:
				self.cached_livetime[key] += bglivetime[key]
			except KeyError:
				self.cached_livetime[key] = bglivetime[key]

	def add_coinc(self, on_instruments, participating_instruments, frequency, snr, uncombined_ifar, likelihood):
		self.reputations.setdefault(self.category_func(on_instruments, participating_instruments, frequency), []).append(self.reputation_func(snr, uncombined_ifar, likelihood))
		if self.category_func(on_instruments, participating_instruments, frequency) not in self.categories:
			self.categories.append(self.category_func(on_instruments, participating_instruments, frequency))

	def index(self):
		for reputations in self.reputations.values():
			reputations.sort()

	def rate_extrapolation(self, N):
		#
		# calculate the FAR extrapolation coefficients for the different categories
		#

		self.extrapolate = True
		for category in self.categories:
			t = self.cached_livetime.setdefault(category, 0.0)
			reputations = self.reputations.setdefault(category, [])
			self.extrapolation_coeffs[category] = extrapolation_coeffs(self.rank_by, N, reputations, t)

	def rate(self, on_instruments, participating_instruments, frequency, snr, uncombined_ifar, likelihood):
		#
		# retrieve the appropriate reputation list (create an empty
		# one if there are no reputations for this category)
		#

		category = self.category_func(frozenset(lsctables.instrument_set_from_ifos(on_instruments)), frozenset(lsctables.instrument_set_from_ifos(participating_instruments)), frequency)
		reputations = self.reputations.setdefault(category, [])


		#
		# len(x) - bisect.bisect_left(x, reputation) = number of
		# elements in list >= reputation
		#

		n = len(reputations) - bisect.bisect_left(reputations, self.reputation_func(snr, uncombined_ifar, likelihood))

		#
		# retrieve the livetime
		#

		t = self.cached_livetime.setdefault(frozenset(lsctables.instrument_set_from_ifos(on_instruments)), 0.0)
		if self.extrapolate:
			if n < self.extrapolation_coeffs[category][0]:
				n = extrapolate_reputation(self.rank_by, self.extrapolation_coeffs[category][1], self.reputation_func(snr, uncombined_ifar, likelihood), t)

		#
		# return the rate of events above the given reputation
		#
		try:
			return n / t

		except ZeroDivisionError, e:
			print >>sys.stderr, "found an event in category %s that has a livetime of 0 s" % repr(self.category_func(on_instruments, participating_instruments, frequency))
			raise e


#
# =============================================================================
#
#                                     Main
#
# =============================================================================
#


#
# command line
#


options, filenames = parse_command_line()


#
# initialize book-keeping
#


background = Summaries(options.categories, options.rank_by, frequency_bins = options.frequency_bins)


#
# iterate over database files accumulating background statistics
#


if options.verbose:
	print >>sys.stderr, "collecting background statistics ..."


for n, filename in enumerate(filenames):
	#
	# open the database
	#

	if options.verbose:
		print >>sys.stderr, "%d/%d: %s" % (n + 1, len(filenames), filename)
	working_filename = dbtables.get_connection_filename(filename, tmp_path = options.tmp_space, verbose = options.verbose)
	connection = sqlite3.connect(working_filename)

	#
	# if the database contains a sim_inspiral table then it is assumed
	# to represent an injection run.  its rings must not added to the
	# livetime, and it cannot provide background coincs, so it is just
	# skipped altogether in this first pass.
	#

	if "sim_ringdown" in dbtables.get_table_names(connection):
		if options.verbose:
			print >>sys.stderr, "\tdatabase contains sim_ringdown table, skipping ..."

		#
		# close the database
		#

		connection.close()
		dbtables.discard_connection_filename(filename, working_filename, verbose = options.verbose)
		continue

	#
	# compute and record background livetime
	#

	background.add_livetime(connection, options.live_time_program, veto_segments_name = options.veto_segments_name, verbose = options.verbose)

	#
	# count background coincs
	#

	if options.verbose:
		print >>sys.stderr, "\tcollecting background statistics ..."
	for on_instruments, participating_instruments, frequency, snr, uncombined_ifar, likelihood in connection.cursor().execute("""
SELECT
	coinc_event.instruments,
	coinc_ringdown.ifos,
	coinc_ringdown.frequency,
	coinc_ringdown.snr,
	CASE coinc_ringdown.false_alarm_rate WHEN 0 THEN "inf" ELSE 1.0 / coinc_ringdown.false_alarm_rate END,
	coinc_event.likelihood
FROM
	coinc_event
	JOIN coinc_ringdown ON (
		coinc_ringdown.coinc_event_id == coinc_event.coinc_event_id
	)
WHERE
	-- require coinc to be background (= at least one of its time slide offsets is non-zero)
	EXISTS (
		SELECT
			*
		FROM
			time_slide
		WHERE
			time_slide.time_slide_id == coinc_event.time_slide_id
		AND time_slide.offset != 0
	)
	"""):
		on_instruments = frozenset(lsctables.instrument_set_from_ifos(on_instruments))
		participating_instruments = frozenset(lsctables.instrument_set_from_ifos(participating_instruments))
		background.add_coinc(on_instruments, participating_instruments, frequency, snr, uncombined_ifar, likelihood)

	#
	# close the database
	#

	connection.close()
	dbtables.discard_connection_filename(filename, working_filename, verbose = options.verbose)


background.index()
if options.extrapolation_num:
	if options.verbose:
		print >>sys.stderr, "\tcalculating FAR extrapolation coefficients ..."
	background.rate_extrapolation(options.extrapolation_num)

#
# iterate over database files assigning false-alarm rates to coincs
#


for n, filename in enumerate(filenames):
	#
	# open the database
	#

	if options.verbose:
		print >>sys.stderr, "%d/%d: %s" % (n + 1, len(filenames), filename)
	working_filename = dbtables.get_connection_filename(filename, tmp_path = options.tmp_space, verbose = options.verbose)
	connection = sqlite3.connect(working_filename)

	#
	# prepare the database
	#

	connection.create_function("background_rate", 6, background.rate)

	#
	# count background coincs by type and mass bin
	#

	if options.verbose:
		print >>sys.stderr, "\tcalculating and recording false alarm rates ..."
	connection.cursor().execute("""
UPDATE
	coinc_ringdown
SET
	%s = (
		SELECT
			background_rate(
				coinc_event.instruments,
				coinc_ringdown.ifos,
				coinc_ringdown.frequency,
				coinc_ringdown.snr,
				CASE coinc_ringdown.false_alarm_rate WHEN 0 THEN "inf" ELSE 1.0 / coinc_ringdown.false_alarm_rate END,
				coinc_event.likelihood
			)
		FROM
			coinc_event
		WHERE
			coinc_event.coinc_event_id == coinc_ringdown.coinc_event_id
	)
	""" % options.populate_column)
	connection.commit()

#
	# close the database
	#

	connection.close()
	dbtables.put_connection_filename(filename, working_filename, verbose = options.verbose)
