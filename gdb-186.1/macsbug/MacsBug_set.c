/*--------------------------------------------------------------------------------------*
 |                                                                                      |
 |                                    MacsBug_set.c                                     |
 |                                                                                      |
 |                          MacsBug SET/SHOW Option Processing                          |
 |                                                                                      |
 |                                     Ira L. Ruben                                     |
 |                       Copyright Apple Computer, Inc. 2000-2001                       |
 |                                                                                      |
 *--------------------------------------------------------------------------------------*

 This file contains all the routines assoicated with the handling of the MacsBug options
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "MacsBug.h"

/*--------------------------------------------------------------------------------------*/

int ditto     	   = 0;				/* ditto dup memory displays		*/
int unmangle  	   = 0;				/* unmanged names in disassembly	*/
int echo_commands  = 0;				/* echo command lines to the history	*/
int wrap_lines 	   = 1;				/* wrap history lines			*/
int show_so_si_src = 1;				/* show source with so/si commands	*/
int dx_state	   = 1;				/* breakpoints enabled state		*/
int tab_value 	   = DEFAULT_TAB_VALUE;		/* history display tab value		*/
int pc_area_lines  = DEFAULT_PC_LINES;		/* user controlled pc area max lines	*/
int cmd_area_lines = DEFAULT_CMD_LINES;		/* user controlled cmd area max lines	*/
int max_history    = DEFAULT_HISTORY_SIZE;	/* max nbr of lines of history recorded	*/

int mb_testing 	   = 0;				/* set by SET mb_testing for debugging	*/

/* The following is what gets set by the SET option command before we get a chance to	*/
/* check each new setting.  Since these are what we tell gdb to set it is these that	*/
/* gdb knows about and thus it is these that gdb uses for its SHOW command.  So we need	*/
/* to make sure they are initialized to the proper values and left in a consistant 	*/
/* state since we have no ide when a SHOW will be issued.  Further the string values	*/
/* need to be malloc'ed space.								*/

/* The string values are set as on | off | show | now.  We initalize them as "enabled"	*/
/* or "disabled" as appropriate.							*/

static char *ditto_args;
static char *unmangle_args;
static char *echo_args;
static char *wrap_args;
static char *sosi_args;
static char *dx_args;
static int  new_tab_value      = DEFAULT_TAB_VALUE;
static int  new_pc_area_lines  = DEFAULT_PC_LINES;
static int  new_cmd_area_lines = DEFAULT_CMD_LINES;
static int  new_max_history    = DEFAULT_HISTORY_SIZE;
static int  new_testing;

/*--------------------------------------------------------------------------------------*/

/*----------------------------------------------------------*
 | check_all_sets - check all SET operations for SET PROMPT |
 *----------------------------------------------------------*
 
 This is a generic SET command filter to see if a SET PROMPT was done.  If it was we
 change the gdb prompt string to the same prompt but prefixed with the cursor positioning
 needed to place it where we want it on the macsbug screen in the command line area.
*/

static void check_all_sets(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    char *prompt;
    
    if (macsbug_screen && !doing_set_prompt && theSetting && gdb_strcmpl(theSetting, "prompt")) {
    	if (type == Set_String)
	     update_macsbug_prompt();
    }
}

/*--------------------------------------------------------------------------------------*/

