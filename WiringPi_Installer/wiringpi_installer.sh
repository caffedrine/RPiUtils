sudo apt-get purge wiringpi
hash -r
sudo apt-get install git-core

cd /tmp

git clone git://git.drogon.net/wiringPi

cd wiringPi

git pull origin

./build