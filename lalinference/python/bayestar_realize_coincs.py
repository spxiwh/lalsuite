#
# Copyright (C) 2013  Leo Singer
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
"""
Synthesize triggers for simulated sources by realizing Gaussian measurement
errors in SNR and time of arrival. The input file (or stdin if the input file
is omitted) should be an optionally gzip-compressed LIGO-LW XML file of the form
produced by lalapps_inspinj. The output file (or stdout if omitted) will be an
optionally gzip-compressed LIGO-LW XML file containing single-detector triggers
and coincidences.

The root-mean square measurement error depends on the SNR of the signal, so
there is a choice for how to generate perturbed time and phase measurements:

 - zero-noise: no measurement error at all
 - from-truth: use true, nominal SNR in each detector
 - from-measurement: first perturb SNR with measurement error, then use
   that perturbed SNR to compute covariance of time and phase errors
"""
from __future__ import division
__author__ = "Leo Singer <leo.singer@ligo.org>"


# Determine list of known detectors for command line arguments.
import lal
available_ifos = sorted(det.frDetector.prefix
    for det in lal.CachedDetectors)

# Command line interface.
from optparse import Option, OptionParser
from lalinference.bayestar import command

measurement_error_choices = ("zero-noise", "from-truth", "from-measurement")
parser = OptionParser(
    formatter = command.NewlinePreservingHelpFormatter(),
    description = __doc__,
    usage="%prog [options] [INPUT.xml[.gz]]",
    option_list = [
        Option("-o", "--output", metavar="OUTPUT.xml[.gz]",
            default="/dev/stdout",
            help="Name of output file [default: %default]"),
        Option("--detector", metavar='|'.join(available_ifos), action="append",
            help="Detectors to use.  May be specified multiple times.",
            choices=available_ifos),
        Option("--waveform",
            help="Waveform to use for injections (overrides values in "
            "sim_inspiral table)"),
        Option("--snr-threshold", type=float, default=4.,
            help="Single-detector SNR threshold [default: %default]"),
        Option("--min-triggers", type=int, default=2,
            help="Emit coincidences only when at least this many triggers "
            "are found [default: %default]"),
        Option("--measurement-error", choices=measurement_error_choices,
            default=measurement_error_choices[0],
            metavar='|'.join(measurement_error_choices),
            help="How to compute the measurement error [default: %default]"),
        Option("--reference-psd", metavar="PSD.xml[.gz]",
            help="Name of PSD file (required)"),
        Option("--f-low", type=float,
            help="Override low frequency cutoff found in sim_inspiral table.")
    ]
)
opts, args = parser.parse_args()
infilename = command.get_input_filename(parser, args)
command.check_required_arguments(parser, opts, 'reference_psd')


# Python standard library imports.
import os

# LIGO-LW XML imports.
from glue.ligolw import ligolw
from glue.ligolw.utils import process as ligolw_process
from glue.ligolw.utils import search_summary as ligolw_search_summary
from glue.ligolw import table as ligolw_table
from pylal import ligolw_thinca
from glue.ligolw import utils as ligolw_utils
from glue.ligolw import lsctables

# glue, LAL and pylal imports.
from glue import segments
import glue.lal
import lal
import lal.series
import lalsimulation
import pylal.progress

# BAYESTAR imports.
from lalinference.bayestar import ligolw as ligolw_bayestar
from lalinference.bayestar import filter
from lalinference.bayestar import timing

# Other imports.
import numpy as np


progress = pylal.progress.ProgressBar()

# Open output file.
progress.update(-1, 'setting up output document')
out_xmldoc = ligolw.Document()
out_xmldoc.appendChild(ligolw.LIGO_LW())

# Write process metadata to output file.
process = ligolw_process.register_to_xmldoc(out_xmldoc, parser.get_prog_name(),
    opts.__dict__, ifos=opts.detector, comment="Simulated coincidences")

# Add search summary to output file.
all_time = segments.segment(
    [glue.lal.LIGOTimeGPS(0), glue.lal.LIGOTimeGPS(2e9)])