/*------------------------------------------*
 | macsbug_set - handle MacsBug SET options |
 *------------------------------------------*
 
 Common routine used by both mset() and all macsbug options that accept settings of the
 form:
  
   [m]set setting [on | off | now | show]
    
 The parameters to this function are:
   
   cmd		      "set" | "mset" | "dx"
   arg		      "" | on | off | now | show
   value	      pointer to int switch to be set according to option
   meaning            string to prefix "is [still] {en|dis}abled" messages
   additiona_stuff    NULL or function to call to do additional stuff when state changes
    
 As a standard gdb SET command the setting's arguments are handled as a arbitrary string
 to allow us to handle the case when no options are specified.  I'd like to use the enum
 form but that requires a argument.
*/
static void macsbug_set(char *cmd, char **arg, int *value, char *meaning,
			void (*additional_stuff)(int state))
{
    int  argc, err = 0;
    char *argv[4], tmpCmdLine[1024];

    static char *options[] = {"ON", "OFF", "NOW", "SHOW", NULL};
    
    gdb_setup_argv(safe_strcpy(tmpCmdLine, *arg), cmd, &argc, argv, 3);
    
    if (argc == 1) {
	if (*value) {
	    *value = 0;
	    if (additional_stuff)
	    	additional_stuff(0);
	    gdb_printf("%s is disabled\n", meaning);
	} else {
	    *value = 1;
	    if (additional_stuff)
	    	additional_stuff(1);
	    gdb_printf("%s is enabled\n", meaning);
	}
    } else if (argc == 2) {
	switch (gdb_keyword(argv[1], options)) {
	    case 0: /* on   */
		if (*value)
		    gdb_printf("%s is still enabled\n", meaning);
		else {
		    *value = 1;
		    if (additional_stuff)
		    	additional_stuff(1);
		    gdb_printf("%s is enabled\n", meaning);
		}
		break;
	    case 1: /* off  */
		if (*value) {
		    *value = 0;
		    if (additional_stuff)
		    	additional_stuff(0);
		    gdb_printf("%s is disabled\n", meaning);
		} else 
		    gdb_printf("%s is still disabled\n", meaning);
		break;
	    case 2: /* now  */
	    case 3: /* show */
		if (*value)
		    gdb_printf("%s is still enabled\n", meaning);
		else
		    gdb_printf("%s is still disabled\n", meaning);
		break;
	    default:
		err = 1;
	}
    } else
	err = 1;
    
    gdb_set_int("$__lastcmd__", 40);
    
    if (*value)
    	if (*arg)
	    *arg = strcpy((char *)gdb_realloc(*arg, 3), "on");
	else
	    *arg = strcpy((char *)gdb_malloc(3), "on");
    else if (*arg)
	*arg = strcpy((char *)gdb_realloc(*arg, 4), "off");
    else
	*arg = strcpy((char *)gdb_malloc(4), "off");
	
    if (err)
    	gdb_error("\"on\", \"off\", \"now\", or \"show\" expected.");
}
 
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------*
 | set_gdb_demangle - "additional stuff" to do for SET UNMANGLE |
 *--------------------------------------------------------------*
 
 We need to tell gdb what's going on too.
*/

static void set_gdb_demangle(int state)
{
    if (state) {
	gdb_execute_command("set print demangle on");
	gdb_execute_command("set print asm-demangle on");
    } else {
	gdb_execute_command("set print demangle off");
	gdb_execute_command("set print asm-demangle off");
    }
}


/*------------------------------------------------------------------------------------*
 | mset args - MacsBug's SET commands retained for compatablity with older gdb script |
 *------------------------------------------------------------------------------------*
 
 In the non-plugin version of the MacsBug commands MSET was created to allow setting of
 MacsBug-compatible options since we couldn't call it SET which is what MacsBug uses.
 So we'll continue to support MSET for compatibility with the older script.  But we are
 no longer maintaining it for noew options.
*/

