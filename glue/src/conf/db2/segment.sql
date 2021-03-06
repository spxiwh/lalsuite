-- This file is part of the Grid LSC User Environment (GLUE)
-- 
-- GLUE is free software: you can redistribute it and/or modify it under the
-- terms of the GNU General Public License as published by the Free Software
-- Foundation, either version 3 of the License, or (at your option) any later
-- version.
-- 
-- This program is distributed in the hope that it will be useful, but WITHOUT
-- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
-- FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
-- details.
-- 
-- You should have received a copy of the GNU General Public License along
-- with this program.  If not, see <http://www.gnu.org/licenses/>.

CREATE TABLE segment
(
-- A "segment" is a time interval which is meaningful for some reason.  For
-- example, it may indicate a period during which an interferometer is locked.

-- Database which created this entry
      creator_db         INTEGER NOT NULL WITH DEFAULT 1,

-- Unique process ID of the process which defined this segment
      process_id         CHAR(13) FOR BIT DATA NOT NULL,

-- Unique segment ID
      segment_id         CHAR(13) FOR BIT DATA NOT NULL,

-- INFORMATION ABOUT THIS SEGMENT
-- Segment start and end times, in GPS seconds.
      start_time         INTEGER NOT NULL,
      start_time_ns      INTEGER,
      end_time           INTEGER NOT NULL,
      end_time_ns        INTEGER,

-- Activity bit for segment. If zero then this time has been analyzed but
-- the segment should not be applied. If positive then this segment should
-- be added to the list of segments (by a union), if negative then this
-- segment should be removed from the list of segments (by a negation and an
-- intersection).
      active             INTEGER NOT NULL,

-- Science segment identification number. If this is a science segment 
-- created by the online segment publish script, this should contain the
-- valued stored in the channel IFO-SV_SEGNUM, otherwise it should be NULL
      segnum             INTEGER,

-- Insertion time (automatically assigned by the database)
      insertion_time     TIMESTAMP WITH DEFAULT CURRENT TIMESTAMP,

      CONSTRAINT segment_pk
      PRIMARY KEY (segment_id, creator_db),

      CONSTRAINT segment_fk_pid
      FOREIGN KEY (process_id, creator_db)
          REFERENCES process(process_id, creator_db)
          ON DELETE CASCADE
)
-- The following line is needed for this table to be replicated to other sites
DATA CAPTURE CHANGES
;
-- Create an index based on time
CREATE INDEX segment_ind_time ON segment(start_time,start_time_ns,end_time,end_time_ns)
;
CREATE INDEX segment_ind_stime ON segment(start_time)
;
CREATE INDEX segment_ind_etime ON segment(end_time)
;
-- Create an index based on segment number
CREATE INDEX segment_ind_segnum ON segment(segnum)
;
