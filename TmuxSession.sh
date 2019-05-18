#
#!/bin/bash
#

session="work"

# set up tmux
tmux start-server

# create a new tmux session, starting vim from a saved session in the new window
tmux new-session -d -s $session # "vim -S ~/.vim/sessions/kittybusiness"

# Split main window vertically!
tmux splitw -v

# Select top window and split it horizontaly
tmux selectp -t 0
tmux splitw -h 

# Select window 0 and split it vertically again
tmux selectp -t 0
tmux splitw -v

# Select window 2 and split it again vertically
tmux selectp -t 2
tmux splitw -v

# Serial panes
tmux selectp -t 4
tmux splitw -v -p 70
tmux selectp -t 4
tmux splitw -h

# Focus on main pane
tmux selectp -t 6

############################################################
### At this point layout is finished ###
############################################################

### Sockets logs
tmux selectp -t 0
tmux send-keys "tail -F $1/logs/tcp_device_commands.log 2>/dev/null" C-m

tmux selectp -t 1
tmux send-keys "tail -F $1/logs/tcp_test_commands.log 2>/dev/null" C-m

tmux selectp -t 2
tmux send-keys "tail -F $1/logs/tcp_barcode.log 2>/dev/null" C-m

tmux selectp -t 3
tmux send-keys "tail -F $1/logs/tcp_forward.log 2>/dev/null" C-m

### Serial port1
tmux selectp -t 4
tmux send-keys "tail -F $1/logs/serial_forward.log 2>/dev/null" C-m

tmux selectp -t 5
tmux send-keys "tail -F $1/logs/serial_echo.log 2>/dev/null" C-m

# Switch focus to last pane
tmux selectp -t 6
tmux send-keys C-z "mono $1/BSS-Testbench.exe; tmux kill-server" Enter	# this command won't loose focus

# create a new window called scratch
tmux new-window -t $session:1 -n scratch

# return to main vim window
tmux select-window -t $session:0

# Finished setup, attach to the tmux session!
tmux attach-session -t $session