static void mset(char *arg, int from_tty)
{
    int  argc;
    char *set_args, *p, *argv[5], tmpCmdLine[1024];
    
    static char *macsbug_set_keywords[] = {"DITTO", "UNMANGLE", NULL};
    
    gdb_setup_argv(safe_strcpy(tmpCmdLine, arg), "mset", &argc, argv, 4);
    
    if (argc > 1) {
        if (argc == 2)
	    p = "";
        else if (argc == 3)
	    p = argv[2];
	else
	    p = NULL;
	
	if (p) {
	    if (gdb_strcmpl(argv[1], "ditto")) {
		if (ditto_args)
		    gdb_free(ditto_args);
		ditto_args = strcpy((char *)gdb_malloc(strlen(arg)+1), p);
		macsbug_set("mset", &ditto_args, &ditto, "Ditto-display in memory dumps", NULL);
	    } else if (gdb_strcmpl(argv[1], "unmangle")) {
		if (unmangle_args)
		    gdb_free(unmangle_args);
		unmangle_args = strcpy((char *)gdb_malloc(strlen(arg)+1), p);
		macsbug_set("mset", &unmangle_args, &unmangle, "Unmangling of symbols", set_gdb_demangle);
	    } else
    	    	gdb_error("usage: MSET DITTO | UNMANGLE [ON | OFF | NOW | SHOW] (invalid arguments)");
	} else
    	    gdb_error("usage: MSET DITTO | UNMANGLE [ON | OFF | NOW | SHOW] (invalid arguments)");
    }
    
    gdb_set_int("$__lastcmd__", 40);
}

#define MSET_HELP \
"MSET option [ON|OFF|NOW|SHOW] -- Change the specified gdb MacsBug behavior.\n" 	\
"On/off options toggle if you don't specify ON or OFF.  NOW or SHOW lets you check\n" 	\
"the setting without disturbing it.  The options are:\n" 				\
"\n"	 										\
"�  DITTO:    When on, DM and DMA show ditto marks (''''''') instead of groups\n" 	\
"�            of identical lines.\n" 							\
"\n" 											\
"�  UNMANGLE: When on, C++ symbols appear as in source code, such as \"TFoo::Bar()\".\n"\
"�            When off, you'll see stuff like \"Bar__4TFooFv\" instead.\n" 		\
"\n" 											\
"SHOW is the same as NOW to display the current setting.  It was added since gdb\n" 	\
"uses SHOW to show settings.\n" 							\
"\n" 											\
"Macsbug features not supported: The is the MacsBug SET command was changed to\n" 	\
"�                               MSET since SET conflicts with the SET gdb command.\n" 	\
"\n" 											\
"�                               Options AUTOGP, ECHO, MOUSE, MENUBAR, SCROLLPROMPT,\n"	\
"�                               SUSPENDPROMPT, and SIMPLIFIED not supported.\n"	\
"\n" 											\
"This command is depricated.  Use SET.  Type \"help set\" to see a list of all gdb\n"	\
"SET options.  The MacsBug options all begin with \"mb-\" although it is optional\n"	\
"with \"ditto\" and \"unmangle\" for compatibility with MSET."


/*-------------------------------------------------------------*
 | control_breakpoints - "additional stuff" to do for (SET) DX |
 *-------------------------------------------------------------*
 
 We need to tell gdb what's going on too.
*/

static void control_breakpoints(int state)
{
    if (state)
	gdb_execute_command("enable breakpoints");
    else
	gdb_execute_command("disable breakpoints");
}


/*---------------------------------------------------*
 | dx on | off | now - enable or disable breakpoints |
 *---------------------------------------------------*/

static void dx(char *arg, int from_tty)
{
    if (dx_args)
    	gdb_free(dx_args);
    
    if (arg)
    	dx_args = strcpy((char *)gdb_malloc(strlen(arg)+1), arg);
    else
    	dx_args = strcpy((char *)gdb_malloc(1), "");
    
    macsbug_set("dx", &dx_args, &dx_state, "Breakpoints", control_breakpoints);
    
    gdb_set_int("$__lastcmd__", 39);
}

#define DX_HELP \
"DX [ON | OFF | NOW | SHOW] -- Temporarily enable/disable/toggle breakpoints.\n"	\
"The setting is toggled when there is no argument.\n"					\
"�\n" 											\
"SHOW is the same as NOW to display the current setting.  It was added\n" 		\
"since gdb uses SHOW to show settings."


