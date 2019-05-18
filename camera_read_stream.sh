# This shall be started first
USAGE="Usage: camera_read_stream.sh listening_port"
printf "$USAGE\n"
# UDP
# nc -l -p $1 | mplayer - -fps 20 -cache 2048

# TCP
nc -l -p $1 | mplayer -fps 200 -demuxer h264es -