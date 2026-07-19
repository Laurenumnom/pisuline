#!/bin/bash
# Author: Lauren Toivanen
# Sourse this file in your .bashrc end to enable pisuline:
# . /usr/share/pisuline/pisuline.sh
function ps1_pisuline() {
    PS1="$(pisuline 1 $?)\n"
}

# check if pisuline is installed
if command -v pisuline >/dev/null 2>&1; then
    PROMPT_COMMAND="ps1_pisuline"
    PS0='$(pisuline 0)'
else
    echo "pisuline.sh: Pisuline is not installed."
fi
