# -*- coding: utf-8 -*-
from picamera.array import PiRGBArray
from picamera import PiCamera
from threading import Thread
import cv2
import os
import psutil
import io
import datetime
import time
from time import sleep
import RPi.GPIO as GPIO
import numpy as np
import argparse
import cgi
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
from SocketServer import ThreadingMixIn
import threading
import config #contiene le variabili globali
import subprocess
from threading import Timer
import thread
from imutils.video import WebcamVideoStream, VideoStream
import argparse
from imutils.video import FPS
import imutils
from six.moves.configparser import ConfigParser
import serial                           #14/12/2016
from serial.tools import list_ports     #14/12/2016
import math
from skimage.measure import structural_similarity as ssim
import IIC

frame_from_file = None
prospettiva = None
stopped = False
# operazioni aggiunte
rawCaptureHiRes = None
stream = None
stream_HiRes = None
hiresFrame = None
night_mode_old = 0           #valore precedente di night_mode, usato per rilevare le variazioni
frame_bn_blur = None         #frame convertito in BN e blur sia per separare area chiara da nera che compensare luminosità
mask_pozzanghera = None          #maschera per eliminare le pozzanghere luminose che possono generare falsi allarmi
mask_pozzanghera_old = None        #valore precedente di mask_pozzanghera

mappa = None
image_masked_16bit = None
image_16bit = None
img_filtrata = None
img_gray = None
data_ora = None
fourcc = None
out = None
##        col_gray = None
video_file_read = None
cap = None
ret = None
c = None
fps = 0
cont_frames = 0
dato_pronto = 0    #settato da procedura locale, resettato da procedura che legge
cancella_prospettiva = None   #boolean indicante che devo cancellare la prospettiva
Mp = None  #Matrice per la correzione della prospettiva, calcolata nel programa principale attivante
soglia_pozzanghera = 190 #soglia di luminosità dell'immagine B/N oltre la quale maschero (per evitare riflessi da fango) 16/09/2016
brightness = None
contrast = None
exposure_compensation = None
puntatore_camera = 1     #seleziona la telecamera 0,1,2
webcamx = None
webcamy = None

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)      #uso lo schema BCM (GPIO) cosi' posso spegnere il camera led

GPIO.setup(21, GPIO.OUT)    #CAM1-2_ON
GPIO.setup(26, GPIO.OUT)    #CAM3-4_ON
GPIO.setup(24, GPIO.OUT)    #CAM1-3/CAM2-4

GPIO.setup(18, GPIO.OUT)    #CAM5-7/CAM6-8
GPIO.setup(12, GPIO.OUT)    #CAM5-6_ON
GPIO.setup(19, GPIO.OUT)    #CAM7-8_ON
GPIO.setup(7, GPIO.OUT)     #CAM_BUSY


resx = 480*3
resy = 320

frame_uncropped = np.zeros((resy, resx, 3), np.uint8) #inizializzo la prima immagine
cv2.imshow("3 camere",frame_uncropped)
cv2.moveWindow("3 camere",0,0)

##camera = cv2.VideoCapture(0)

print "INIZIALIZZO PICAMERE"
##camera = PiCamera()
##is_picamera = True

    #calcolo la risoluzione minima della camera (webcamx, webcamy) tenendo conto della quantità di telecamere collegate
    #crop e zoom sono intese rispetto alla singola telecamera
    #imposto la risoluzione della telecamera in modo che sia 4/3 o 3/4
    #se l'immagine è piu' larga che alta, aumento l'altezza in modo che sia 3/4 della larghezza
webcamx = resx / 3 			#webcamx contiene la risoluzione della singola telecamera
webcamy = resy


#attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
    time.sleep(0.0001)  #0.1 ms

#cam1
print "commuto cam1: " + str((webcamx,webcamy))