search_summary_table = lsctables.New(lsctables.SearchSummaryTable)
out_xmldoc.childNodes[0].appendChild(search_summary_table)
summary = ligolw_search_summary.append_search_summary(out_xmldoc, process,
    inseg=all_time, outseg=all_time)

# Read PSDs.
progress.update(-1, 'reading ' + opts.reference_psd)
xmldoc = ligolw_utils.load_filename(
    opts.reference_psd, contenthandler=lal.series.PSDContentHandler)
psds = lal.series.read_psd_xmldoc(xmldoc)
psds = dict(
    (key, timing.InterpolatedPSD(filter.abscissa(psd), psd.data.data))
    for key, psd in psds.iteritems() if psd is not None)

# Read injection file.
progress.update(-1, 'reading ' + infilename)
xmldoc = ligolw_utils.load_filename(
    infilename, contenthandler=ligolw_bayestar.LSCTablesContentHandler)

# Extract simulation table from injection file.
sim_inspiral_table = ligolw_table.get_table(xmldoc,
    lsctables.SimInspiralTable.tableName)

# Create a SnglInspiral table and initialize its row ID counter.
sngl_inspiral_table = lsctables.New(lsctables.SnglInspiralTable)
out_xmldoc.childNodes[0].appendChild(sngl_inspiral_table)
sngl_inspiral_table.set_next_id(lsctables.SnglInspiralID(0))

# Create a time slide entry.  Needed for coinc_event rows.
time_slide_table = lsctables.New(lsctables.TimeSlideTable)
out_xmldoc.childNodes[0].appendChild(time_slide_table)
time_slide_id = time_slide_table.get_time_slide_id(
    dict((ifo, 0) for ifo in opts.detector), create_new=process)

# Create a CoincDef table and record a CoincDef row for
# sngl_inspiral <-> sngl_inspiral coincidences.
coinc_def_table = lsctables.New(lsctables.CoincDefTable)
out_xmldoc.childNodes[0].appendChild(coinc_def_table)
coinc_def = ligolw_thinca.InspiralCoincDef
coinc_def_id = coinc_def_table.get_next_id()
coinc_def.coinc_def_id = coinc_def_id
coinc_def_table.append(coinc_def)

# Create a CoincMap table.
coinc_map_table = lsctables.New(lsctables.CoincMapTable)
out_xmldoc.childNodes[0].appendChild(coinc_map_table)

# Create a CoincEvent table.
coinc_table = lsctables.New(lsctables.CoincTable)
out_xmldoc.childNodes[0].appendChild(coinc_table)

# Precompute values that are common to all simulations.
detectors = [lalsimulation.DetectorPrefixToLALDetector(ifo)
    for ifo in opts.detector]
responses = [det.response for det in detectors]
locations = [det.location for det in detectors]

