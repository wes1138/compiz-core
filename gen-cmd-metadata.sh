#!/bin/bash

[[ -z $1 ]] && ncmds=20 || ncmds=$1

cat > ./metadata/commands.xml.in <<EOF
<compiz>
    <plugin name="commands">
	<_short>Commands</_short>
	<_long>Assigns bindings to arbitrary commands</_long>
	<category>General</category>
	<display>
	    <group>
		<_short>Commands</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./metadata/commands.xml.in <<EOF
		<option name="command$i" type="string">
		    <_short>Command line $i</_short>
		    <_long>Command line to be executed in shell when run_command$i is invoked</_long>
		    <default></default>
		</option>
EOF
done

cat >> ./metadata/commands.xml.in <<EOF
	    </group>
	    <group>
		<_short>Key Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./metadata/commands.xml.in <<EOF
		<option name="run_command${i}_key" type="key">
		    <_short>Run command $i</_short>
		    <_long>A keybinding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat >> ./metadata/commands.xml.in <<EOF
	    </group>
	    <group>
		<_short>Button Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./metadata/commands.xml.in <<EOF
		<option name="run_command${i}_button" type="button">
		    <_short>Run command $i</_short>
		    <_long>A button binding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat >> ./metadata/commands.xml.in <<EOF
	    </group>
	    <group>
		<_short>Edge Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./metadata/commands.xml.in <<EOF
		<option name="run_command${i}_edge" type="edge">
		    <_short>Run command $i</_short>
		    <_long>An edge binding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat >> ./metadata/commands.xml.in <<EOF
	    </group>
	</display>
    </plugin>
</compiz>
EOF

# now build corresponding header file with metadata struct and constants.

cat > ./plugins/commands.h <<EOF
/* DO NOT EDIT ME.  I'm auto-generated.  Not to mention ugly.
 * Instead talk to the mad scientist that built me: ../gen-cmd-metadata.sh */

#pragma once

static Bool
runCommandDispatch (CompDisplay     *d,
		    CompAction      *action,
		    CompActionState state,
		    CompOption      *option,
		    int             nOption);

static const CompMetadataOptionInfo commandsDisplayOptionInfo[] = {
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./plugins/commands.h <<EOF
    { "command$i", "string", 0, 0, 0 },
EOF
done
for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./plugins/commands.h <<EOF
    { "run_command${i}_key", "key", 0, runCommandDispatch, 0 },
EOF
done
for (( i = 0; i < $ncmds; i++ )); do
	cat >> ./plugins/commands.h <<EOF
    { "run_command${i}_button", "button", 0, runCommandDispatch, 0 },
EOF
done
for (( i = 0; i < $ncmds - 1; i++ )); do
	cat >> ./plugins/commands.h <<EOF
    { "run_command${i}_edge", "edge", 0, runCommandDispatch, 0 },
EOF
done
cat >> ./plugins/commands.h <<EOF
    { "run_command$((ncmds - 1))_edge", "edge", 0, runCommandDispatch, 0 }
};

#define COMMANDS_NUM_COMMANDS                        $ncmds
#define COMMANDS_DISPLAY_OPTION_COMMAND0             0
#define COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_KEY     $ncmds
#define COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_BUTTON  $((ncmds * 2))
#define COMMANDS_DISPLAY_OPTION_RUN_COMMAND0_EDGE    $((ncmds * 3))
#define COMMANDS_DISPLAY_OPTION_NUM                  $((ncmds * 4))
EOF
