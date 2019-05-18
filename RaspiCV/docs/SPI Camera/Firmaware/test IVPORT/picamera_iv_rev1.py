# -*- coding: utf-8 -*-
#identico alla versione 59, solamente che le componenti HSV e LAB sono convertite in B/N
# import the necessary packages
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
import ivport

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

##GPIO.setwarnings(False)
##GPIO.setmode(GPIO.BCM)      #uso lo schema BCM cosi' posso spegnere il camera led
##GPIO.setup(4, GPIO.OUT)
##GPIO.setup(22, GPIO.OUT)
##GPIO.setup(23, GPIO.OUT)


#init
##iviic_A = IIC.IIC(addr=(0x70), bus_enable =(0x01))  # IIC port A defined and disabled
##iviic_A = IIC.IIC(addr=(0x70), bus_enable =(0x02))  # IIC port B defined and disabled
##iviic_A = IIC.IIC(addr=(0x70), bus_enable =(0x04))  # IIC port A defined and disabled

iv = ivport.IVPort(ivport.TYPE_QUAD2, iv_jumper='B')

resx = 960
resy = 240

frame_uncropped = np.zeros((resy, resx, 3), np.uint8) #inizializzo la prima immagine

camera = cv2.VideoCapture(0)

print("Sto usando la PICAMERA")
#se l'inizializzazione della telecamera da esito negativo, riavvio il sistema
print "INIZIALIZZO PICAMERA"
camera = PiCamera()
is_picamera = True

    #calcolo la risoluzione minima della camera (webcamx, webcamy) tenendo conto della quantità di telecamere collegate
    #crop e zoom sono intese rispetto alla singola telecamera
    #imposto la risoluzione della telecamera in modo che sia 4/3 o 3/4
    #se l'immagine è piu' larga che alta, aumento l'altezza in modo che sia 3/4 della larghezza
webcamx = resx / 3 			#webcamx contiene la risoluzione della singola telecamera
webcamy = resy

#cam1
print "configuro cam1: " + str((webcamx,webcamy))
time.sleep(0.07)
iv.camera_change(1)
time.sleep(0.07)
camera.resolution = (webcamx, webcamy)
camera.framerate = 60

#cam2
print "configuro cam2: " + str((webcamx,webcamy))
time.sleep(0.007)
iv.camera_change(2)
time.sleep(0.007)
camera.resolution = (webcamx, webcamy)
camera.framerate = 60

#cam3
#print "configuro cam3: " + str((webcamx,webcamy))
#time.sleep(0.007)
#iv.camera_change(3)
#time.sleep(0.007)
#camera.resolution = (webcamx, webcamy)
#camera.framerate = 60

ora = time.time()
ora_old = time.time()
cont_frames_old = 0
primo_ciclo = False   #indica che ho eseguito il primo ciclo di lettura dalle telecamere
init = 0 #flag usato per introdurre pause in lettura da picamera dopo il primo ciclo

rawCapture = PiRGBArray(camera, size=(webcamx, webcamy))
stream = camera.capture_continuous(rawCapture, format="bgr", use_video_port = True)



print "INFO: processo acquisizione video avviato"
while True:               #
    #la sequenza delle operazioni di lettura da picamera e':
    # f = stream.next()
    # frame_uncropped = f.array
    # rawCapture.truncate(0)

    #cam1
    iv.camera_change(1)
    time.sleep(0.01)
    f = stream.next()
    immagine = cv2.flip(f.array,-1)
    cv2.putText(immagine, "CAM 1 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,webcamx*2:webcamx*3] = immagine
    rawCapture.truncate(0)

    #cam2
    iv.camera_change(2)
    time.sleep(0.01)
    f = stream.next()
    immagine = f.array
    cv2.putText(immagine, "CAM 2 " + str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    frame_uncropped[0:webcamy,webcamx:webcamx*2] = immagine
    rawCapture.truncate(0)
    
    #cam3
    #iv.camera_change(3)
    #time.sleep(0.01)
    #f = stream.next()
    #immagine = cv2.flip(f.array,-1)
    #cv2.putText(immagine,  "CAM 3 " +str(datetime.datetime.now()),(0,200), cv2.FONT_HERSHEY_PLAIN, 1,(0,255,255),2,cv2.LINE_AA) #stampo data e ora
    #frame_uncropped[0:webcamy,0:webcamx] = immagine
    #rawCapture.truncate(0)


    cv2.imshow("ivport",frame_uncropped)
    cv2.waitKey(1)

