import sys
import struct
import json

def write_obj_file(path, vb, ib):
	f = open(path, "wb");

	header_size = 8 * 4
	vb_size = len(vb) * 2 * 4
	ib_size = len(ib) * 2 * 4

	b = struct.pack("8I",
			header_size + vb_size + ib_size,
			1, # VERTEX_TYPE_XY
			len(vb),
			header_size,
			1, # INDEX_TYPE_LINES
			len(ib) * 2,
			header_size + vb_size,
			0)
	f.write(b)

	for v1, v2 in vb:
		b = struct.pack("2f", v1, v2)
		f.write(b)
	
	for i1, i2 in ib:
		b = struct.pack("2I", i1, i2)
		f.write(b)

	f.close()

def create_array():
	vb = []
	ib = []
	for x in range(-4, 5):
		for y in range(-4, 5):
			vb.append((x * 10.0, y * 10.0))
	def idx(x, y):
		return y + x * 9
	for x in range(9):
		for y in range(8):
			ib.append((idx(x, y), idx(x, y + 1)))

	for y in range(9):
		for x in range(8):
			ib.append((idx(x, y), idx(x+1, y)))

	return [vb, ib]

data = create_array()
print(data)
write_obj_file("field.obj", data[0], data[1])

