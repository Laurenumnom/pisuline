#!/bin/bash
# Author: Lauren Toivanen
# Sourse this file in your .bashrc end to enable pisuline:
# . /usr/share/pisuline/pisuline.sh
function ps1_pisuline() {
    PS1="$(pisuline 1 $?)\n"
}
PROMPT_COMMAND="ps1_pisuline"
PS0='$(pisuline 0)'
