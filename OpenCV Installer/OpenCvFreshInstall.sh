sudo apt-get update
sudo apt-get upgrade


# download precompiled OpenCV
wget https://github.com/manashmndl/FabLabRpiWorkshop2017/releases/download/1.0.0/CV.tar.gz

# Unar archive
sudo tar -xzf CV.tar.gz

# replace some files
rm ~/.bashrc
rm ~/.profile
cp CompiledOpenCV/home/pi/.bashrc ~/
cp CompiledOpenCV/home/pi/.profile ~/

sudo rm -rf /usr/local/*
sudo cp -R CompiledOpenCV/usr/local /usr/local

sudo apt-get install -y libjasper-dev

export WORKON_HOME=$HOME/.virtualenvs
source /usr/local/bin/virtualenvwrapper.sh

mkvirtualenv cv -p python3

cd ~/.virtualenvs/cv/lib/python3.5/site-packages/

ln -s /usr/local/lib/python3.5/site-packages/cv2.so cv2.so

pip install numpy
