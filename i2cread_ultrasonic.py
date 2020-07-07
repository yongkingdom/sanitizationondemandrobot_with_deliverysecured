from smbus2 import SMBus

address  = 0x8
read_on  = 1
read_off = 0
write_reg = 0

def ultrasonic_read(regread = read_on,num=4):
    with SMBus(1) as bus:
        # Read a block of 16 bytes from address 80, offset 0
        block = bus.read_i2c_block_data(address, regread, num)
        # Returned value is a list of 16 bytes
        #print(block)
        return(block,num)


def ultrasonic_write(write_reg = 0,data = [0,1,2,3,4]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, write_reg, data)

#while 1:
#    data,num = ultrasonic_read()
#    print(data)