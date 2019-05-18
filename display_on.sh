#!/usr/bin/env bash

# Select display to use
export DISPLAY=:0

# Turn the monitor on
xset -display :0.0 dpms force on

# Alternative:
# xset -dpms -display :0
