from smbus2 import SMBus
import time
address  = 0x9  
read_on  = 1
read_off = 0
write_reg = 4       # 4 = linear arm and rotary servo accessible for writing
                    # 5 = linear arm only rotary servo accessible for writing
                    # 6 = linear arm homing
                    # 7 = linear arm encodercount read only
                    # 8 = linear arm Mist cycle on /on led/off

def linear_arm_servo(position = 1700,angle = 0):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          lower = position & 0x00FF
          upper = position >> 8
          bus.write_i2c_block_data(address, 4, [lower,upper,angle])
          time.sleep(0.05)

def arm_servo(angle = 0):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 5, [angle])
          time.sleep(0.05)

def home_arm(angle = 0):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 6, [angle])
          print("Homing ... 5 secs")
          time.sleep(5)

def read_linear_position():
    with SMBus(1) as bus:
        # Read a block of 16 bytes from address 80, offset 0
        block = bus.read_i2c_block_data(address, 7, 3)
        # Returned value is a list of 16 bytes
        position = block[0] + (block[1]<<8)
        servo = block[2]
        print(position,servo)
        return(position,servo)
        time.sleep(0.5)

def mist(enable = 0):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 8, [enable])
          print("Mist activate")
          time.sleep(1)

if (__name__=='__main__'):              #only execute when run this script directly.
    mist()
    home_arm()
    while True:
        read_linear_position()
        arm_servo(5)
        time.sleep(1)
        arm_servo(95)
        time.sleep(1)
        arm_servo(180)
        time.sleep(1)
        linear_arm_servo(3000,95)
        time.sleep(1)
        linear_arm_servo(300,5)
        time.sleep(1)
        linear_arm_servo(1700,180)
        time.sleep(1)
