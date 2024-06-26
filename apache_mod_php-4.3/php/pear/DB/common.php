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
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// Base class for DB implementations.
//

/**
 * DB_common is a base class for DB implementations, and should be
 * inherited by all such.
 */
class DB_common {
    // {{{ properties

	var $features;		// assoc of capabilities for this DB implementation
	var $errorcode_map;	// assoc mapping native error codes to DB ones
	var $type;			// DB type (mysql, oci8, odbc etc.)
	var $prepare_tokens;
	var $prepare_types;
	var $prepare_maxstmt;
	var $error_mode = PEAR_ERROR_RETURN;
	var $error_level = E_USER_NOTICE;
	var $error_callback;
	var $last_query = '';
	var $fetchmode = DB_FETCHMODE_DEFAULT;

    // }}}
	// {{{ toString()

	function toString() {
		$info = get_class($this);
		$info .=
			": (phptype=" . $this->phptype .
			", dbsyntax=" . $this->dbsyntax .
			")";
		if ($this->connection) {
			$info .= " [connected]";
		}
		return $info;
	}

	// }}}
    // {{{ constructor

	function DB_common() {
		$this->features = array();
		$this->errorcode_map = array();
		$this->fetchmode = DB_FETCHMODE_ORDERED;
	}

    // }}}
    // {{{ quoteString()

	/**
	 * Quotes a string so it can be safely used within string delimiters
	 * in a query.
	 *
	 * @param $string the input string to quote
	 *
	 * @return string the quoted string
	 */
	function quoteString($string) {
		return str_replace("'", "\'", $string);
	}

	// }}}
	// {{{ provides()

	/**
	 * Tell whether a DB implementation or its backend extension
	 * supports a given feature.
	 *
	 * @param $feature name of the feature (see the DB class doc)
	 *
	 * @return bool whether this DB implementation supports $feature
	 */
	function provides($feature) {
		return $this->features[$feature];
	}

	// }}}
	// {{{ errorCode()

	/**
	 * Map native error codes to DB's portable ones.  Requires that
	 * the DB implementation's constructor fills in the $errorcode_map
	 * property.
	 *
	 * @param $nativecode the native error code, as returned by the backend
	 * database extension (string or integer)
	 *
	 * @return int a portable DB error code, or FALSE if this DB
	 * implementation has no mapping for the given error code.
	 */
	function errorCode($nativecode) {
		if ($this->errorcode_map[$nativecode]) {
			return $this->errorcode_map[$nativecode];
		}
		//php_error(E_WARNING, get_class($this)."::errorCode: no mapping for $nativecode");
		// Fall back to DB_ERROR if there was no mapping.  Ideally,
		// this should never happen.
		return $this->raiseError(DB_ERROR, false, false, false, $nativecode);
	}

	// }}}
	// {{{ errorMessage()

	/**
	 * Map a DB error code to a textual message.  This is actually
	 * just a wrapper for DB::errorMessage().
	 *
	 * @param $dbcode the DB error code
	 *
	 * @return string the corresponding error message, of FALSE
	 * if the error code was unknown
	 */
	function errorMessage($dbcode) {
		return DB::errorMessage($this->errorcode_map[$dbcode]);
	}

	// }}}
    // {{{ raiseError()

	/**
	 * This method is called by DB to generate an error.
	 *
	 */
	function &raiseError($code = DB_ERROR, $mode = false, $level = false,
						 $debuginfo = false, $nativecode = false) {
		if (!$mode) {
			$mode = $this->error_mode;
		}
		if ($mode == PEAR_ERROR_CALLBACK) {
			if (!is_string($level) &&
				!(is_array($level) && sizeof($level) == 2 &&
				  is_object($level[0]) && is_string($level[1]))) {
				$level = $this->error_callback;
			}
		} else {
			if (!$level) {
				$level = $this->error_level;
			}
		}
		if (!$debuginfo) {
			$debuginfo = $this->last_query;
		}
		if ($nativecode) {
			$debuginfo .= " [nativecode=$nativecode]";
		}
		return new DB_Error($code, $mode, $level, $debuginfo);
	}

    // }}}
    // {{{ setErrorHandling()

