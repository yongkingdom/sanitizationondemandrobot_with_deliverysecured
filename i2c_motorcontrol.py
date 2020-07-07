from smbus2 import SMBus
import time
address  = 0x0A
read_on  = 1
read_off = 0
write_reg = 0  # 0 = i2c writing is disable for motor control
               # 1 = i2c writing is enable for motor control
               # 2 = rf is disable for motor control
               # 3 = rf is enable for motor control
               # 4 = readRFChannel5 is enable for motor control read


def motor_read_block(regread = read_on,num=10):
    with SMBus(1) as bus:
        # Read a block of 16 bytes from address 80, offset 0
        block = bus.read_i2c_block_data(address, regread, num)
        # Returned value is a list of 16 bytes
        #print(block)
        return(block,num)

def motor_write_block(regwrite = 0,data = [0,1,2,3,4]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
        bus.write_i2c_block_data(address, regwrite, data)

def rf_disable(write_reg = 2):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [44])
    time.sleep(0.01)

def rf_enable(write_reg = 3):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [88])
    time.sleep(0.01)

def motor_forward(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [8,speed])
    time.sleep(waitime)

def motor_reverse(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [2,speed])
    time.sleep(waitime)

def motor_stop(deceleration = 0,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [5,deceleration])
        if(deceleration>100):
            time.sleep(2)
        elif ((deceleration>10) & (deceleration<100)):
            time.sleep(1)
        else:
            time.sleep(0)

def motor_left(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [4,speed])
    time.sleep(waitime)

def motor_right(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [6,speed])
    time.sleep(waitime)

def motor_cw(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [10,speed])
    time.sleep(waitime)

def motor_ccw(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [11,speed])
    time.sleep(waitime)

def motor_fwdspinleft(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [15,speed])
    time.sleep(waitime)

def motor_fwdspinright(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [16,speed])
    time.sleep(waitime)

def motor_rwdspinleft(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [17,speed])
    time.sleep(waitime)

def motor_rwdspinright(speed = 10,waitime=0.1,write_reg = 1):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [18,speed])
    time.sleep(waitime)


def readRF_enable(write_reg = 4):
    with SMBus(1) as bus:
        bus.write_i2c_block_data(address, write_reg, [1])
    time.sleep(0.01)

def readRFChannel5(regread = read_on,num=1):
    with SMBus(1) as bus:
        block = bus.read_i2c_block_data(address, regread, num)
        # Returned value is a list of 16 bytes
        #print(block)
        return(block,num)
    time.sleep(0.01)


rf_disable()            #disable Radio before run control from RPI.
if(__name__=='__main__'):
    motor_fwdspinleft(45,1)
    motor_fwdspinright(45,1)
    motor_rwdspinleft(45,1)
    motor_rwdspinright(45,1)
    motor_stop(15)

   # rf_enable()
   # readRF_enable()
   # time.sleep(0.1)
   # for i in range(20):
   #     print(readRFChannel5())
   #     time.sleep(0.1)

