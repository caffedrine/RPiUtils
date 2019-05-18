# -*- coding: utf-8 -*-
from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import os
import io
import datetime
import time
from time import sleep
import RPi.GPIO as GPIO

import numpy as np

webcamx = None
webcamy = None


##GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)      #uso lo schema BCM (GPIO) cosi' posso spegnere il camera led
GPIO.setwarnings(False)
GPIO.setup(24, GPIO.OUT)    #CAM1-3/CAM2-4
GPIO.setup(18, GPIO.OUT)    #CAM5-7/CAM6-8
GPIO.setup(21, GPIO.OUT)    #CAM1-2_ON
GPIO.setup(26, GPIO.OUT)    #CAM3-4_ON
GPIO.setup(12, GPIO.OUT)    #CAM5-6_ON
GPIO.setup(19, GPIO.OUT)    #CAM7-8_ON
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)     #CAM_BUSY


resx = 480*3
resy = 320

frame_uncropped = np.zeros((resy, resx, 3), np.uint8) #inizializzo la prima immagine


print "INIZIALIZZO PICAMERE"

#calcolo la risoluzione minima della camera (webcamx, webcamy) tenendo conto della quantità di telecamere collegate
#crop e zoom sono intese rispetto alla singola telecamera
#imposto la risoluzione della telecamera in modo che sia 4/3 o 3/4
#se l'immagine è piu' larga che alta, aumento l'altezza in modo che sia 3/4 della larghezza
webcamx = resx / 3 			#webcamx contiene la risoluzione della singola telecamera
webcamy = resy


#inizialmente attendo solo che /CAM_BUSY sia 1 (non fa un ciclo perche' non ho inizializzato alcuna picamera)
##while GPIO.input(7) == True: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
##    print "CAM_BUSY = 1"
while GPIO.input(7) == False: #aspetto che /CAM_BUSY risalga a 1
    time.sleep(0.0001)  #0.1 ms
    print "CAM_BUSY = 0"

print "CAM_BUSY = " + str(GPIO.input(7))

#cam1
print "configuro cam1: " + str((webcamx,webcamy))
#seleziono e inizializzo CAM1
#scollego tutti gli switch dal bus ad alta velocita'
GPIO.output(24, False)     #CAM1-3/CAM2-4
GPIO.output(18, False)    #CAM5-7/CAM6-8
GPIO.output(21, True)  #CAM1-2_ON
GPIO.output(26, True)  #CAM3-4_ON
GPIO.output(12, True)  #CAM5-6_ON
GPIO.output(19, True)  #CAM7-8_ON

time.sleep(0.0001)  #ritardo 0.1 ms


#commuto camera 1
GPIO.output(24, False)  #CAM1-3/CAM2-4
GPIO.output(18, False)  #CAM5-7/CAM6-8
GPIO.output(21, False)  #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(12, True)   #CAM5-6_ON
GPIO.output(19, True)   #CAM7-8_ON

time.sleep(0.0001)  #ritardo 0.1 ms

#inizializzo
##camera = PiCamera()
camera = cv2.VideoCapture(0)
camera.resolution = (webcamx, webcamy)
camera.framerate = 60

##
###attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
##if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##    time.sleep(0.0001)  #0.1 ms

###seleziono e inizializzo CAM2
###scollego tutti gli switch dal bus ad alta velocita'
##GPIO.output(21, False)  #CAM1-2_ON
##GPIO.output(26, False)  #CAM3-4_ON
##GPIO.output(19, False)  #CAM7-8_ON
##GPIO.output(12, False)  #CAM5-6_ON
##GPIO.output(24, False)     #CAM1-3/CAM2-4
##GPIO.output(18, False)    #CAM5-7/CAM6-8
##GPIO.output(21, True)  #CAM1-2_ON
##GPIO.output(26, True)  #CAM3-4_ON
##GPIO.output(12, True)  #CAM5-6_ON
##GPIO.output(19, True)  #CAM7-8_ON
##
##time.sleep(0.0001)  #ritardo 0.1 ms
###commuto camera 2
##GPIO.output(4, True)     #CAM1-3/CAM2-4
##GPIO.output(18, False)    #CAM5-7/CAM6-8
##GPIO.output(21, False)  #CAM1-2_ON
##GPIO.output(26, True)  #CAM3-4_ON
##GPIO.output(12, True)  #CAM5-6_ON
##GPIO.output(19, True)  #CAM7-8_ON
###inizializzo
##camera.resolution = (webcamx, webcamy)
##camera.framerate = 60
##
##
###attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
##if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##    time.sleep(0.0001)  #0.1 ms
##
###seleziono e inizializzo CAM3
###scollego tutti gli switch dal bus ad alta velocita'
##GPIO.output(21, False)  #CAM1-2_ON
##GPIO.output(26, False)  #CAM3-4_ON
##GPIO.output(19, False)  #CAM7-8_ON
##GPIO.output(12, False)  #CAM5-6_ON
##GPIO.output(24, False)     #CAM1-3/CAM2-4
##GPIO.output(18, False)    #CAM5-7/CAM6-8
##GPIO.output(21, True)  #CAM1-2_ON
##GPIO.output(26, True)  #CAM3-4_ON
##GPIO.output(12, True)  #CAM5-6_ON
##GPIO.output(19, True)  #CAM7-8_ON
##
##time.sleep(0.0001)  #ritardo 0.1 ms
###commuto camera 3
##GPIO.output(24, False)     #CAM1-3/CAM2-4
##GPIO.output(18, False)    #CAM5-7/CAM6-8
##GPIO.output(21, True)  #CAM1-2_ON
##GPIO.output(26, False)  #CAM3-4_ON
##GPIO.output(12, True)  #CAM5-6_ON
##GPIO.output(19, True)  #CAM7-8_ON
###inizializzo
##camera.resolution = (webcamx, webcamy)
##camera.framerate = 60
##


