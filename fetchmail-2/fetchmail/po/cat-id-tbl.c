/* Automatically generated by po2tbl.sed from fetchmail.pot.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "libgettext.h"

const struct _msg_ent _msg_tbl[] = {
  {"", 1},
  {"Checking if %s is really the same node as %s\n", 2},
  {"Yes, their IP addresses match\n", 3},
  {"No, their IP addresses don't match\n", 4},
  {"nameserver failure while looking for `%s' during poll of %s.\n", 5},
  {"mapped %s to local %s\n", 6},
  {"passed through %s matching %s\n", 7},
  {"\
analyzing Received line:\n\
%s", 8},
  {"line accepted, %s is an alias of the mailserver\n", 9},
  {"line rejected, %s is not an alias of the mailserver\n", 10},
  {"no Received address found\n", 11},
  {"found Received address `%s'\n", 12},
  {"message delimiter found while scanning headers\n", 13},
  {"no local matches, forwarding to %s\n", 14},
  {"forwarding and deletion suppressed due to DNS errors\n", 15},
  {"writing RFC822 msgblk.headers\n", 16},
  {"no recipient addresses matched declared local names", 17},
  {"recipient address %s didn't match any local name\n", 18},
  {"message has embedded NULs", 19},
  {"SMTP listener rejected local recipient addresses: ", 20},
  {"writing message text\n", 21},
  {"kerberos error %s\n", 22},
  {"krb5_sendauth: %s [server says '%*s'] \n", 23},
  {"\t%d msg %d octets long skipped by fetchmail.\n", 24},
  {"Kerberos V4 support not linked.\n", 25},
  {"Kerberos V5 support not linked.\n", 26},
  {"Option --flush is not supported with %s\n", 27},
  {"Option --all is not supported with %s\n", 28},
  {"Option --limit is not supported with %s\n", 29},
  {"timeout after %d seconds waiting to connect to server %s.\n", 30},
  {"timeout after %d seconds waiting for server %s.\n", 31},
  {"timeout after %d seconds waiting for %s.\n", 32},
  {"timeout after %d seconds waiting for listener to respond.\n", 33},
  {"timeout after %d seconds.\n", 34},
  {"Subject: fetchmail sees repeated timeouts\r\n", 35},
  {"\
Fetchmail saw more than %d timouts while attempting to get mail from %s@%s.\n", 36},
  {"\
This could mean that your mailserver is stuck, or that your SMTP listener", 37},
  {"\
is wedged, or that your mailbox file on the server has been corrupted by", 38},
  {"a server error.  You can run `fetchmail -v -v' to diagnose the problem.", 39},
  {"Fetchmail won't poll this mailbox again until you restart it.", 40},
  {"pre-connection command failed with status %d\n", 41},
  {"fetchmail: internal inconsistency\n", 42},
  {"fetchmail: %s connection to %s failed\n", 43},
  {": host is unknown\n", 44},
  {": name is valid but has no IP address\n", 45},
  {": unrecoverable name server error\n", 46},
  {": temporary name server error\n", 47},
  {": unknown DNS error %d\n", 48},
  {"Lock-busy error on %s@%s\n", 49},
  {"Authorization failure on %s@%s\n", 50},
  {"Subject: fetchmail authentication failed\r\n", 51},
  {"Fetchmail could not get mail from %s@%s.\n", 52},
  {"The attempt to get authorization failed.", 53},
  {"This probably means your password is invalid.", 54},
  {"selecting or re-polling folder %s\n", 55},
  {"selecting or re-polling default folder\n", 56},
  {"%s at %s (folder %s)\n", 57},
  {"%s at %s\n", 58},
  {"Polling %s\n", 59},
  {"%d %s (%d seen) for %s", 60},
  {"messages", 61},
  {"message", 62},
  {"%d %s for %s", 63},
  {" (%d octets).", 64},
  {"No mail for %s", 65},
  {"Skipping message %d, length -1\n", 66},
  {"skipping message %d\n", 67},
  {" (oversized, %d octets)\n", 68},
  {"reading message %d of %d", 69},
  {" (%d %soctets)", 70},
  {"header ", 71},
  {" (%d body octets) ", 72},
  {"message %d was not the expected length (%d actual != %d expected)\n", 73},
  {" retained\n", 74},
  {" flushed\n", 75},
  {" not flushed\n", 76},
  {"fetchlimit reached; %d messages left on server\n", 77},
  {"socket", 78},
  {"authorization", 79},
  {"missing or bad RFC822 header", 80},
  {"MDA", 81},
  {"client/server synchronization", 82},
  {"client/server protocol", 83},
  {"lock busy on server", 84},
  {"SMTP transaction", 85},
  {"DNS lookup", 86},
  {"undefined\n", 87},
  {"%s error while fetching from %s\n", 88},
  {"post-connection command failed with status %d\n", 89},
  {"%s: can't find your name and home directory!\n", 90},
  {"%s: can't determine your host!", 91},
  {"gethostbyname failed for %s\n", 92},
  {"Unknown system error", 93},
  {"%s (log message incomplete)", 94},
  {": Error %d", 95},
  {"partial error message buffer overflow", 96},
  {"%s's SMTP listener does not support ESMTP\n", 97},
  {"%s's SMTP listener does not support ETRN\n", 98},
  {"Queuing for %s started\n", 99},
  {"No messages waiting for %s\n", 100},
  {"Pending messages for %s started\n", 101},
  {"Unable to queue messages for node %s\n", 102},
  {"Node %s not allowed: %s\n", 103},
  {"ETRN syntax error\n", 104},
  {"ETRN syntax error in parameters\n", 105},
  {"Unknown ETRN error %d\n", 106},
  {"Option --keep is not supported with ETRN\n", 107},
  {"Option --flush is not supported with ETRN\n", 108},
  {"Option --remote is not supported with ETRN\n", 109},
  {"Option --check is not supported with ETRN\n", 110},
  {"fetchmail: thread sleeping for %d sec.\n", 111},
  {"This is fetchmail release %s", 112},
  {"Taking options from command line", 113},
  {" and %s\n", 114},
  {"Lockfile at %s\n", 115},
  {"No mailservers set up -- perhaps %s is missing?\n", 116},
  {"fetchmail: cannot allocate memory for lock name.\n", 117},
  {"fetchmail: removing stale lockfile\n", 118},
  {"fetchmail: no mailservers have been specified.\n", 119},
  {"fetchmail: no other fetchmail is running\n", 120},
  {"fetchmail: error killing %s fetchmail at %d; bailing out.\n", 121},
  {"background", 122},
  {"foreground", 123},
  {"fetchmail: %s fetchmail at %d killed.\n", 124},
  {"\
fetchmail: can't check mail while another fetchmail to same host is \
running.\n", 125},
  {"\
fetchmail: can't poll specified hosts with another fetchmail running at %d.\n", 126},
  {"fetchmail: another foreground fetchmail is running at %d.\n", 127},
  {"\
fetchmail: can't accept options while a background fetchmail is running.\n", 128},
  {"fetchmail: background fetchmail at %d awakened.\n", 129},
  {"fetchmail: elder sibling at %d died mysteriously.\n", 130},
  {"Enter password for %s@%s: ", 131},
  {"starting fetchmail %s daemon \n", 132},
  {"poll of %s skipped (failed authentication or too many timeouts)\n", 133},
  {"interval not reached, not querying %s\n", 134},
  {"saved UID List\n", 135},
  {"Query status=%d\n", 136},
  {"All connections are wedged.  Exiting.\n", 137},
  {"fetchmail: sleeping at %s\n", 138},
  {"awakened by %s\n", 139},
  {"awakened by signal %d\n", 140},
  {"awakened at %s\n", 141},
  {"normal termination, status %d\n", 142},
  {"Warning: multiple mentions of host %s in config file\n", 143},
  {"\
fetchmail: warning: no DNS available to check multidrop fetches from %s\n", 144},
  {"couldn't find HESIOD pobox for %s\n", 145},
  {"couldn't find canonical DNS name of %s\n", 146},
  {"%s configuration invalid, port number cannot be negative\n", 147},
  {"%s configuration invalid, RPOP requires a privileged port\n", 148},
  {"%s configuration invalid, LMTP can't use default SMTP port\n", 149},
  {"terminated with signal %d\n", 150},
  {"%s querying %s (protocol %s) at %s\n", 151},
  {"POP2 support is not configured.\n", 152},
  {"POP3 support is not configured.\n", 153},
  {"IMAP support is not configured.\n", 154},
  {"ETRN support is not configured.\n", 155},
  {"Cannot support ETRN without gethostbyname(2).\n", 156},
  {"unsupported protocol selected.\n", 157},
  {"Poll interval is %d seconds\n", 158},
  {"Logfile is %s\n", 159},
  {"Idfile is %s\n", 160},
  {"Progress messages will be logged via syslog\n", 161},
  {"Fetchmail will masquerade and will not generate Received\n", 162},
  {"Fetchmail will forward misaddressed multidrop messages to %s.\n", 163},
  {"Options for retrieving from %s@%s:\n", 164},
  {"  Mail will be retrieved via %s\n", 165},
  {"  Poll of this server will occur every %d intervals.\n", 166},
  {"  True name of server is %s.\n", 167},
  {"  This host %s be queried when no host is specified.\n", 168},
  {"will not", 169},
  {"will", 170},
  {"  Password will be prompted for.\n", 171},
  {"  APOP secret = \"%s\".\n", 172},
  {"  RPOP id = \"%s\".\n", 173},
  {"  Password = \"%s\".\n", 174},
  {"  Protocol is KPOP with Kerberos %s authentication", 175},
  {"  Protocol is %s", 176},
  {" (using service %s)", 177},
  {" (using network security options %s)", 178},
  {" (using port %d)", 179},
  {" (using default port)", 180},
  {" (forcing UIDL use)", 181},
  {"  Kerberos V4 preauthentication enabled.\n", 182},
  {"  Kerberos V5 preauthentication enabled.\n", 183},
  {"  Server nonresponse timeout is %d seconds", 184},
  {" (default).\n", 185},
  {"  Default mailbox selected.\n", 186},
  {"  Selected mailboxes are:", 187},
  {"  %s messages will be retrieved (--all %s).\n", 188},
  {"All", 189},
  {"Only new", 190},
  {"  Fetched messages %s be kept on the server (--keep %s).\n", 191},
  {"  Old messages %s be flushed before message retrieval (--flush %s).\n", 192},
  {"  Rewrite of server-local addresses is %s (--norewrite %s).\n", 193},
  {"enabled", 194},
  {"disabled", 195},
  {"  Carriage-return stripping is %s (stripcr %s).\n", 196},
  {"  Carriage-return forcing is %s (forcecr %s).\n", 197},
  {"  Interpretation of Content-Transfer-Encoding is %s (pass8bits %s).\n", 198},
  {"  MIME decoding is %s (mimedecode %s).\n", 199},
  {"  Nonempty Status lines will be %s (dropstatus %s)\n", 200},
  {"discarded", 201},
  {"kept", 202},
  {"  Message size limit is %d octets (--limit %d).\n", 203},
  {"  No message size limit (--limit 0).\n", 204},
  {"  Message size warning interval is %d seconds (--warnings %d).\n", 205},
  {"  Size warnings on every poll (--warnings 0).\n", 206},
  {"  Received-message limit is %d (--fetchlimit %d).\n", 207},
  {"  No received-message limit (--fetchlimit 0).\n", 208},
  {"  SMTP message batch limit is %d.\n", 209},
  {"  No SMTP message batch limit (--batchlimit 0).\n", 210},
  {"  Deletion interval between expunges is %d (--expunge %d).\n", 211},
  {"  No expunges (--expunge 0).\n", 212},
  {"  Messages will be appended to %s as BSMTP\n", 213},
  {"  Messages will be delivered with \"%s\".\n", 214},
  {"  Messages will be %cMTP-forwarded to:", 215},
  {" (default)", 216},
  {"  Host part of MAIL FROM line will be %s\n", 217},
  {"  Recognized listener spam block responses are:", 218},
  {"  Spam-blocking disabled\n", 219},
  {"  Server connection will be brought up with \"%s\".\n", 220},
  {"  No pre-connection command.\n", 221},
  {"  Server connection will be taken down with \"%s\".\n", 222},
  {"  No post-connection command.\n", 223},
  {"  No localnames declared for this host.\n", 224},
  {"  Multi-drop mode: ", 225},
  {"  Single-drop mode: ", 226},
  {"%d local name(s) recognized.\n", 227},
  {"  DNS lookup for multidrop addresses is %s.\n", 228},
  {"  Server aliases will be compared with multidrop addresses by ", 229},
  {"IP address.\n", 230},
  {"name.\n", 231},
  {"  Envelope-address routing is disabled\n", 232},
  {"  Envelope header is assumed to be: %s\n", 233},
  {"Received", 234},
  {"  Number of envelope header to be parsed: %d\n", 235},
  {"  Prefix %s will be removed from user id\n", 236},
  {"  No prefix stripping\n", 237},
  {"  Predeclared mailserver aliases:", 238},
  {"  Local domains:", 239},
  {"  Connection must be through interface %s.\n", 240},
  {"  No interface requirement specified.\n", 241},
  {"  Polling loop will monitor %s.\n", 242},
  {"  No monitor interface specified.\n", 243},
  {"  Server connections will be mode via plugin %s (--plugin %s).\n", 244},
  {"  No plugin command specified.\n", 245},
  {"  Listener connections will be mode via plugout %s (--plugout %s).\n", 246},
  {"  No plugout command specified.\n", 247},
  {"  No UIDs saved from this host.\n", 248},
  {"  %d UIDs saved.\n", 249},
  {"  Pass-through properties \"%s\".\n", 250},
  {"ERROR: no support for getpassword() routine\n", 251},
  {"\
\n\
Caught signal... bailing out.\n", 252},
  {"Could not decode initial BASE64 challenge\n", 253},
  {"Could not decode OTP challenge\n", 254},
  {"Secret pass phrase: ", 255},
  {"could not decode initial BASE64 challenge\n", 256},
  {"principal %s in ticket does not match -u %s\n", 257},
  {"non-null instance (%s) might cause strange behavior\n", 258},
  {"could not decode BASE64 ready response\n", 259},
  {"challenge mismatch\n", 260},
  {"Couldn't get service name for [%s]\n", 261},
  {"Using service name [%s]\n", 262},
  {"Sending credentials\n", 263},
  {"Error exchanging credentials\n", 264},
  {"Couldn't unwrap security level data\n", 265},
  {"Credential exchange complete\n", 266},
  {"Server requires integrity and/or privacy\n", 267},
  {"Unwrapped security level flags: %s%s%s\n", 268},
  {"Maximum GSS token size is %ld\n", 269},
  {"Error creating security level request\n", 270},
  {"Requesting authorisation as %s\n", 271},
  {"Releasing GSS credentials\n", 272},
  {"Error releasing credentials\n", 273},
  {"Protocol identified as IMAP4 rev 1\n", 274},
  {"Protocol identified as IMAP4 rev 0\n", 275},
  {"Protocol identified as IMAP2 or IMAP2BIS\n", 276},
  {"OTP authentication is supported\n", 277},
  {"GSS authentication is supported\n", 278},
  {"Required GSS capability not supported by server\n", 279},
  {"KERBEROS_V4 authentication is supported\n", 280},
  {"Required KERBEROS_V4 capability not supported by server\n", 281},
  {"Required LOGIN capability not supported by server\n", 282},
  {"re-poll failed\n", 283},
  {"mailbox selection failed\n", 284},
  {"missing IP interface address\n", 285},
  {"invalid IP interface address\n", 286},
  {"invalid IP interface mask\n", 287},
  {"activity on %s -noted- as %d\n", 288},
  {"skipping poll of %s, %s down\n", 289},
  {"skipping poll of %s, %s IP address excluded\n", 290},
  {"activity on %s checked as %d\n", 291},
  {"skipping poll of %s, %s inactive\n", 292},
  {"activity on %s was %d, is %d", 293},
  {"warning: found \"%s\" before any host names", 294},
  {"%s:%d: warning: found \"%s\" before any host names\n", 295},
  {"%s:%d: warning: unknown token \"%s\"\n", 296},
  {"String '%s' is not a valid number string.\n", 297},
  {"Value of string '%s' is %s than %d.\n", 298},
  {"smaller", 299},
  {"larger", 300},
  {"Invalid protocol `%s' specified.\n", 301},
  {"Invalid preauthentication `%s' specified.\n", 302},
  {"fetchmail: network security support is disabled\n", 303},
  {"usage:  fetchmail [options] [server ...]\n", 304},
  {"  Options are as follows:\n", 305},
  {"  -?, --help        display this option help\n", 306},
  {"  -V, --version     display version info\n", 307},
  {"  -c, --check       check for messages without fetching\n", 308},
  {"  -s, --silent      work silently\n", 309},
  {"  -v, --verbose     work noisily (diagnostic output)\n", 310},
  {"  -d, --daemon      run as a daemon once per n seconds\n", 311},
  {"  -N, --nodetach    don't detach daemon process\n", 312},
  {"  -q, --quit        kill daemon process\n", 313},
  {"  -L, --logfile     specify logfile name\n", 314},
  {"\
      --syslog      use syslog(3) for most messages when running as a \
daemon\n", 315},
  {"      --invisible   don't write Received & enable host spoofing\n", 316},
  {"  -f, --fetchmailrc specify alternate run control file\n", 317},
  {"  -i, --idfile      specify alternate UIDs file\n", 318},
  {"      --postmaster  specify recipient of last resort\n", 319},
  {"  -I, --interface   interface required specification\n", 320},
  {"  -M, --monitor     monitor interface for activity\n", 321},
  {"      --plugin      specify external command to open connection\n", 322},
  {"      --plugout     specify external command to open smtp connection\n", 323},
  {"  -p, --protocol    specify retrieval protocol (see man page)\n", 324},
  {"  -U, --uidl        force the use of UIDLs (pop3 only)\n", 325},
  {"  -P, --port        TCP/IP service port to connect to\n", 326},
  {"  -A, --auth        authentication type (password or kerberos)\n", 327},
  {"  -t, --timeout     server nonresponse timeout\n", 328},
  {"  -E, --envelope    envelope address header\n", 329},
  {"  -Q, --qvirtual    prefix to remove from local user id\n", 330},
  {"  -u, --username    specify users's login on server\n", 331},
  {"  -a, --all         retrieve old and new messages\n", 332},
  {"  -K, --nokeep      delete new messages after retrieval\n", 333},
  {"  -k, --keep        save new messages after retrieval\n", 334},
  {"  -F, --flush       delete old messages from server\n", 335},
  {"  -n, --norewrite   don't rewrite header addresses\n", 336},
  {"  -l, --limit       don't fetch messages over given size\n", 337},
  {"  -w, --warnings    interval between warning mail notification\n", 338},
  {"  -T, --netsec      set IP security request\n", 339},
  {"  -S, --smtphost    set SMTP forwarding host\n", 340},
  {"  -D, --smtpaddress set SMTP delivery domain to use\n", 341},
  {"  -Z, --antispam,   set antispam response values\n", 342},
  {"  -b, --batchlimit  set batch limit for SMTP connections\n", 343},
  {"  -B, --fetchlimit  set fetch limit for server connections\n", 344},
  {"  -e, --expunge     set max deletions between expunges\n", 345},
  {"      --mda         set MDA to use for forwarding\n", 346},
  {"      --bsmtp       set output BSMTP file\n", 347},
  {"      --lmtp        use LMTP (RFC2033) for delivery\n", 348},
  {"  -r, --folder      specify remote folder name\n", 349},
  {"Required APOP timestamp not found in greeting\n", 350},
  {"Timestamp syntax error in greeting\n", 351},
  {"Undefined protocol request in POP3_auth\n", 352},
  {"lock busy!  Is another session active?\n", 353},
  {"Messages inserted into list on server. Cannot handle this.\n", 354},
  {"protocol error\n", 355},
  {"protocol error while fetching UIDLs\n", 356},
  {"Option --remote is not supported with POP3\n", 357},
  {"Success", 358},
  {"Restricted user (something wrong with account)", 359},
  {"Invalid userid or passphrase", 360},
  {"Deity error", 361},
  {"RPA token 2: Base64 decode error\n", 362},
  {"Service chose RPA version %d.%d\n", 363},
  {"Service challenge (l=%d):\n", 364},
  {"Service timestamp %s\n", 365},
  {"RPA token 2 length error\n", 366},
  {"Realm list: %s\n", 367},
  {"RPA error in service@realm string\n", 368},
  {"RPA token 4: Base64 decode error\n", 369},
  {"User authentication (l=%d):\n", 370},
  {"RPA status: %02X\n", 371},
  {"RPA token 4 length error\n", 372},
  {"RPA rejects you: %s\n", 373},
  {"RPA rejects you, reason unknown\n", 374},
  {"RPA User Authentication length error: %d\n", 375},
  {"RPA Session key length error: %d\n", 376},
  {"RPA _service_ auth fail. Spoof server?\n", 377},
  {"Session key established:\n", 378},
  {"RPA authorisation complete\n", 379},
  {"Get response\n", 380},
  {"Get response return %d [%s]\n", 381},
  {"Hdr not 60\n", 382},
  {"Token length error\n", 383},
  {"Token Length %d disagrees with rxlen %d\n", 384},
  {"Mechanism field incorrect\n", 385},
  {"dec64 error at char %d: %x\n", 386},
  {"Inbound binary data:\n", 387},
  {"Outbound data:\n", 388},
  {"RPA String too long\n", 389},
  {"Unicode:\n", 390},
  {"RPA Failed open of /dev/urandom. This shouldn't\n", 391},
  {"    prevent you logging in, but means you\n", 392},
  {"    cannot be sure you are talking to the\n", 393},
  {"    service that you think you are (replay\n", 394},
  {"    attacks by a dishonest service are possible.)\n", 395},
  {"User challenge:\n", 396},
  {"MD5 being applied to data block:\n", 397},
  {"MD5 result is: \n", 398},
  {"forwarding to %s\n", 399},
  {"%cMTP error: %s\n", 400},
  {"BSMTP file open or preamble write failed\n", 401},
  {"about to deliver with: %s\n", 402},
  {"MDA open failed\n", 403},
  {"%cMTP connect to %s failed\n", 404},
  {"%cMTP listener doesn't like recipient address `%s'\n", 405},
  {"can't even send to %s!\n", 406},
  {"no address matches; forwarding to %s.\n", 407},
  {"MDA exited abnormally or returned nonzero status\n", 408},
  {"Message termination or close of BSMTP file failed\n", 409},
  {"SMTP listener refused delivery\n", 410},
  {"LMTP delivery error on EOM\n", 411},
  {"Unexpected non-503 response to LMTP EOM: %s\n", 412},
  {"fetchmail: socketpair failed\n", 413},
  {"fetchmail: fork failed\n", 414},
  {"dup2 failed\n", 415},
  {"running %s %s %s\n", 416},
  {"execl(%s) failed\n", 417},
  {"fetchmail: getaddrinfo(%s.%s)\n", 418},
  {"fetchmail: illegal address length received for host %s\n", 419},
  {"malloc failed\n", 420},
  {"realloc failed\n", 421},
};

int _msg_tbl_length = 421;