for sim_inspiral in progress.iterate(sim_inspiral_table):

    # Unpack some values from the row in the table.
    m1 = sim_inspiral.mass1
    m2 = sim_inspiral.mass2
    f_low = sim_inspiral.f_lower if opts.f_low is None else opts.f_low
    DL = sim_inspiral.distance
    ra = sim_inspiral.longitude
    dec = sim_inspiral.latitude
    inc = sim_inspiral.inclination
    phi = sim_inspiral.coa_phase
    psi = sim_inspiral.polarization
    epoch = lal.LIGOTimeGPS(
        sim_inspiral.geocent_end_time, sim_inspiral.geocent_end_time_ns)
    gmst = lal.GreenwichMeanSiderealTime(epoch)
    waveform = sim_inspiral.waveform if opts.waveform is None else opts.waveform
    approximant, amplitude_order, phase_order = \
        timing.get_approximant_and_orders_from_string(waveform)

    # Pre-evaluate some trigonometric functions that we will need.
    u = np.cos(inc)
    u2 = np.square(u)

    # Signal models for each detector.
    signal_models = [
        timing.SignalModel(m1, m2, psds[ifo], f_low,
        approximant, amplitude_order, phase_order)
        for ifo in opts.detector]

    # Get SNR=1 horizon distances for each detector.
    horizons = np.asarray([signal_model.get_horizon_distance()
        for signal_model in signal_models])

    # Get antenna factors for each detector.
    Fplus, Fcross = np.asarray([
        lal.ComputeDetAMResponse(response, ra, dec, psi, gmst)
        for response in responses]).T

    # Compute TOAs at each detector.
    toas = np.asarray([lal.TimeDelayFromEarthCenter(location, ra, dec,
        epoch) for location in locations])

    # Compute SNR in each detector.
    snrs = (0.5 * (1 + u2) * Fplus + 1j * u * Fcross) * horizons / DL

    abs_snrs = np.abs(snrs)
    arg_snrs = np.angle(snrs)

    if opts.measurement_error == 'zero-noise':
        pass
    elif opts.measurement_error == 'from-truth':
        # If user asked, apply noise to amplitudes /before/ adding noise to TOAs and phases.

        # Add noise to SNR estimates.
        abs_snrs += np.random.randn(len(abs_snrs))

        for i, signal_model in enumerate(signal_models):
            arg_snrs[i], toas[i]  = np.random.multivariate_normal(
                [arg_snrs[i], toas[i]], signal_model.get_cov(abs_snrs[i]))
    elif opts.measurement_error == 'from-measurement':
        # Otherwise, by defualt, apply noise to TOAs and phases first.

        for i, signal_model in enumerate(signal_models):
            arg_snrs[i], toas[i]  = np.random.multivariate_normal(
                [arg_snrs[i], toas[i]], signal_model.get_cov(abs_snrs[i]))

        # Add noise to SNR estimates.
        abs_snrs += np.random.randn(len(abs_snrs))
    else:
        raise RuntimeError("This code should not be reached.")

    sngl_inspirals = []

    # Loop over individual detectors and create SnglInspiral entries.
    for ifo, abs_snr, arg_snr, toa, horizon in zip(
            opts.detector, abs_snrs, arg_snrs, toas, horizons):

        # If SNR < threshold, then the injection is not found. Skip it.
        if abs_snr < opts.snr_threshold:
            continue

        # Create SnglInspiral entry.
        sngl_inspiral = lsctables.SnglInspiral()
        for validcolumn in sngl_inspiral_table.validcolumns.iterkeys():
            setattr(sngl_inspiral, validcolumn, None)
        sngl_inspiral.process_id = process.process_id
        sngl_inspiral.ifo = ifo
        sngl_inspiral.mass1 = m1
        sngl_inspiral.mass2 = m2
        sngl_inspiral.end_time = (epoch + toa).gpsSeconds
        sngl_inspiral.end_time_ns = (epoch + toa).gpsNanoSeconds
        sngl_inspiral.snr = abs_snr
        sngl_inspiral.coa_phase = np.angle(np.exp(1j * arg_snr))
        sngl_inspiral.eff_distance = horizon / sngl_inspiral.snr
        sngl_inspirals.append(sngl_inspiral)

    # If too few triggers were found, then skip this event.
    if len(sngl_inspirals) < opts.min_triggers:
        continue

    # Add Coinc table entry.
    coinc = lsctables.Coinc()
    coinc.coinc_event_id = coinc_table.get_next_id()
    coinc.process_id = process.process_id
    coinc.coinc_def_id = coinc_def_id
    coinc.time_slide_id = time_slide_id
    coinc.set_instruments(opts.detector)
    coinc.nevents = len(opts.detector)
    coinc.likelihood = None
    coinc_table.append(coinc)

    # Record all sngl_inspiral records and associate them with coincidences.
    for sngl_inspiral in sngl_inspirals:
        # Give this sngl_inspiral record an id and add it to the table.
        sngl_inspiral.event_id = sngl_inspiral_table.get_next_id()
        sngl_inspiral_table.append(sngl_inspiral)

        # Add CoincMap entry.
        coinc_map = lsctables.CoincMap()
        coinc_map.coinc_event_id = coinc.coinc_event_id
        coinc_map.table_name = sngl_inspiral_table.tableName
        coinc_map.event_id = sngl_inspiral.event_id
        coinc_map_table.append(coinc_map)


# Record process end time.
progress.update(-1, 'writing ' + opts.output)
ligolw_process.set_process_end_time(process)

# Write output file.
ligolw_utils.write_filename(out_xmldoc, opts.output,
    gz=(os.path.splitext(opts.output)[-1]==".gz"))
