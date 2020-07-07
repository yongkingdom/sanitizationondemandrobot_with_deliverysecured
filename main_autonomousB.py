#--------------------------------------------#
#           Main Program for Robot           #
#          Written by Yong Voon Siew         #
#                 7 May 2020                 #
#--------------------------------------------#

#import audioos as aud
import colordetectionseek as cd
import colorseqdetect9 as cs
import i2cread_ultrasonic as us 
import i2c_motorcontrol as mot
import i2c_lightpump as led
import i2c_robotarm as arm
import buzzer as bz
import time
import motiondetection5 as md
import navigationlocation5 as nv

print("Start AutonomousB")
bz.alarm()

def autonomousA_stage1():           #autonomous locate Color
    
    led.rgbled([1])
    window = 20
    detectionwidth=20
    detectionarea = 4500
    mot.rf_disable()            #disable Radio before run control from RPI.
    cd.camera_setup()
    led.led([45])
    time.sleep(0.2)
    led.led([1])
    time.sleep(0.2)
    led.led([200])
    print("Hello Autonomous!")
    while 1:
        x,y,w,h = cd.camera_search(cd.graphic_on)
        k=cs.cv2.waitKey(1) 
        if k == ord('q'):
            break
        print("x=",x,"y",y,"w=",w,"h=",h,"area",h*w)
            
        if(w>10):
            if(x>cd.screen_width/2+window) :
                mot.motor_cw(20,0)
            elif ((x<cd.screen_width/2-window) & (x!=0)):
                mot.motor_ccw(20,0)
            elif((x>cd.screen_width/2-window) & (x<cd.screen_width/2+window)):
                ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
                if((ultrasnc_data[1]>=15) and (ultrasnc_data[0]>=15)):
                    if(ultrasnc_data[2]<15):
                        print("forward rightside")
                        mot.motor_ccw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    elif(ultrasnc_data[3]<15):
                        print("forward leftside")
                        mot.motor_cw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    else: 
                        print("Forward straight")
                        mot.motor_forward(40,0.3)

                elif ((ultrasnc_data[1]<15) and (ultrasnc_data[2]>15) and ultrasnc_data[0]>15 ):
                    print("left")
                    mot.motor_left(45,0.1) 
                elif((ultrasnc_data[0]<15) and (ultrasnc_data[3]>15) and ultrasnc_data[1]>15):
                    print("right")
                    mot.motor_right(45,0.1) 
                elif ((ultrasnc_data[0]<15) and (ultrasnc_data[1]<15)):
                    print("reverse")    
                    mot.motor_reverse(50,0.1)
                            
            else:
                mot.motor_stop(0)
                
            if((h*w)>detectionarea):
                mot.motor_stop(0)
                #led.rgbled([3])
                break
  
    cd.camera_close()
    print("\n Done. Mission Accomplished!\n")
    mot.motor_stop(10)

def autonomousA_stage2(detectionarea=17000):           #autonomous locate Color Sequencing
    led.rgbled([1])
    window = 20
    detectionwidth=20
    detectionarea = detectionarea
    mot.rf_disable()            #disable Radio before run control from RPI.
    cs.camera_setup()
    led.led([45])
    time.sleep(0.2)
    led.led([1])
    time.sleep(0.2)
    led.led([200])
    print("Hello Autonomous!")
    while 1:
        #x,y,w,h = cd.camera_search(cd.graphic_off)
        success,x,y,w,h,area = cs.colorsequence(cs.graphic_on)
        k=cs.cv2.waitKey(1) 
        if k == ord('q'):
            break
        print("success=",success,"x=",x,"y",y,"area=",area,"w=",w,"h=",h)
            
        if(success):
            if(x>cs.screen_width/2+window) :
                mot.motor_cw(20,0)
            elif ((x<cs.screen_width/2-window) & (x!=0)):
                mot.motor_ccw(20,0)
            elif((x>cs.screen_width/2-window) & (x<cs.screen_width/2+window)):
                ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
                if((ultrasnc_data[1]>=15) and (ultrasnc_data[0]>=15)):
                    if(ultrasnc_data[2]<15):
                        print("forward rightside")
                        mot.motor_ccw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    elif(ultrasnc_data[3]<15):
                        print("forward leftside")
                        mot.motor_cw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    else: 
                        print("Forward straight")
                        mot.motor_forward(40,0.3)

                elif ((ultrasnc_data[1]<15) and (ultrasnc_data[2]>15) and ultrasnc_data[0]>15 ):
                    print("left")
                    mot.motor_left(45,0.1) 
                elif((ultrasnc_data[0]<15) and (ultrasnc_data[3]>15) and ultrasnc_data[1]>15):
                    print("right")
                    mot.motor_right(45,0.1) 
                elif ((ultrasnc_data[0]<15) and (ultrasnc_data[1]<15)):
                    print("reverse")    
                    mot.motor_reverse(50,0.1)
                            
            else:
                mot.motor_stop(0)
                
            if(area>detectionarea):
                mot.motor_stop(100)
                #led.rgbled([3])
                break
  
    cs.camera_close()
    print("\n Done. Mission Accomplished!\n")
   # wallsquaring(15)            # if during final detection from camera it stop at an angle it will align perpendicular to the wall using 2 front ultrasonic.
    time.sleep(0.5)
    mot.motor_left(45,1) 
    mot.motor_right(45,2) 
    mot.motor_stop(50) 
    time.sleep(0.5)
    mot.motor_stop(10)
    

