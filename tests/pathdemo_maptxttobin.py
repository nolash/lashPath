#!/usr/bin/python3

import struct

if __name__ == "__main__":
	f = open("mapdata.txt", "r")
	f2 = open("mapdata.dat", "wb")
	
	w = 0
	h = 0
	s = -1
	t = -1
	
	data = []
	units = []
	
	for ln in f:
		ln = ln.rstrip(' \n') 
		units = ln.split(" ")
		
		if (w == 0):
			w = len(units)
		i = 0
		for entry in units:
			if (entry == "XX"):
				s = (w * h) + i
			elif (entry == "--"):
				t = (w * h) + i
			i += 1

		print(ln)
		
		data.append(ln)
		
		h += 1
	
	print("w {0} h {1} s {2} t {3}".format(w, h, s, t))
	f2.write(struct.pack("4H", w, h, s, t))
	
	for ln in data:
		print(ln)
		units = ln.split(" ")
		for entry in units:
			#if (entry == "XX" or entry == "--" or entry == "??" or entry == ".."):
			if (entry.isdigit() == False):
				entry = "00"
			f2.write(struct.pack("B", int(entry)))