	/**
	 * Sets how errors generated by this DB object should be handled.
	 *
	 * @param $mode int
	 *        one of PEAR_ERROR_RETURN, PEAR_ERROR_PRINT,
	 *        PEAR_ERROR_TRIGGER, PEAR_ERROR_DIE or
	 *        PEAR_ERROR_CALLBACK.
	 *
	 * @param $options mixed
	 *        Ignored unless $mode is PEAR_ERROR_TRIGGER or
	 *        PEAR_ERROR_CALLBACK.  When $mode is PEAR_ERROR_TRIGGER,
	 *        this parameter is expected to be an integer among
	 *        E_USER_NOTICE, E_USER_WARNING or E_USER_ERROR.  When
	 *        $mode is PEAR_ERROR_CALLBACK, this parameter is expected
	 *        to be the callback function or method.  A callback
	 *        function is a string with the name of the function, a
	 *        callback method is an array of two elements: the element
	 *        at index 0 is an object, and the element at index 1 is
	 *        the name of the method to call in the object.
	 *
	 * @see PEAR_ERROR_RETURN
	 * @see PEAR_ERROR_PRINT
	 * @see PEAR_ERROR_TRIGGER
	 * @see PEAR_ERROR_DIE
	 * @see PEAR_ERROR_CALLBACK
	 */
	function setErrorHandling($mode, $options = false) {
		switch ($mode) {
			case PEAR_ERROR_RETURN:
			case PEAR_ERROR_PRINT:
			case PEAR_ERROR_TRIGGER:
			case PEAR_ERROR_DIE:
				$this->error_mode = $mode;
				if (!$options) {
					$this->error_level = E_USER_NOTICE;
				} else {
					$this->error_level = $options;
				}
				break;
			case PEAR_ERROR_CALLBACK:
				$this->error_mode = $mode;
				if (is_string($options) ||
					(is_array($options) && sizeof($options) == 2 &&
					 is_object($options[0]) && is_string($options[1]))) {
					$this->error_callback = $options;
				} else {
					trigger_error(E_USER_WARNING, "invalid error callback");
				}
				$this->error_level = PEAR_ERROR_RETURN;
				break;
			default:
				trigger_error(E_USER_WARNING, "invalid error mode");
				break;
		}
	}

    // }}}
    // {{{ setFetchMode()

	/**
	 * Sets which fetch mode should be used by default on queries
	 * on this connection.
	 *
	 * @param $fetchmode int DB_FETCHMODE_ORDERED or
	 *        DB_FETCHMODE_ASSOC, possibly bit-wise OR'ed with
	 *        DB_FETCHMODE_FLIPPED.
	 *
	 * @see DB_FETCHMODE_ORDERED
	 * @see DB_FETCHMODE_ASSOC
	 * @see DB_FETCHMODE_FLIPPED
	 */
	function setFetchMode($fetchmode) {
		switch ($fetchmode) {
			case DB_FETCHMODE_ORDERED:
			case DB_FETCHMODE_ASSOC:
				$this->fetchmode = $fetchmode;
				break;
			default:
				return $this->raiseError("invalid get mode");
		}
	}

    // }}}
    // {{{ prepare()

	/**
	 * Prepares a query for multiple execution with execute().  With
	 * PostgreSQL, this is emulated.
	 */
	function prepare($query) {
		$tokens = split('[\&\?]', $query);
		$token = 0;
		$types = array();
		for ($i = 0; $i < strlen($query); $i++) {
			switch ($query[$i]) {
				case '?':
					$types[$token++] = DB_PARAM_SCALAR;
					break;
				case '&':
					$types[$token++] = DB_PARAM_OPAQUE;
					break;
			}
		}
		$this->prepare_tokens[] = &$tokens;
		end($this->prepare_tokens);
		$k = key($this->prepare_tokens);
		$this->prepare_types[$k] = $types;
		return $k;
	}

    // }}}
    // {{{ execute_emulate_query()

