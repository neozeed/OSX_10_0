<?php
/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Bakken <ssb@fast.no>                                   |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

ob_implicit_flush();

define('TEST_PASSED', 0);
define('TEST_FAILED', -1);
define('TEST_SKIPPED', -2);
define('TEST_INTERNAL_ERROR', -3);

initialize();
/*
$opts = parse_options(&$argc, &$argv);
if ($opts['help']) {
    usage();
    exit;
}
*/
do_testing($argc, $argv);

exit;

/*****************************************************************************/

function usage() {
    writeln("Usage: run-tests.php [-d] [-h] [dir|file...]");
}

/*
 * Please use write() and writeln() for all screen output.
 * This makes it easier to convert to HTML output later.
 */

function write($str) {
    global $term_bold, $term_norm;
    $str = str_replace("%b", $term_bold, $str);
    $str = str_replace("%B", $term_norm, $str);
    print $str;
}

function writeln($str) {
    write("$str\n");
}

function initialize() {
    global $term, $windows_p, $php, $skip, $testdirs, $tmpfile,
	$skipped, $failed, $passed, $total, $term_bold, $term_norm,
	$tests_in_dir;

    // XXX Should support HTML output as well.
    $term = getenv("TERM");
    if (preg_match('/^(xterm|vt220)/', $term)) {
	$term_bold = sprintf("%c%c%c%c", 27, 91, 49, 109);
	$term_norm = sprintf("%c%c%c", 27, 91, 109);
    } elseif (preg_match('/^vt100/', $term)) {
	$term_bold = sprintf("%c%c%c%c", 27, 91, 49, 109);
	$term_norm = sprintf("%c%c%c", 27, 91, 109);
    } else {
	$term_bold = $term_norm = "";
    }

    $windows_p = (substr(php_uname(), 0, 7) == "Windows");
    if ($windows_p) {
	$php = "./php.exe";
    } else {
	$php = $GLOBALS["TOP_BUILDDIR"]."/php";
    }

    if (!is_executable($php)) {
	writeln("PHP CGI binary ($php) not executable.");
	writeln("Please compile PHP as a CGI executable and try again.");
	exit;
    }

    $skip = array(
	"CVS" => 1
	);
    $testdirs = array();
    $tmpfile = array();
    $tests_in_dir = array();

    register_shutdown_function("delete_tmpfiles");

    $skipped = $failed = $passed = $total = 0;
}

function &parse_options(&$argc, &$argv) {
    $options = array();
    while ($argc > 0 && ($opt = substr($argv[0], 0, 2)) == "--") {
	$opt = array_shift($argv);
	$argc--;
	if ($arg == "--") {
	    return $options;
	}
	if (preg_match('/^--([^=]+)=(.*)$/', $opt, $matches)) {
	    $opt = $matches[1];
	    $arg = $matches[2];
	} else {
	    $arg = true;
	}
	$options[$opt] = $arg;
    }
    return $options;
}

function do_testing($argc, &$argv) {
    global $term, $windows_p, $php, $skip, $testdirs, $tmpfile, $opts,
	$skipped, $failed, $passed, $total, $term_bold, $term_norm;

    if ($argc > 1) {
	if (is_dir($argv[1])) {
	    $dir = $argv[1];
	} else {
	    for ($i = 1; $i < $argc; $i++) {
		switch (run_test($argv[$i])) {
		    case TEST_SKIPPED:
		    case TEST_INTERNAL_ERROR:
			$skipped++;
			break;
		    case TEST_FAILED:
			$failed++;
			break;
		    case TEST_PASSED:
			$passed++;
			break;
		}
		$total++;
	    }
	}
    } else {
	$dir = $GLOBALS["TOP_SRCDIR"];
    }

    if ($dir) {
	find_testdirs($dir);
	for ($i = 0; $i < sizeof($testdirs); $i++) {
	    run_tests_in_dir($testdirs[$i]);
	}
    }

    $counting = $total - $skipped;

    if ($counting <= 0) {
	writeln("No tests were run.");
	return;
    }

    $total_d = (double)$total;
    $counting_d = (double)$counting;
    $passed_p  = 100 * ($passed / $counting_d);
    $failed_p  = 100 * ($failed / $counting_d);
    $skipped_p = 100 * ($skipped / $total_d);
    $passed_pstr = sprintf($passed_p < 10.0 ? "%1.1f" : "%3.0f", $passed_p);
    $failed_pstr = sprintf($failed_p < 10.0 ? "%1.1f" : "%3.0f", $failed_p);
    $skipped_pstr = sprintf($skipped_p < 10.0 ? "%1.1f" : "%3.0f", $skipped_p);

    writeln("TEST RESULT SUMMARY");
    writeln("=============================");
    writeln(sprintf("Number of tests:  %4d", $total));
    writeln(sprintf("Tests skipped:    %4d (%s%%)", $skipped, $skipped_pstr));
    writeln(sprintf("Tests failed:     %4d (%s%%)", $failed, $failed_pstr));
    writeln(sprintf("Tests passed:     %4d (%s%%)", $passed, $passed_pstr));
}

