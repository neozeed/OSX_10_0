<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Sebastian Bergmann <sb@phpOpenTracker.de>                   |
// +----------------------------------------------------------------------+
//
// $Id: Timer.php,v 1.1.1.1 2001/01/25 05:00:28 wsanchez Exp $
//

  /**
  * Benchmark::Timer
  * 
  * Purpose:
  * 
  *   Timing Script Execution, Generating Profiling Information
  * 
  * Example:
  * 
  *   $timer = new Benchmark_Timer;
  * 
  *   $timer->start();
  *   $timer->set_marker( "Marker 1" );
  *   $timer->stop();
  * 
  *   $profiling = $timer->get_profiling();
  * 
  * @author   Sebastian Bergmann <sb@phpOpenTracker.de>
  * @version  $Revision: 1.1.1.1 $
  * @access   public
  */

  class Benchmark_Timer
  {
    // {{{ properties

    /**
    * Contains the markers
    *
    * @var    array
    * @access public
    */

    var $markers = array();
    
    // }}}
    // {{{ start()

    /**
    * Set "Start" marker.
    *
    * @see    set_marker(), stop()
    * @access public
    */

    function start()
    {
      $this->set_marker( "Start" );
    }

    // }}}
    // {{{ stop()

    /**
    * Set "Stop" marker.
    *
    * @see    set_marker(), start()
    * @access public
    */

    function stop()
    {
      $this->set_marker( "Stop" );
    }

    // }}}
    // {{{ set_marker()

    /**
    * Set marker.
    *
    * @param  string  name of the marker to be set
    * @see    start(), stop()
    * @access public
    */

    function set_marker( $name )
    {
      $microtime = explode( " ", microtime() );
      $this->markers[ $name ] = $microtime[ 1 ] . substr( $microtime[ 0 ], 1 );
    }

    // }}}
    // {{{ time_elapsed()

    /**
    * Returns the time elapsed betweens two markers.
    *
    * @param  string  $start        start marker, defaults to "Start"
    * @param  string  $end          end marker, defaults to "Stop"
    * @return double  $time_elapsed time elapsed between $start and $end
    * @access public
    */

    function time_elapsed( $start = "Start", $end = "Stop" )
    {
      return bcsub( $this->markers[ $end ], $this->markers[ $start ], 6 );
    }
    
    // }}}
    // {{{ get_profiling()

    /**
    * Returns profiling information.
    *
    * $profiling[ x ][ "name"  ] = name of marker x
    * $profiling[ x ][ "time"  ] = time index of marker x
    * $profiling[ x ][ "diff"  ] = execution time from marker x-1 to this marker x
    * $profiling[ x ][ "total" ] = total execution time up to marker x
    *
    * @return array $profiling
    * @access public
    */

    function get_profiling()
    {
      $i = 0;
      $total = 0;
      $result = array();

      while( list( $marker, $time ) = each( $this->markers ) )
      {
        if( $marker == "Start" )
        {
          $diff = "-";
        }

        else
        {
          $diff  = bcsub( $time,  $temp, 6 );
          $total = bcadd( $total, $diff, 6 );
        }

        $result[ $i ][ "name"  ] = $marker;
        $result[ $i ][ "time"  ] = $time;
        $result[ $i ][ "diff"  ] = $diff;
        $result[ $i ][ "total" ] = $total;

        $temp = $time;
        $i++;
      }

      return $result;
    }

    // }}}
  }
?>