/*-----------------------------------------------*
 | set_XXXXX - SET XXXXX [on | off | now | show] |
 *-----------------------------------------------*
 
 All options of this form are functions defined by the macro below.  The macro parameters have
 the following meanings:
   
   funct_name		the name of the function we are to define
   sw_name		the global switch to be set (an int)
   sw_value		the string value (on | off | now | show)
   help			the SET/SHOW help info
   additional_stuff	NULL or a function to be called when the switch state changes
*/
 
#define SET_ON_OFF_NOW(funct_name, sw_name, sw_value, help, additional_stuff) 		\
static void funct_name(char *theSetting, Gdb_Set_Type type, void *value, int show)	\
{											\
    macsbug_set("set", &sw_value, &sw_name, help, additional_stuff);			\
}
   
SET_ON_OFF_NOW(set_ditto,        ditto,          ditto_args, 	"Ditto-display in memory dumps",    NULL);
SET_ON_OFF_NOW(set_unmangle,     unmangle,       unmangle_args, "Unmangling of symbols",            set_gdb_demangle);
SET_ON_OFF_NOW(set_echo,         echo_commands,  echo_args, 	"Echoing command lines to history", NULL);
SET_ON_OFF_NOW(set_wrap,         wrap_lines,     wrap_args, 	"Wrapping history lines",           NULL);
SET_ON_OFF_NOW(set_so_si_source, show_so_si_src, sosi_args, 	"Source with SO/SI",                NULL);
SET_ON_OFF_NOW(set_dx,           dx_state,     	 dx_args, 	"Breakpoints",           	    control_breakpoints);

#define DITTO_DESCRIPTION 	"Set ditto marks for DM and DMA repeated lines"
#define UNMANGLE_DESCRIPTION 	"Set C++ symbol unmangling"
#define ECHO_DESCRIPTION 	"Set echoing commands to history area"
#define WRAP_DESCRIPTION 	"Set history area line wrapping"
#define SOSI_DESCRIPTION 	"Set source display with SO/SI"
#define DX_DESCRIPTION 		"Set stopping on breakpoints"


/*------------------------------------------------------------------------*
 | set_tab - SET mb-tab <tab setting to detab source for history display> |
 *------------------------------------------------------------------------*/

static void set_tab(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    if (new_tab_value < 0 | new_tab_value > 20)
    	gdb_error("invalid tab value (must be 0 to 20).");
    else
    	tab_value = new_tab_value;
}

#define TAB_DESCRIPTION "Set history area source line tab interpretation"


/*--------------------------------------------------------*
 | set_pc_area = SET mb-pc-area <nbr of lines in pc area> |
 *--------------------------------------------------------*/

static void set_pc_area(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    if (new_pc_area_lines < MIN_PC_LINES || new_pc_area_lines > MAX_PC_LINES)
	gdb_error("screen pc area must be %d to %d lines long", MIN_PC_LINES, MAX_PC_LINES);
    else {
    	pc_area_lines = new_pc_area_lines;
	if (macsbug_screen)
	    refresh(NULL, 0);
    }
}

#define PC_AREA_DESCRIPTION "Set number of lines in pc area"


/*---------------------------------------------------------------*
 | set_cmd_area = SET mb-cmd-area <nbr of lines in command area> |
 *---------------------------------------------------------------*/

static void set_cmd_area(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    if (new_cmd_area_lines < MIN_PC_LINES || new_cmd_area_lines > MAX_PC_LINES)
	gdb_error("screen command linearea must be %d to %d lines long", MIN_CMD_LINES, MAX_CMD_LINES);
    else {
    	cmd_area_lines = new_cmd_area_lines;
	if (macsbug_screen)
	    refresh(NULL, 0);
    }
}

#define CMD_AREA_DESCRIPTION "Set number of lines in command line area"


/*--------------------------------------------------------------*
 | set_history_size - SET mb-history <line capacity of history> |
 *--------------------------------------------------------------*/

static void set_history_size(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    get_screen_size(&max_rows, &max_cols);

    if (new_max_history < max_rows)
    	gdb_error("the history size cannot be set smaller than the number of lines in the history area");
    else {
        if (macsbug_screen && new_max_history < max_history)
	   forget_some_history(new_max_history);
	max_history = new_max_history;
    }
}