	/**
	 * @return a string containing the real query run when emulating
	 * prepare/execute.  A DB error code is returned on failure.
	 */
	function execute_emulate_query($stmt, $data = false) {
		$p = &$this->prepare_tokens;
		$stmt = (int)$this->prepare_maxstmt++;
		if (!isset($this->prepare_tokens[$stmt]) ||
			!is_array($this->prepare_tokens[$stmt]) ||
			!sizeof($this->prepare_tokens[$stmt])) {
			return $this->raiseError(DB_ERROR_INVALID);
		}
		$qq = &$this->prepare_tokens[$stmt];
		$qp = sizeof($qq) - 1;
		if ((!$data && $qp > 0) ||
			(!is_array($data) && $qp > 1) ||
			(is_array($data) && $qp > sizeof($data))) {
			return $this->raiseError(DB_ERROR_NEED_MORE_DATA);
		}
		$realquery = $qq[0];
		for ($i = 0; $i < $qp; $i++) {
			if ($this->prepare_types[$stmt][$i] == DB_PARAM_OPAQUE) {
				if (is_array($data)) {
					$fp = fopen($data[$i], "r");
				} else {
					$fp = fopen($data, "r");
				}
				$pdata = '';
				if ($fp) {
					while (($buf = fread($fp, 4096)) != false) {
						$pdata .= $buf;
					}
				}
			} else {
				if (is_array($data)) {
					$pdata = &$data[$i];
				} else {
					$pdata = &$data;
				}
			}
			$realquery .= "'" . $this->quoteString($pdata) . "'";
			$realquery .= $qq[$i + 1];
		}
		return $realquery;
	}

    // }}}

    // {{{ executeMultiple()

	/**
	 * This function does several execute() calls on the same
	 * statement handle.  $data must be an array indexed numerically
	 * from 0, one execute call is done for every "row" in the array.
	 *
	 * If an error occurs during execute(), executeMultiple() does not
	 * execute the unfinished rows, but rather returns that error.
	 */
	function executeMultiple($stmt, &$data) {
		for ($i = 0; $i < sizeof($data); $i++) {
			$res = $this->execute($stmt, $data[$i]);
			if (DB::isError($res)) {
				return $res;
			}
		}
		return DB_OK;
	}

    // }}}
    // {{{ getOne()

	/**
	 * Fetch the first column of the first row of data returned from
	 * a query.  Takes care of doing the query and freeing the results
	 * when finished.
	 *
	 * @param $query the SQL query
	 * @param $params if supplied, prepare/execute will be used
	 *        with this array as execute parameters
	 * @access public
	 */
	function &getOne($query, $params = array()) {
		if (sizeof($params) > 0) {
			$sth = $this->prepare($query);
			if (DB::isError($sth)) {
				return $sth;
			}
			$res = $this->execute($sth, $params);
		} else {
			$res = $this->simpleQuery($query);
		}
		if (DB::isError($res)) {
			return $res;
		}
		$row = $this->fetchRow($res, DB_FETCHMODE_ORDERED);
		if (DB::isError($row)) {
			return $row;
		}
		$ret = &$row[0];
		$this->freeResult($res);
		if (isset($sth)) {
			$this->freeResult($sth);
		}
		return $ret;
	}

    // }}}
    // {{{ getRow()

	/**
	 * Fetch the first row of data returned from a query.  Takes care
	 * of doing the query and freeing the results when finished.
	 *
	 * @param $query the SQL query
	 * @access public
	 * @return array the first row of results as an array indexed from
	 * 0, or a DB error code.
	 */
	function &getRow($query, $fetchmode = DB_FETCHMODE_DEFAULT, $params = array()) {
		if ($fetchmode == DB_FETCHMODE_DEFAULT) {
			$fetchmode = $this->fetchmode;
		}
		if (sizeof($params) > 0) {
			$sth = $this->prepare($query);
			if (DB::isError($sth)) {
				return $sth;
			}
			$res = $this->execute($sth, $params);
		} else {
			$res = $this->simpleQuery($query);
		}
		if (DB::isError($res)) {
			return $res;
		}
		$row = $this->fetchRow($res, $fetchmode);
		if (DB::isError($row)) {
			return $row;
		}
		$this->freeResult($res);
		if (isset($sth)) {
			$this->freeResult($sth);
		}
		return $row;
	}

    // }}}
    // {{{ getCol()

	/**
	 * Fetch a single column from a result set and return it as an
	 * indexed array.
	 *
	 * @param $query the SQL query
	 *
	 * @param $col which column to return (integer [column number,
	 * starting at 0] or string [column name])
	 *
	 * @access public
	 *
	 * @return array an indexed array with the data from the first
	 * row at index 0, or a DB error code.
	 */
	function &getCol($query, $col = 0, $params = array()) {
		if (sizeof($params) > 0) {
			$sth = $this->prepare($query);
			if (DB::isError($sth)) {
				return $sth;
			}
			$res = $this->execute($sth, $params);
		} else {
			$res = $this->simpleQuery($query);
		}
		if (DB::isError($res)) {
			return $res;
		}
		$fetchmode = is_int($col) ? DB_FETCHMODE_ORDERED : DB_FETCHMODE_ASSOC;
		$ret = array();
		while ($row = $this->fetchRow($res, $fetchmode)) {
			if (DB::isError($row)) {
				$ret = $row;
				break;
			}
			$ret[] = $row[$col];
		}
		$this->freeResult($res);
		if (isset($sth)) {
			$this->freeResult($sth);
		}
		return $ret;
	}