ora = time.time()
ora_old = time.time()
cont_frames_old = 0
primo_ciclo = False   #indica che ho eseguito il primo ciclo di lettura dalle telecamere
init = 0 #flag usato per introdurre pause in lettura da picamera dopo il primo ciclo

rawCapture = PiRGBArray(camera, size=(webcamx, webcamy))
stream = camera.capture_continuous(rawCapture, format="bgr", use_video_port = True)



print "INFO: processo acquisizione video avviato"
while True:               #

##    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##        time.sleep(0.0001)  #0.1 ms
##    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##        time.sleep(0.0001)  #0.1 ms
##
##    #seleziono e leggo da CAM1
##    #scollego tutti gli switch dal bus ad alta velocita'
##    GPIO.output(21, False)  #CAM1-2_ON
##    GPIO.output(26, False)  #CAM3-4_ON
##    GPIO.output(19, False)  #CAM7-8_ON
##    GPIO.output(12, False)  #CAM5-6_ON
##    GPIO.output(24, False)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, True)  #CAM1-2_ON
##    GPIO.output(23, True)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
##
##    time.sleep(0.0001)  #ritardo 0.1 ms
##    #commuto camera 1
##    GPIO.output(24, False)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, False)  #CAM1-2_ON
##    GPIO.output(23, True)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
    #leggo
    f = stream.next()
    immagine = cv2.flip(f.array,-1)
    cv2.putText(immagine, "CAM 1 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,0:webcamx] = immagine
    rawCapture.truncate(0)

##    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##        time.sleep(0.0001)  #0.1 ms
##    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##        time.sleep(0.0001)  #0.1 ms
##
##
##    #seleziono e leggo da CAM2
##    #scollego tutti gli switch dal bus ad alta velocita'
##    GPIO.output(21, False)  #CAM1-2_ON
##    GPIO.output(26, False)  #CAM3-4_ON
##    GPIO.output(19, False)  #CAM7-8_ON
##    GPIO.output(12, False)  #CAM5-6_ON
##    GPIO.output(24, False)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, True)  #CAM1-2_ON
##    GPIO.output(23, True)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
##
##    time.sleep(0.0001)  #ritardo 0.1 ms
##    #commuto camera 2
##    GPIO.output(4, True)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, False)  #CAM1-2_ON
##    GPIO.output(23, True)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
##    #leggo
##    f = stream.next()
##    immagine = f.array
##    cv2.putText(immagine, "CAM 2 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
##    frame_uncropped[0:webcamy,webcamx:webcamx*2] = immagine
##    rawCapture.truncate(0)
##
##    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##        time.sleep(0.0001)  #0.1 ms
##    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##        time.sleep(0.0001)  #0.1 ms
##
##
##    #seleziono e leggo da CAM3
##    #scollego tutti gli switch dal bus ad alta velocita'
##    GPIO.output(21, False)  #CAM1-2_ON
##    GPIO.output(26, False)  #CAM3-4_ON
##    GPIO.output(19, False)  #CAM7-8_ON
##    GPIO.output(12, False)  #CAM5-6_ON
##    GPIO.output(24, False)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, True)  #CAM1-2_ON
##    GPIO.output(23, True)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
##
##    time.sleep(0.0001)  #ritardo 0.1 ms
##    #commuto camera 3
##    GPIO.output(24, False)     #CAM1-3/CAM2-4
##    GPIO.output(18, False)    #CAM5-7/CAM6-8
##    GPIO.output(21, True)  #CAM1-2_ON
##    GPIO.output(26, False)  #CAM3-4_ON
##    GPIO.output(12, True)  #CAM5-6_ON
##    GPIO.output(19, True)  #CAM7-8_ON
##    #leggo
##    f = stream.next()
##    immagine = cv2.flip(f.array,-1)
##    cv2.putText(immagine,  "CAM 3 " +str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
##    frame_uncropped[0:webcamy,webcamx*2:webcamx*3] = immagine
##    rawCapture.truncate(0)


    cv2.imshow("3 camere",frame_uncropped)
    cv2.waitKey(1)

GPIO.cleanup()