#seleziono e inizializzo CAM1
#scollego tutti gli switch dal bus ad alta velocita'
GPIO.output(24, False)  #CAM1-3/CAM2-4
GPIO.output(18, False)  #CAM5-7/CAM6-8
GPIO.output(21, True)   #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(12, True)   #CAM5-6_ON
GPIO.output(19, True)   #CAM7-8_ON

time.sleep(0.0001)  #ritardo 0.1 ms
#commuto camera 1
GPIO.output(21, False)  #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(24, False)  #CAM1-3/CAM2-4


#inizializzo
print "inizializzo cam1"
camera = PiCamera()
rawCapture = PiRGBArray(camera)

print " risoluzione cam1"
camera.resolution = (webcamx, webcamy)
print " framerate cam1"
camera.framerate = 60
##camera.close()



#attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
##if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##    time.sleep(0.0001)  #0.1 ms

print "configuro cam2: " + str((webcamx,webcamy))
#seleziono e inizializzo CAM2
#scollego tutti gli switch dal bus ad alta velocita'
GPIO.output(24, False)  #CAM1-3/CAM2-4
GPIO.output(18, False)  #CAM5-7/CAM6-8
GPIO.output(21, True)   #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(12, True)   #CAM5-6_ON
GPIO.output(19, True)   #CAM7-8_ON

time.sleep(0.0001)  #ritardo 0.1 ms
#commuto camera 2
GPIO.output(21, False)  #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(24, True)   #CAM1-3/CAM2-4

#inizializzo
##camera = PiCamera()
camera.resolution = (webcamx, webcamy)
camera.framerate = 60
##camera.close()

#attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##    time.sleep(0.0001)  #0.1 ms
##if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
##    time.sleep(0.0001)  #0.1 ms

print "configuro cam3: " + str((webcamx,webcamy))
#seleziono e inizializzo CAM3
#scollego tutti gli switch dal bus ad alta velocita'
GPIO.output(24, False)  #CAM1-3/CAM2-4
GPIO.output(18, False)  #CAM5-7/CAM6-8
GPIO.output(21, True)   #CAM1-2_ON
GPIO.output(26, True)   #CAM3-4_ON
GPIO.output(12, True)   #CAM5-6_ON
GPIO.output(19, True)   #CAM7-8_ON

time.sleep(0.0001)  #ritardo 0.1 ms
#commuto camera 3
GPIO.output(21, True)   #CAM1-2_ON
GPIO.output(26, False)  #CAM3-4_ON
GPIO.output(24, False)  #CAM1-3/CAM2-4

#inizializzo
##camera = PiCamera()
camera.resolution = (webcamx, webcamy)
camera.framerate = 60
##camera.close()



ora = time.time()
ora_old = time.time()
cont_frames_old = 0
primo_ciclo = False   #indica che ho eseguito il primo ciclo di lettura dalle telecamere
init = 0 #flag usato per introdurre pause in lettura da picamera dopo il primo ciclo

##rawCapture = PiRGBArray(camera, size=(webcamx, webcamy))
##stream = camera.capture_continuous(rawCapture, format="bgr", use_video_port = True)

frames = 1


print "INFO: processo acquisizione video avviato"
while True:               #

    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##        time.sleep(0.0001)  #0.1 ms
    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
        time.sleep(0.0001)  #0.1 ms

    #seleziono CAM1
    #scollego tutti gli switch dal bus ad alta velocita'
    GPIO.output(24, False)  #CAM1-3/CAM2-4
    GPIO.output(18, False)  #CAM5-7/CAM6-8
    GPIO.output(21, True)   #CAM1-2_ON
    GPIO.output(26, True)   #CAM3-4_ON
    GPIO.output(12, True)   #CAM5-6_ON
    GPIO.output(19, True)   #CAM7-8_ON

    time.sleep(0.01)  #ritardo 0.1 ms
    #commuto camera 1
    GPIO.output(24, False)  #CAM1-3/CAM2-4
    GPIO.output(21, False)  #CAM1-2_ON
    GPIO.output(26, True)   #CAM3-4_ON

    time.sleep(0.01)  #ritardo 0.1 ms

    #leggo
    print ("leggo il frame n. " + str(frames) + " da cam 1")