#define HISTORY_DESCRIPTION "Set number of remembered history lines"


/*-----------------------------------------------------*
 | set_mb_testing - internal switch to control testing |
 *-----------------------------------------------------*/

static void set_mb_testing(char *theSetting, Gdb_Set_Type type, void *value, int show)
{
    switch (new_testing) {
    	case 0:
	    mb_testing = 0;
	    break;
	
	default:
	    gdb_error("Invalid value.");
	    break;
    }
}

#define TESTING_DESCRIPTION "Private debugging value";


/*----------------------------------------------------*
 | init_macsbug_set - MacsBug SET/SHOW initialization |
 *----------------------------------------------------*/

void init_macsbug_set(void)
{
    MACSBUG_COMMAND(dx,   DX_HELP);
    //MACSBUG_COMMAND(mset, MSET_HELP);
    gdb_define_plugin("mset", mset, Gdb_Support, MSET_HELP);

    gdb_define_set_generic(check_all_sets);
    
    gdb_define_set("ditto",           set_ditto,        Set_String, &ditto_args,         0, DITTO_DESCRIPTION);
    gdb_define_set("unmangle",        set_unmangle,     Set_String, &unmangle_args,      0, UNMANGLE_DESCRIPTION);
    gdb_define_set("dx",              set_dx,     	Set_String, &dx_args,            0, DX_DESCRIPTION);
    
    gdb_define_set("mb-ditto",        set_ditto,        Set_String, &ditto_args,         0, DITTO_DESCRIPTION);
    gdb_define_set("mb-unmangle",     set_unmangle,     Set_String, &unmangle_args,      0, UNMANGLE_DESCRIPTION);
    gdb_define_set("mb-echo",         set_echo,         Set_String, &echo_args,          0, ECHO_DESCRIPTION);
    gdb_define_set("mb-wrap",         set_wrap,         Set_String, &wrap_args,          0, WRAP_DESCRIPTION);
    gdb_define_set("mb-so-si-source", set_so_si_source, Set_String, &sosi_args,          0, SOSI_DESCRIPTION);
    gdb_define_set("mb-dx",  	      set_dx,           Set_String, &dx_args,            0, DX_DESCRIPTION);
   
    gdb_define_set("mb-tab",          set_tab,          Set_Int,    &new_tab_value,      0, TAB_DESCRIPTION);
    gdb_define_set("mb-pc-area",      set_pc_area,      Set_Int,    &new_pc_area_lines,  0, PC_AREA_DESCRIPTION);
    gdb_define_set("mb-cmd-area",     set_cmd_area,     Set_Int,    &new_cmd_area_lines, 0, CMD_AREA_DESCRIPTION);
    gdb_define_set("mb-history",      set_history_size, Set_Int,    &new_max_history,    0, HISTORY_DESCRIPTION);
    
    //gdb_define_set("mb-testing",    set_mb_testing,   Set_Int,    &new_testing,        0, TESTING_DESCRIPTION);
    
    /* Init the string values for the SHOW command. It has to be malloc'ed space. From	*/
    /* this point on we'll maintain them as the values changed.				*/
    
    #define INIT_ENABLED_DISABLED(x, y) if (y)						\
					    x = strcpy((char *)gdb_malloc(3), "on");	\
    					else						\
					    x = strcpy((char *)gdb_malloc(4), "off");
    
    INIT_ENABLED_DISABLED(ditto_args, 	 ditto);
    INIT_ENABLED_DISABLED(unmangle_args, unmangle);
    INIT_ENABLED_DISABLED(echo_args, 	 echo_commands);
    INIT_ENABLED_DISABLED(wrap_args, 	 wrap_lines);
    INIT_ENABLED_DISABLED(sosi_args, 	 show_so_si_src);
    INIT_ENABLED_DISABLED(dx_args, 	 dx_state);
}
