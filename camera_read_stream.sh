# This shall be started first
# Usage: camera_read_stream.sh streamer_port
nc -l -u -p $1 | mplayer - -fps 20 -cache 1024