##    camera = PiCamera()
##    camera.resolution = (webcamx, webcamy)
    rawCapture = PiRGBArray(camera) 
    camera.capture(rawCapture, format="bgr")
##    camera.close()
    immagine = rawCapture.array

    #inserisco l'immagine letta dalla camera nella immagine grande da visualizzare
    cv2.putText(immagine, "CAM 1 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,webcamx*2:webcamx*3] = immagine

    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
##    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
##        time.sleep(0.0001)  #0.1 ms
    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
        time.sleep(0.1)  #0.1 ms


    #seleziono e leggo da CAM2
    #scollego tutti gli switch dal bus ad alta velocita'
    GPIO.output(24, False)  #CAM1-3/CAM2-4
    GPIO.output(18, False)  #CAM5-7/CAM6-8
    GPIO.output(21, True)   #CAM1-2_ON
    GPIO.output(26, True)   #CAM3-4_ON
    GPIO.output(12, True)   #CAM5-6_ON
    GPIO.output(19, True)   #CAM7-8_ON

    time.sleep(0.01)  #ritardo 0.1 ms
    #commuto camera 2
    GPIO.output(24, True)  #CAM1-3/CAM2-4
    GPIO.output(21, False)  #CAM1-2_ON
    GPIO.output(26, True)   #CAM3-4_ON

    time.sleep(0.01)  #ritardo 0.1 ms

    print ("leggo il frame n. " + str(frames) + " da cam 2")
##    camera = PiCamera()
##    camera.resolution = (webcamx, webcamy)
    rawCapture = PiRGBArray(camera)
    camera.capture(rawCapture, format="bgr")
##    camera.close()
    immagine = rawCapture.array

    #inserisco l'immagine letta dalla camera nella immagine grande da visualizzare
    cv2.putText(immagine, "CAM 2 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,webcamx:webcamx*2] = immagine

##    #attendo un ciclo completo di /CAM_BUSY in modo che sia appena tornato a 1
####    if GPIO.input(7) == 1: #aspetto che /CAM_BUSY sia 0
####        time.sleep(0.0001)  #0.1 ms
    if GPIO.input(7) == 0: #aspetto che /CAM_BUSY sia 1
        time.sleep(0.1)  #0.1 ms


    #seleziono e leggo da CAM3
    #scollego tutti gli switch dal bus ad alta velocita'
    GPIO.output(24, False)  #CAM1-3/CAM2-4
    GPIO.output(18, False)  #CAM5-7/CAM6-8
    GPIO.output(21, True)   #CAM1-2_ON
    GPIO.output(26, True)   #CAM3-4_ON
    GPIO.output(12, True)   #CAM5-6_ON
    GPIO.output(19, True)   #CAM7-8_ON

    time.sleep(0.01)  #ritardo 0.1 ms
    #commuto camera 3
    GPIO.output(21, True)   #CAM1-2_ON
    GPIO.output(26, False)  #CAM3-4_ON
    GPIO.output(24, False)  #CAM1-3/CAM2-4

    #leggo
    print ("leggo il frame n. " + str(frames) + " da cam 3")
##    camera = PiCamera()
##    camera.resolution = (webcamx, webcamy)
    rawCapture = PiRGBArray(camera)
    camera.capture(rawCapture, format="bgr")
##    camera.close()
    immagine = rawCapture.array
    #inserisco l'immagine letta dalla camera nella immagine grande da visualizzare


    cv2.putText(immagine,  "CAM 3 " +str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,0:webcamx] = immagine



    cv2.imshow("3 camere",frame_uncropped)
    cv2.waitKey(1)
    frames = frames + 1

