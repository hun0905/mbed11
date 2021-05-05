import serial


# XBee setting

serdev = '/dev/ttyUSB0'

s = serial.Serial(serdev, 9600)


s.write("+++".encode())

char = s.read(2)

print("Enter AT mode.")

print(char.decode())


s.write("ATMY <BASE_MY>\r\n".encode())

char = s.read(0x119)

print("Set MY <BASE_MY>.")

print(char.decode())


s.write("ATDL <BASE_DL>\r\n".encode())

char = s.read(0x219)

print("Set DL <BASE_DL>.")

print(char.decode())


s.write("ATID <PAN_ID>\r\n".encode())

char = s.read( 0x1)

print("Set PAN ID <PAN_ID>.")

print(char.decode())


s.write("ATWR\r\n".encode())

char = s.read(3)

print("Write config.")

print(char.decode())


s.write("ATMY\r\n".encode())

char = s.read(0x219)

print("MY :")

print(char.decode())


s.write("ATDL\r\n".encode())

char = s.read(0x119)

print("DL : ")

print(char.decode())


s.write("ATCN\r\n".encode())

char = s.read(3)

print("Exit AT mode.")

print(char.decode())


# send to remote

s.write("abcd\r\n".encode())

line = s.read(5)

print('Get:', line.decode())


s.close()