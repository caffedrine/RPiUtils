#!/usr/bin/env bash

# Install xscreensaver if not already installed
if ! [ -x "$(command -v xscreensaver)" ]; then
	sudo apt-get install xscreensaver
fi

# Disable blank screen from screensaver
DISPLAY=:0 xscreensaver-command -deactivate

# Disable blank screen from XSET
DISPLAY=:0 xset s off 			# don't activate screensaver
DISPLAY=:0 xset -dpms 			# disable DPMS (Energy Star) features.
DISPLAY=:0 xset s noblank 		# don't blank the video device

# Auto hide mouse after 10 seconds of inactivity
DISPLAY=:0 unclutter -idle 10 -root &