function find_testdirs($dir = '.', $first_pass = true) {
    global $testdirs, $skip;

    if ($first_pass && is_dir($dir)) {
	$testdirs[] = $dir;
    }
    $dp = @opendir($dir);
    if (!$dp) {
	print "Warning: could not open directory $dir\n";
	return false;
    }
    while ($ent = readdir($dp)) {
	$path = "$dir/$ent";
	if ((isset($skip[$ent]) && $skip[$ent]) || substr($ent, 0, 1) == "." || !is_dir($path)) {
	    continue;
	}
	if (strstr("/$path/", "/tests/")) {
	    $testdirs[] = $path;
	}
	find_testdirs($path, false);
    }
    closedir($dp);
}

function run_tests_in_dir($dir = '.') {
    global $skip, $skipped, $failed, $passed, $total, $opts, $tests_in_dir;
    $dp = opendir($dir);
    if (!$dp) {
	print "Warning: could not run tests in $dir\n";
	return false;
    }
    $testfiles = array();
    while ($ent = readdir($dp)) {
	if ((isset($skip[$ent]) && $skip[$ent]) || substr($ent, 0, 1) == "." || substr($ent, -5) != ".phpt") {
	    continue;
	}
	$testfiles[] = "$dir/$ent";
	if(isset($tests_in_dir[$dir]))	$tests_in_dir[$dir]++; else $tests_in_dir[$dir]=1;
    }
    closedir($dp);
    if (isset($tests_in_dir[$dir]) && ($tests_in_dir[$dir] == 0)) {
	return true;
    }
    $oskipped = $skipped;
    writeln("%bRunning tests in $dir%B");
    writeln("=================".str_repeat("=", strlen($dir)));
    sort($testfiles);
    for ($i = 0; $i < sizeof($testfiles); $i++) {
	switch (run_test($testfiles[$i])) {
	    case TEST_SKIPPED:
	    case TEST_INTERNAL_ERROR:
		$skipped++;
		break;
	    case TEST_FAILED:
		$failed++;
		break;
	    case TEST_PASSED:
		$passed++;
		break;
	}
	$total++;
    }
    if ($oskipped + (isset($tests_in_dir[$dir])?$tests_in_dir[$dir]:0)  == $skipped) {
	writeln("(all skipped)");
    }
    writeln("");
    return true;
}

function skip_headers($fp) {
    while (!feof($fp)) {
	if (trim(fgets($fp, 1024)) == "") {
	    break;
	}
    }
}

function delete_tmpfiles() {
    global $tmpfile;
    reset($tmpfile);
    while (list($k, $v) = each($tmpfile)) {
	if (file_exists($v)) {
	    //print "unlink($v): "; var_dump(unlink($v));
	    unlink($v);
	}
    }
}

/**
 * Compares two files, ignoring blank lines.
 *
 * @param $file1 string name of first file to compare
 * @param $file2 string name of second file to compare
 *
 * @return bool whether the files were "equal"
 */
function compare_results($file1, $file2) {
	$data1 = $data2 = "";
    if (!($fp1 = @fopen($file1, "r")) || !($fp2 = @fopen($file2, "r"))) {
	return false;
    }
    while (!(feof($fp1) || feof($fp2))) {
	if (!feof($fp1) && trim($line1 = fgets($fp1, 10240)) != "") {
	    //print "adding line1 $line1\n";
	    $data1 .= $line1;
	}
	if (!feof($fp2) && trim($line2 = fgets($fp2, 10240)) != "") {
	    //print "adding line2 $line2\n";
	    $data2 .= $line2;
	}
    }
    fclose($fp1);
    fclose($fp2);
    if (trim($data1) != trim($data2)) {
	//print "data1=";var_dump($data1);
	//print "data2=";var_dump($data2);
	return false;
    }
    return true;
}

