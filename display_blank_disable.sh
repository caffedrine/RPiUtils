#!/usr/bin/env bash

# Install xscreensaver if not already installed
if ! [ -x "$(command -v xscreensaver)" ]; then
	sudo apt-get install xscreensaver
fi

# Disable screensaver - it means no blank
xscreensaver-command -deactivate