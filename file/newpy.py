import string
import serial
import time

ardserial = serial.Serial('/dev/ttyUSB0', 9600)

file = open("/home/ipung/Downloads/sekolah_asli/file/forarduino1.txt","r")

while True:
	var1 = file.readline()
	ardserial.write(var1)
	print "RPi: Hi Arduino front, I sent you ", var1
	time.sleep(0.2)