function run_test($file) {
    global $php, $tmpfile, $term_bold, $term_norm;

    $variables = array("TEST", "POST", "GET", "FILE", "EXPECT", "SKIPIF",
		       "OUTPUT");
    $fp = @fopen($file, "r");
    if (!$fp) {
	return TEST_INTERNAL_ERROR;
    }
    $tmpdir = dirname($file);
    $tmpfix = "phpt.";
    $tmpfile["FILE"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["SKIPIF"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["POST"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["EXPECT"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["OUTPUT"] = tempnam($tmpdir, $tmpfix);
    
    while ($line = fgets($fp, 4096)) {
	if (preg_match('/^--([A-Z]+)--$/', $line, $matches)) {
	    $var = $matches[1];
	    if (isset($tmpfile[$var]) && $tmpfile[$var]) {
		$fps[$var] = @fopen($tmpfile[$var], "w");
	    } else {
		$$var = '';
	    }
	} else {
	    if (isset($var) && $var) {
		if ($var == "POST") {
		    $line = trim($line);
		}
		if (isset($fps[$var]) && $fps[$var]) {
		    fwrite($fps[$var], $line);
		} else {
		    $$var .= $line;
		}
	    }
	}
    }
		if(isset($fps) && is_array($fps)) {
			reset($fps);
			while (list($k, $v) = each($fps)) {
				if (is_resource($v)) {
					fclose($v);
				}
			}
    }
    putenv("PHP_TEST=1");
    putenv("REDIRECT_STATUS=1");
    putenv("CONTENT_LENGTH=");
    putenv("QUERY_STRING=".(isset($GET)?$GET:""));
    if (isset($fps["SKIPIF"])) {
	$tmpfile["SKIPIF_OUTPUT"] = tempnam($tmpdir, $tmpfix);
	putenv("REQUEST_METHOD=GET");
	putenv("CONTENT_TYPE=");
	putenv("PATH_TRANSLATED=$tmpfile[SKIPIF]");
	putenv("SCRIPT_FILENAME=$tmpfile[SKIPIF]");
	$skipifcmd = "$php -q -f $tmpfile[SKIPIF] > $tmpfile[SKIPIF_OUTPUT]";
	system($skipifcmd, $ret);
	$sp = @fopen($tmpfile["SKIPIF_OUTPUT"], "r");
	if ($sp) {
	    skip_headers($sp);
	    $skip = trim(fgets($sp, 1024));
	    fclose($sp);
	    if ($skip == "skip") {
		delete_tmpfiles();
		return TEST_SKIPPED;
	    }
	}
    }
    putenv("PATH_TRANSLATED=$tmpfile[FILE]");
    putenv("SCRIPT_FILENAME=$tmpfile[FILE]");
    if (isset($fps["POST"])) {
	putenv("REQUEST_METHOD=POST");
	putenv("CONTENT_TYPE=application/x-www-form-urlencoded");
	putenv("CONTENT_LENGTH=".filesize($tmpfile["POST"]));
    } else {
	putenv("REQUEST_METHOD=GET");
	putenv("CONTENT_TYPE=");
	putenv("CONTENT_LENGTH=");
    }
    putenv("a=");
    putenv("b=");
    putenv("c=");
    putenv("d=");
    if (isset($fps["POST"])) {
	$cmd = "$php -f $tmpfile[FILE] < $tmpfile[POST]";
    } else {
	$cmd = "$php -f $tmpfile[FILE]";
    }
    $ofp = @fopen($tmpfile["OUTPUT"], "w");
    if (!$ofp) {
	writeln("Error: could not write to output file");
	delete_tmpfiles();
	return TEST_INTERNAL_ERROR;
    }
    $cp = popen($cmd, "r");
    if (!$cp) {
	writeln("Error: could not execute: $cmd");
	delete_tmpfiles();
	return TEST_INTERNAL_ERROR;
    }
    skip_headers($cp);
    while ($data = fread($cp, 2048)) {
	fwrite($ofp, $data);
    }
    fclose($ofp);
    pclose($cp);
    $desc = isset($TEST)?trim($TEST):"";
    $outfile = preg_replace('/\.phpt$/', '.out', $file);
    $expectfile = preg_replace('/\.phpt$/', '.exp', $file);
    $phpfile = preg_replace('/\.phpt$/', '.php', $file);
    if (compare_results($tmpfile["OUTPUT"], $tmpfile["EXPECT"])) {
	$status = TEST_PASSED;
	$text = "passed";
	$pre = $post = "";
	if (file_exists($outfile)) {
	    unlink($outfile);
	}
	if (file_exists($expectfile)) {
	    unlink($expectfile);
	}
	if (file_exists($phpfile)) {
	    unlink($phpfile);
	}
    } else {
	//system("env");
	$status = TEST_FAILED;
	$text = "failed";
	$pre = $term_bold;
	$post = $term_norm;
	$desc .= " (".basename($file).")";
	if (file_exists($outfile)) {
	    unlink($outfile);
	}
	copy($tmpfile["OUTPUT"], $outfile);
	copy($tmpfile["EXPECT"], $expectfile);
	copy($tmpfile["FILE"], $phpfile);
    }
    writeln(sprintf("%s%-68s ... %s%s", $pre, substr($desc, 0, 68),
		    $text, $post));
//    if ($status == TEST_FAILED) {
//	  for ($i = 0; $i < sizeof($variables); $i++) {
//	      $var = $variables[$i];
//	      print "$var:\n";
//	      if ($tmpfile[$var]) {
//		  if (file_exists($tmpfile[$var])) {
//		      system("cat ".$tmpfile[$var]);
//		  }
//	      } else {
//		  print $$var;
//	      }
//	  }
//	  print "--\n\n";
//    }
    delete_tmpfiles();
    return $status;
}

?>