    // }}}
    // {{{ getAssoc()

	/**
	 * Fetch the entire result set of a query and return it as an
	 * associative array using the first column as the key.
	 *
	 * @param $query the SQL query
	 *
	 * @param $force_array (optional) used only when the query returns
	 * exactly two columns.  If true, the values of the returned array
	 * will be one-element arrays instead of scalars.
	 *
	 * @access public
	 *
	 * @return array associative array with results from the query.
	 * If the result set contains more than two columns, the value
	 * will be an array of the values from column 2-n.  If the result
	 * set contains only two columns, the returned value will be a
	 * scalar with the value of the second column (unless forced to an
	 * array with the $force_array parameter).  A DB error code is
	 * returned on errors.  If the result set contains fewer than two
	 * columns, a DB_ERROR_TRUNCATED error is returned.
	 *
	 * For example, if the table "mytable" contains:
	 *
	 *  ID      TEXT       DATE
	 * --------------------------------
	 *  1       'one'      944679408
	 *  2       'two'      944679408
	 *  3       'three'    944679408
	 *
	 * Then the call getAssoc('SELECT id,text FROM mytable') returns:
	 *   array(
	 *     '1' => 'one',
	 *     '2' => 'two',
	 *     '3' => 'three',
	 *   )
	 *
	 * ...while the call getAssoc('SELECT id,text,date FROM mydate') returns:
	 *   array(
	 *     '1' => array('one', '944679408'),
	 *     '2' => array('two', '944679408'),
	 *     '3' => array('three', '944679408')
	 *   )
	 *
	 * Keep in mind that database functions in PHP usually return string
	 * values for results regardless of the database's internal type.
	 */
	function &getAssoc($query, $force_array = false, $params = array()) {
		if (sizeof($params) > 0) {
			$sth = $this->prepare($query);
			if (DB::isError($sth)) {
				return $sth;
			}
			$res = $this->execute($sth, $params);
		} else {
			$res = $this->simpleQuery($query);
		}
		if (DB::isError($res)) {
			return $res;
		}
		$cols = $this->numCols($res);
		if ($cols < 2) {
			return $this->raiseError(DB_ERROR_TRUNCATED);
		}
		$results = array();
		if ($cols > 2 || $force_array) {
			// return array values
			// XXX this part can be optimized
			while (($row = $this->fetchRow($res, DB_FETCHMODE_ORDERED))
				   && !DB::isError($row)) {
				reset($row);
				// we copy the row of data into a new array
				// to get indices running from 0 again
				$results[$row[0]] = array_slice($row, 1);
			}
		} else {
			// return scalar values
			while (($row = $this->fetchRow($res)) && !DB::isError($row)) {
				$results[$row[0]] = $row[1];
			}
		}
		$this->freeResult($res);
		if (isset($sth)) {
			$this->freeResult($sth);
		}
		return $results;
	}

    // }}}
	// {{{ getAll()

	/**
	 * Fetch all the rows returned from a query.
	 *
	 * @param $query the SQL query
	 * @access public
	 * @return array an nested array, or a DB error
	 */
	function &getAll($query, $fetchmode = DB_FETCHMODE_DEFAULT, $params = array()) {
		if ($fetchmode == DB_FETCHMODE_DEFAULT) {
			$fetchmode = $this->fetchmode;
		}
		if (sizeof($params) > 0) {
			$sth = $this->prepare($query);
			if (DB::isError($sth)) {
				return $sth;
			}
			$res = $this->execute($sth, $params);
		} else {
			$res = $this->simpleQuery($query);
		}
		if (DB::isError($res)) {
			return $res;
		}
		$results = array();
		while (($row = $this->fetchRow($res, $fetchmode)) && !DB::isError($row)) {
			if ($fetchmode & DB_FETCHMODE_FLIPPED) {
				foreach ($row as $key => $val) {
					$results[$key][] = $val;
				}
			} else {
				$results[] = $row;
			}
		}
		$this->freeResult($res);
		if (isset($sth)) {
			$this->freeResult($sth);
		}
		return $results;
	}

	// }}}
}

// Local variables:
// tab-width: 4
// c-basic-offset: 4
// End:
?>
