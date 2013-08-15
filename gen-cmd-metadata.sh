#!/bin/bash

[[ -z $1 ]] && ncmds=20 || ncmds=$1

cat <<EOF
<compiz>
    <plugin name="commands">
	<_short>Commands</_short>
	<_long>Assigns bindings to arbitrary commands</_long>
	<category>General</category>
	<display>
EOF

cat <<EOF
	    <group>
		<_short>Commands</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat <<EOF
		<option name="command$i" type="string">
		    <_short>Command line $i</_short>
		    <_long>Command line to be executed in shell when run_command$i is invoked</_long>
		    <default></default>
		</option>
EOF
done

cat <<EOF
	    </group>
	    <group>
		<_short>Key Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat <<EOF
		<option name="run_command${i}_key" type="key">
		    <_short>Run command $i</_short>
		    <_long>A keybinding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat <<EOF
	    </group>
	    <group>
		<_short>Button Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat <<EOF
		<option name="run_command${i}_button" type="button">
		    <_short>Run command $i</_short>
		    <_long>A button binding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat <<EOF
	    </group>
	    <group>
		<_short>Edge Bindings</_short>
EOF

for (( i = 0; i < $ncmds; i++ )); do
	cat <<EOF
		<option name="run_command${i}_edge" type="edge">
		    <_short>Run command $i</_short>
		    <_long>An edge binding that when invoked, will run the shell command identified by command$i</_long>
		</option>
EOF
done

cat <<EOF
	    </group>
	</display>
    </plugin>
</compiz>
EOF
