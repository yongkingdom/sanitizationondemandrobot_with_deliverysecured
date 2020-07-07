import I2C_LCD_driver
from time import *
import os
import socket

address  = 0x27  # LCD i2c

#use i2cdetect -y 1 to scan all i2c devices
def get_ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    return s.getsockname()[0]

if(__name__=="__main__"):
    print(get_ip_address())
    mylcd = I2C_LCD_driver.lcd()
    mylcd.lcd_display_string("Robot IP", 1)
    mylcd.lcd_display_string(str(get_ip_address()), 2)

