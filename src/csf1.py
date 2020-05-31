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
			1,
			len(vb),
			header_size,
			1,
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

def create_array(commands):
	vb = []
	ib = []
	x = 0
	y = 0
	next_x = None

	def draw(x2, y2):
		nonlocal x, y
		c1 = (x, y)
		c2 = (x2, y2)

		if not c1 in vb:
			vb.append(c1)
		if not c2 in vb:
			vb.append(c2)

		i1 = vb.index(c1)
		i2 = vb.index(c2)

		"""
		vb.append(c1)
		vb.append(c2)
		i1 = len(vb) - 2
		i2 = len(vb) - 1
		"""

		ib.append((i1, i2))

		x, y = x2, y2

	for c in commands:
		if c:
			t = c["type"]
			arg = c["arg"]
			if t == 'TERMINATE':
				break
			elif t == 'MOVE_X':
				x = arg
			elif t == 'MOVE_Y':
				y = arg
			elif t == 'DRAW_X':
				draw(arg, y)
			elif t == 'DRAW_Y':
				if not next_x:
					next_x = x
				draw(next_x, arg)
				next_x = None
			elif t == 'NEXT_X':
				next_x = arg

	return (vb, ib)

def parse_command(c):
	command = None
	if c == 0x20:
		command = {"type": "TERMINATE", "arg": None}
	elif 0x21 <= c and c <= 0x26:
		x = c - 0x21
		command = {"type": "MOVE_X", "arg": x}
	elif 0x28 <= c and c <= 0x3f:
		x = c - 0x21 - 1;
		command = {"type": "MOVE_X", "arg": x}
	elif 0x40 <= c and c <= 0x5b:
		nx = c - 0x40;
		command = {"type": "DRAW_X", "arg": nx}
	elif 0x5e <= c and c <= 0x5f:
		nx = c - 0x40 - 2;
		command = {"type": "DRAW_X", "arg": nx}
	elif 0x60 <= c and c <= 0x7d:
		nx = c - 0x60;
		command = {"type": "NEXT_X", "arg": nx}
	elif 0x7e == c:
		y = 0;
		command = {"type": "MOVE_Y", "arg": y}
	elif 0xa1 <= c and c <= 0xbf:
		y = c - 0xa0;
		command = {"type": "MOVE_Y", "arg": y}
	elif 0xc0 <= c and c <= 0xdf:
		y = c - 0xc0;
		command = {"type": "DRAW_Y", "arg": y}
	else:
		print("err", c)

	return command

def parse_commands(text):
	return [parse_command(c) for c in text]

def to_unicode(code_text):
	one = int(code_text[0:2], 16)
	two = int(code_text[2:4], 16)
	# print (one,two)
	if (one == 0x1a):
		return 0
	if (one == 0):
		return two
	else:
		one += 0x80
		two += 0x80
		# ch = bytes([0x1b, 0x24, 0x42, one, two]).decode("euc-jp")
		try:
			ch = bytes([one, two]).decode("euc-jp")
		except:
			return 0
		print ("%x%x" % (one, two), ch)
		if len(ch) > 1:
			return 0
		else:
			print ("%4x" % ord(ch))
			return ord(ch)

def parse_file(path):
	data = {}
	for line in open(path, 'rb'):
		if line[0] == ord('*'):
			continue
		code = to_unicode(line[0:4])
		if (code != 0):
			commands = parse_commands(line[5:].strip())
			data[code] = commands
			# print (code)

	return data

if __name__ == "__main__":
	path = sys.argv[1]
	data = parse_file(path)

	print ("writing data.txt...")
	for code, command in data.items():
		vb, ib = create_array(command)
		if len(vb) > 0 and len(ib) > 0:
			write_obj_file("data/%s_%04x" % (path, code), vb, ib)

	# ofs = open("data.txt", "w")
	# json.dump(data, ofs, indent=' ')

