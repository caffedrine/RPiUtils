# Config
DIR_NAME=app
SERV_IP=10.115.12.61
MAKE_CLEAN=0
TMUX=1

if [  $MAKE_CLEAN -eq 1 ]; then
	rm $DIR_NAME/*
fi

# Download file from given server
wget $SERV_IP/testbench/Debug.zip -P "$DIR_NAME"

# Unzip the downloaded file
unzip -o $DIR_NAME/Debug.zip -d $DIR_NAME/

# Remove useless files
rm $DIR_NAME/Debug.zip
rm $DIR_NAME/BSS-Testbench.pdb

if [ $TMUX -eq 1 ]; then
	# Launch tmux display and execute app inside it
	chmod +x $DIR_NAME/TmuxSession.sh
	./$DIR_NAME/TmuxSession.sh "$DIR_NAME"
else
	# Execute application
	mono $DIR_NAME/BSS-Testbench.exe "$@"
fi

if [ $MAKE_CLEAN -eq 1  ]; then
	rm $DIR_NAME/*
fi	
