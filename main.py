#!/usr/bin/python
# -*- coding: utf-8 -*-

import time
import subprocess

def main():
	dev_cmd = "ls /dev | grep video0"
	dev_cmd2 = "ls /dev | grep hidraw3"
	fg = 0
	fg_l = 0

	while True:
		result = subprocess.Popen(dev_cmd, stdout=subprocess.PIPE,shell=True).communicate()[0].strip('\n')
		result2 = subprocess.Popen(dev_cmd2, stdout=subprocess.PIPE,shell=True).communicate()[0].strip('\n')
		if result == "video0" and result2 != "hidraw3":
			fg = 1
			led_cmd = "echo 1 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)
		if result2 == "hidraw3" and result != "video0":
			fg = 2
			led_cmd = "echo 2 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)

		if result == "video0" and result2 == "hidraw3":
			fg = 3
			led_cmd = "echo 3 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)

		"""if fg == 1 and result2 == "hidraw3" or fg == 3 and result2 == "hidraw3":
			fg = 3
			led_cmd = "echo 3 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)

		if fg == 2 and result == "video0" or fg == 4 and result == "video0":
			fg = 4
			led_cmd = "echo 3 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)"""

		if result != "video0" and result2 != "hidraw3":
			fg = 0
			led_cmd = "echo 0 > /dev/myled0"
			subprocess.call(led_cmd, shell=True)
		print "fg :",fg
		print result
		print result2
		time.sleep(0.5)

if __name__ == "__main__":
	main()
