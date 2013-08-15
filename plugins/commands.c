/*
 * Copyright © 2009 Danny Baumann
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Danny Baumann not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 * Danny Baumann makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * DANNY BAUMANN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL DENNIS KASPRZYK BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Danny Baumann <dannybaumann@web.de>
 */

#include <compiz-core.h>

static CompMetadata commandsMetadata;

static int displayPrivateIndex;

#include "commands.h"

typedef struct _CommandsDisplay {
    CompOption opt[COMMANDS_DISPLAY_OPTION_NUM];
} CommandsDisplay;

#define GET_COMMANDS_DISPLAY(d)                                       \
    ((CommandsDisplay *) (d)->base.privates[displayPrivateIndex].ptr)
#define COMMANDS_DISPLAY(d)                                           \
    CommandsDisplay *cd = GET_COMMANDS_DISPLAY (d)

#define NUM_OPTIONS(d) (sizeof ((d)->opt) / sizeof (CompOption))

static Bool
runCommandDispatch (CompDisplay     *d,
		    CompAction      *action,
		    CompActionState state,
		    CompOption      *option,
		    int             nOption)
{
    CompScreen *s;
    Window     xid;

    xid = getIntOptionNamed (option, nOption, "root", 0);
    s   = findScreenAtDisplay (d, xid);

    if (s)
    {
	int index = COMMANDS_DISPLAY_OPTION_COMMAND0 + action->priv.val;

	COMMANDS_DISPLAY (d);

	runCommand (s, cd->opt[index].value.s);
    }

    return TRUE;
}

static CompBool
commandsInitDisplay (CompPlugin  *p,
		     CompDisplay *d)
{
    CommandsDisplay *cd;
    int             i;

    if (!checkPluginABI ("core", CORE_ABIVERSION))
	return FALSE;

    cd = malloc (sizeof (CommandsDisplay));
    if (!cd)
	return FALSE;

    if (!compInitDisplayOptionsFromMetadata (d,
					     &commandsMetadata,
					     commandsDisplayOptionInfo,
					     cd->opt,
					     COMMANDS_DISPLAY_OPTION_NUM))
    {
	free (cd);
	return FALSE;
    }

    for (i = 0; i < COMMANDS_NUM_COMMANDS; i++)
    {
	int opt;
	
	opt = COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_KEY + i;
	cd->opt[opt].value.action.priv.val = i;
	opt = COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_BUTTON + i;
	cd->opt[opt].value.action.priv.val = i;
	opt = COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_EDGE + i;
	cd->opt[opt].value.action.priv.val = i;
    }

    d->base.privates[displayPrivateIndex].ptr = cd;

    return TRUE;
}

static void
commandsFiniDisplay (CompPlugin  *p,
		     CompDisplay *d)
{
    COMMANDS_DISPLAY (d);

    compFiniDisplayOptions (d, cd->opt, COMMANDS_DISPLAY_OPTION_NUM);

    free (cd);
}

static CompOption *
commandsGetDisplayOptions (CompPlugin  *p,
			   CompDisplay *d,
			   int         *count)
{
    COMMANDS_DISPLAY (d);

    *count = NUM_OPTIONS (cd);
    return cd->opt;
}

static CompBool
commandsSetDisplayOption (CompPlugin      *p,
			  CompDisplay     *d,
			  const char      *name,
			  CompOptionValue *value)
{
    CompOption *o;

    COMMANDS_DISPLAY (d);

    o = compFindOption (cd->opt, NUM_OPTIONS (cd), name, NULL);
    if (!o)
	return FALSE;

    return compSetDisplayOption (d, o, value);
}

static CompBool
commandsInitObject (CompPlugin *p,
		    CompObject *o)
{
    static InitPluginObjectProc dispTab[] = {
	(InitPluginObjectProc) 0, /* InitCore */
	(InitPluginObjectProc) commandsInitDisplay
    };

    RETURN_DISPATCH (o, dispTab, ARRAY_SIZE (dispTab), TRUE, (p, o));
}

static void
commandsFiniObject (CompPlugin *p,
		    CompObject *o)
{
    static FiniPluginObjectProc dispTab[] = {
	(FiniPluginObjectProc) 0, /* FiniCore */
	(FiniPluginObjectProc) commandsFiniDisplay
    };

    DISPATCH (o, dispTab, ARRAY_SIZE (dispTab), (p, o));
}

static CompOption *
commandsGetObjectOptions (CompPlugin *p,
			  CompObject *o,
			  int        *count)
{
    static GetPluginObjectOptionsProc dispTab[] = {
	(GetPluginObjectOptionsProc) 0, /* GetCoreOptions */
	(GetPluginObjectOptionsProc) commandsGetDisplayOptions
    };

    *count = 0;
    RETURN_DISPATCH (o, dispTab, ARRAY_SIZE (dispTab),
		     (void *) count, (p, o, count));
}

static CompBool
commandsSetObjectOption (CompPlugin      *p,
			 CompObject      *o,
			 const char      *name,
			 CompOptionValue *value)
{
    static SetPluginObjectOptionProc dispTab[] = {
	(SetPluginObjectOptionProc) 0, /* SetCoreOption */
	(SetPluginObjectOptionProc) commandsSetDisplayOption,
    };

    RETURN_DISPATCH (o, dispTab, ARRAY_SIZE (dispTab), FALSE,
		     (p, o, name, value));
}

static Bool
commandsInit (CompPlugin *p)
{
    if (!compInitPluginMetadataFromInfo (&commandsMetadata,
					 p->vTable->name,
					 commandsDisplayOptionInfo,
					 COMMANDS_DISPLAY_OPTION_NUM, 0, 0))
	return FALSE;

    displayPrivateIndex = allocateDisplayPrivateIndex ();
    if (displayPrivateIndex < 0)
    {
	compFiniMetadata (&commandsMetadata);
	return FALSE;
    }

    compAddMetadataFromFile (&commandsMetadata, p->vTable->name);

    return TRUE;
}

static void
commandsFini (CompPlugin *p)
{
    freeDisplayPrivateIndex (displayPrivateIndex);
    compFiniMetadata (&commandsMetadata);
}

static CompMetadata *
commandsGetMetadata (CompPlugin *p)
{
    return &commandsMetadata;
}

static CompPluginVTable commandsVTable = {
    "commands",
    commandsGetMetadata,
    commandsInit,
    commandsFini,
    commandsInitObject,
    commandsFiniObject,
    commandsGetObjectOptions,
    commandsSetObjectOption
};

CompPluginVTable *
getCompPluginInfo20070830 (void)
{
    return &commandsVTable;
}