def wallsquaring(distance = 15):
    ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
    print(ultrasnc_data)
    for n in range(15):
        ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
        print(ultrasnc_data)
        if(ultrasnc_data[0]!=ultrasnc_data[1] and ultrasnc_data[0]<=40 and ultrasnc_data[1]<=40):
            for i in range(500):    # align left side 
                ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
                print("Ultrasonic Left:",ultrasnc_data,num)
                time.sleep(0.1)
                if(ultrasnc_data[0]>distance):
                    mot.motor_fwdspinleft(35,0.01)
                elif(ultrasnc_data[0]<(distance-1)):
                    mot.motor_rwdspinleft(35,0.01)
                else:
                    mot.motor_stop(0)
                    break
            for i in range(500):    # align right side
                ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
                print("Ultrasonic Right:",ultrasnc_data,num)
                time.sleep(0.1)
                if(ultrasnc_data[1]>distance):
                    mot.motor_fwdspinright(35,0.01)
                elif(ultrasnc_data[1]<(distance-1)):
                    mot.motor_rwdspinright(35,0.01)
                else:
                    mot.motor_stop(0)
                    break
        elif ((ultrasnc_data[0]>40) or (ultrasnc_data[1]>40)):
            print("Ilegal wall squaring. Too far from any wall.")
            break



def autonomousB_stage1_loop():            #autonomous locate QR Code
    nv.camera_setup()
    while 1:
        nv.readqr(1)
        k=nv.cv2.waitKey(1) 
        if k == ord('x'):
            nv.cv2.destroyWindow("QRCode")
            break
        elif k != -1 and k!=ord('s'):
            print(chr(k))
            nv.charsearch(k)
            #xyz=np.where(barray==ord('4'))
            #print(xyz)
            #print(barray) 
        elif k == ord('s'):
            print("Your character 's'")
            #nv.charsearchdata()
            nv.charsearch(ord('4'))
    nv.camera_close()

def autonomousB_stage1(detectionarea=22500):
    nv.camera_setup()
    led.rgbled([1])
    window = 20
    detectionwidth=20
    detectionarea = detectionarea
    mot.rf_disable()            #disable Radio before run control from RPI.
    led.led([45])
    time.sleep(0.2)
    led.led([1])
    time.sleep(0.2)
    led.led([200])
    print("Autonomous QRCode Detect!")
    while 1:
        x,y,text,h,w = nv.readqr(1)
        area= h*w
        k=nv.cv2.waitKey(1) 
        if k == ord('q'):
            break
        print("x=",x,"y",y,"Text",text,"h",h,"w",w,"area",area)
            
        if(area>100):
            if(x>nv.screen_width/2+window) :
                mot.motor_cw(20,0)
            elif ((x<nv.screen_width/2-window) & (x!=0)):
                mot.motor_ccw(20,0)
            elif((x>nv.screen_width/2-window) & (x<nv.screen_width/2+window)):
                ultrasnc_data,num = us.ultrasonic_read(us.read_on,4)
                if((ultrasnc_data[1]>=15) and (ultrasnc_data[0]>=15)):
                    if(ultrasnc_data[2]<15):
                        print("forward rightside")
                        mot.motor_ccw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    elif(ultrasnc_data[3]<15):
                        print("forward leftside")
                        mot.motor_cw(45,0.1)    
                        mot.motor_forward(40,0.1)
                    else: 
                        print("Forward straight")
                        mot.motor_forward(40,0.3)

                elif ((ultrasnc_data[1]<15) and (ultrasnc_data[2]>15) and ultrasnc_data[0]>15 ):
                    print("left")
                    mot.motor_left(45,0.1) 
                elif((ultrasnc_data[0]<15) and (ultrasnc_data[3]>15) and ultrasnc_data[1]>15):
                    print("right")
                    mot.motor_right(45,0.1) 
                elif ((ultrasnc_data[0]<15) and (ultrasnc_data[1]<15)):
                    print("reverse")    
                    mot.motor_reverse(50,0.1)
                            
            else:
                mot.motor_stop(0)
                
            if(area > detectionarea):
                mot.motor_stop(100)
                break
    nv.camera_close()
    print("\n Done. Mission Accomplished!\n")

if(__name__=='__main__'):
    autonomousB_stage1()
    nv.charsearch(ord('4'))
    while True:
        True

if(__name__=='__main__'):

    md.loopautodetect()          #motion detection
    bz.sounder(1)
    print("Movement detected. Start Cleaning!")
    autonomous1_stage1()         #color detection
    bz.sounder(1)
    autonomous1_stage2(10000)    #color sequence detection
    bz.sounder(1)
    autonomous2_stage1()        #qrcode detection and readout
    bz.sounder(1)
    mot.motor_forward(30,2)
    mot.motor_stop(100)
    wallsquaring(20)             #wall squaring
    nv.charsearch(ord('7'))
    arm.linear_arm_servo(50)      #move arm
    print("Activate Arm")
    arm.linear_servo(90)
    
