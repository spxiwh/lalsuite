# Copyright (C) 2016 Ian Harry
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# the knowledge that it will not be of any use whatsoever. It is distributed
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

"""
The code reads in a set of compressed template banks and combines them into a
single template bank.
"""

import argparse
import numpy
import h5py
import logging
__author__  = "Ian Harry <ian.harry@ligo.org>"
__program__ = "lalapps_cbc_sbank_hdf5_bankcombiner"

parser = argparse.ArgumentParser(description=__doc__[1:])
parser.add_argument("--output-file", type=str,
                    help="Output hdf bank file.")
parser.add_argument("--input-filenames", nargs='*', default=None,
                    action="store",
                    help="List of input hdf bank files.")
parser.add_argument("--verbose", action="store_true", default=False)

args = parser.parse_args()

attrs_dict = None
items_dict = None
curr_waveforms = None

out_fp = h5py.File(args.output_file, 'w')

for file_name in args.input_filenames:
    hdf_fp = h5py.File(file_name, 'r')
    if 'empty_file' in hdf_fp.attrs:
        continue
    if attrs_dict is None:
        attrs_dict = {}
        for key, item in hdf_fp.attrs.items():
            attrs_dict[key] = item
    if items_dict is None:
        items_dict = {}
        for item, entries in hdf_fp.items():
            if not item == 'waveforms':
                items_dict[item] = entries[:]
    else:
        curr_items = set(items_dict.keys())
        new_items = set(hdf_fp.keys())
        new_items -= set(['waveforms'])
        # This does the XOR check of the two sets of keys.
        # Basically we demand that the two files must have the same items.
        if set(curr_items).symmetric_difference(new_items):
            err_msg = "All input files must contain the same data structures. "
            err_msg += "File {} ".format(file_name)
            err_msg += "contains fields {} ".format(new_items)
            err_msg += "other files contain {}.".format(curr_items)
            raise ValueError(err_msg)
        for item, entries in hdf_fp.items():
            if not item == 'waveforms':
                items_dict[item] = numpy.append(items_dict[item], entries[:])
    if 'waveforms' in hdf_fp.keys():
        hdf_fp_wd = hdf_fp['waveforms']
        # If curr_waveforms is None, this is the first file and we just
        # copy the entire waveform group. This is much quicker than the
        # checks on subsequent files, so if adding a small set of waveforms
        # to a large file, make sure the large file is given *first*!
        if curr_waveforms is None:
            h5py.h5o.copy(hdf_fp.id, 'waveforms', out_fp.id, 'waveforms')
            #hdf_fp.copy('waveforms', out_fp)
            curr_waveforms = set(hdf_fp_wd.keys())
            out_fp_wf = out_fp['waveforms']
        else:
            # Now we need to check each waveform, this is slow!
            new_waveforms = hdf_fp_wd.keys()
            for wf in new_waveforms:
                if wf not in curr_waveforms:
                    h5py.h5o.copy(hdf_fp_wd.id, wf, out_fp_wf.id, wf)
                    #hdf_fp.copy('waveforms/'+wf, out_fp['waveforms'])
                    curr_waveforms.add(wf)
                else:
                    old_df = out_fp_wf[wf].attrs['waveform_df']
                    new_df = hdf_fp_wd[wf].attrs['waveform_df']
                    if new_df < old_df:
                        del out_fp_wf[wf]
                        hdf_fp.copy('waveforms/'+wf, out_fp['waveforms'])
                        #out_fp.flush()
                    
    hdf_fp.close()

if attrs_dict is None:
    out_fp.attrs['empty_file'] = True
else:
    for item, value in items_dict.items():
        out_fp[item] = value
    for item, value in attrs_dict.items():
        out_fp.attrs[item] = value
out_fp.close()
