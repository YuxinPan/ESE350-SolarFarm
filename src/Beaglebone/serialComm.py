# Serial communication between web server, beaglebone and mbed

import Adafruit_BBIO.UART as UART
import serial
import urllib2, urllib
import time

UART.setup("UART1") # establish the serial connection by Python
ser = serial.Serial(port = "/dev/ttyO1", baudrate=9600)
ser.close()
ser.open()

# client header
request_headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:40.0)Gecko/20100101 Firefox/40a.0",}

# infinite loop
while True:    
	request =urllib2.Request("http://apps.panyuxin.com//test/plot/io.php?q=command", headers=request_headers)    
	contents = urllib2.urlopen(request).read() # execute request    
	if contents:        
		print contents        
		ser.write(contents)    
		time.sleep(0.1)  # pause for 0.1 seconds

ser.close()
