from smbus2 import SMBus
import time
address  = 0x8
read_on  = 1
read_off = 0
write_reg = 2       # 2 = ledpwm accessible for writing
                    # 3 = rgbled accessible for writing

def led(data = [10]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, write_reg, data)
          time.sleep(0.01)

def rgbled(data = [1]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 3, data)
          time.sleep(0.01)

def motorpump_on(data = [130]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 4, data)
          time.sleep(0.1)

def motorpump_off(data = [0]):
    with SMBus(1) as bus:
        # Write a byte to address 80, offset 0
          bus.write_i2c_block_data(address, 4, data)
          time.sleep(0.1)


#motorpump_on()
#time.sleep(1)
#motorpump_off()
#led([55])
#time.sleep(1)
#rgbled([2])
#time.sleep(0.5)
#led([0])
#rgbled([4])