# GDBtk (Insight) entry point
# Copyright 1997, 1998, 1999 Cygnus Solutions
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License (GPL) as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.


# State is controlled by 5 global boolean variables.
#
# gdb_target_changed
# gdb_exe_changed
# gdb_running
# gdb_downloading
# gdb_loaded

################### Initialization code #########################

# If GDBtk fails to start at all, you might want to uncomment one or 
# both of these.
#set tcl_traceExec 2
#set tcl_traceCompile 1

# Add gdb's Tcl library directory to the end of the auto-load search path, if 
# it isn't already on the path:
# Note: GDBTK_LIBRARY will be set in tcl_findLibrary before main.tcl is called.

if {[info exists auto_path]} {
  if {[lsearch -exact $auto_path $GDBTK_LIBRARY] < 0} {
    lappend auto_path $GDBTK_LIBRARY
  }
}

# Require the packages we need.  Most are loaded already, but this will catch 
# any odd errors... :
package require Tcl 8.0
package require Tk 8.0
package require Itcl 3.0
package require Itk 3.0
package require Gdbtk 1.0
package require combobox 1.0

namespace import itcl::*

namespace import debug::*

if {![find_iwidgets_library]} {
  tk_messageBox -title Error -message "Could not find the Iwidgets libraries.
Got nameofexec: [info nameofexecutable]
Error(s) were: \n$errMsg" \
      -icon error -type ok
  exit
}

# Environment variables controlling debugging:
# GDBTK_TRACE
#	unset or 0	no tracing
#	1		tracing initialized but not started
#	2		tracing initialized and started
#
# GDBTK_DEBUGFILE - filename to write debugging messages and
#	trace information (if tracing is enabled).
#
if {[info exists env(GDBTK_TRACE)] && $env(GDBTK_TRACE) != 0} {
  # WARNING: the tracing code must not trace into itself or
  # infinite recursion will result. As currently configured
  # the tracing code will not trace basic tcl functions or anything defined
  # before debug::init.  For this reason we must source the DebugWin
  # code before debug::init is called.
  source [file join $GDBTK_LIBRARY debugwin.ith]
  source [file join $GDBTK_LIBRARY debugwin.itb]

  # Calling this installs our hooks for tracing and profiling.
  # This WILL slow things down.
  ::debug::init

  if {$env(GDBTK_TRACE) == 2} {
    ::debug::trace_start
  }
}

if {[info exists env(GDBTK_DEBUGFILE)]} {
  ::debug::logfile $env(GDBTK_DEBUGFILE)
}

if {$tcl_platform(platform) == "unix"} {
#  tix resetoptions TK TK
#  tk_setPalette tan
  tix resetoptions TixGray [tix cget -fontset]
}

# initialize state variables
initialize_gdbtk

# set traces on state variables
trace variable gdb_running w do_state_hook
trace variable gdb_downloading w do_state_hook
trace variable gdb_loaded w do_state_hook
define_hook state_hook

# set up preferences
pref init

# let libgui tell us how to feel
standard_look_and_feel

# now let GDB set its default preferences
pref_set_defaults

# read in preferences
pref_read

init_disassembly_flavor

ManagedWin::init

# This stuff will help us play nice with WindowMaker's AppIcons.
# Can't do the first bit yet, since we don't get this from gdb...
# wm command . [concat $argv0 $argv] 
wm group . . 

# Open debug window if testsuite is not running and GDBTK_DEBUG is set
if {![info exists env(GDBTK_TEST_RUNNING)] || !$env(GDBTK_TEST_RUNNING)} {
  if {[info exists env(GDBTK_DEBUG)] && $env(GDBTK_DEBUG) > 1} {
    ManagedWin::open DebugWin
  }
}

# some initial commands to get gdb in the right mode
gdb_cmd {set height 0}
gdb_cmd {set width 0}

if {[info exists env(GDBTK_TEST_RUNNING)] && $env(GDBTK_TEST_RUNNING)} {
  set gdb_target_name "exec"
} else {
  # gdb_target_name is the name of the GDB target; that is, the argument
  # to the GDB target command.
  set gdb_target_name ""
  # By setting gdb_target_changed, we force a target dialog
  # to be displayed on the first "run"
  set gdb_target_changed 1
}

update

# Uncomment the next line if you want a splash screen at startup...
# ManagedWin::open About -transient -expire 5000

gdbtk_idle

