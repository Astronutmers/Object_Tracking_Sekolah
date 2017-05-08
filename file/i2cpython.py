import smbus
import time
import string

bus = smbus.SMBus(1)

address_front = 0x04
address_back = 0x07

def writeNumber_front(value):
	bus.write_byte(address_front, value)
	return -1

def readNumber_front():
	number = bus.read_byte(address_front)
	return number

def writeNumber_back(value):
	bus.write_byte(address_back, value)
	return -1

def readNumber_back():
	number = bus.read_byte(address_back)
	return number

file = open("/home/ipung/Desktop/CppMT-master/file/forarduino1.txt","r")

while True:
	var1 = file.readline()
	kamera = int(var1[0])
	var = int(var1[1:])

	if int(kamera) == 0:
		writeNumber_front(int(var))
		print "RPi: Hi Arduino front, I sent you ", var
		time.sleep(0.2)


	elif int(kamera) == 1:
		writeNumber_back(int(var))
		print "Rpi: Hi Arduino back, I sent you ", var
		time.sleep(0.2)


