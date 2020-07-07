import cv2
import numpy as np
import matplotlib.pyplot as plt
import math
from imutils.video import FPS

graphic_off = 0
graphic_on = 1
screen_width = 500 #720 #640 #500
screen_hight = 281 #405 #360 #281
whatcolor = 0
pink = 1
yellow =2
green =3 
blue = 4
orange = 5
graphic_enable = 0
lowerBound = np.array([12, 192, 174])
upperBound = np.array([25, 255, 255])
area = 0

#pink 
#lower HSV : 112, 164, 19 
#Upper HSV : 176, 255, 255

#yellow
#lower HSV : 26, 192, 11 
#Upper HSV : 41, 255, 255

#green
#lower HSV : 41, 160, 51 
#Upper HSV : 61, 255, 255


def nothing(x):
    #
    pass

def camera_setup():
    print("Start Camera!")
    global cam
    cam = cv2.VideoCapture(0)  

def colorchoice(whatcolor = orange, graphic_enable = graphic_off):
    global lowerBound
    global upperBound
    for i in range(1):
        if whatcolor == pink:          # detect pink color
            lowerBound = np.array([112, 164, 19])
            upperBound = np.array([176, 255, 255])
            #print("Pink")
        elif whatcolor == yellow:       # detect yellow color
            lowerBound = np.array([24, 192, 44])        
            upperBound = np.array([35, 255, 255])
            #print("Yellow")
        elif whatcolor==green:       # detect green color
            lowerBound = np.array([36, 135, 128])
            upperBound = np.array([61, 255, 255])
            #print("Green")
        elif whatcolor==blue:       # detect blue color
            #lowerBound = np.array([62, 176, 125])       #daytime
            lowerBound = np.array([46, 209, 114])       #nighttime
            upperBound = np.array([109, 255, 255])
            #print("Blue")
        elif whatcolor==orange:       # detect orange color
            #lowerBound = np.array([12, 192, 174])       # daytime
            lowerBound = np.array([10, 191, 153])        # nighttime
            upperBound = np.array([24, 255, 255])
            #print("Orange")

        if(graphic_enable==graphic_on):
            cv2.namedWindow("Trackbars")                # Create Trackbars
            cv2.moveWindow("Trackbars", 10,20)          # Move it to (x,y)
            cv2.createTrackbar("Lower-H","Trackbars",lowerBound[0],180,nothing)
            cv2.createTrackbar("Lower-S","Trackbars",lowerBound[1],255,nothing)
            cv2.createTrackbar("Lower-V","Trackbars",lowerBound[2],255,nothing)
            cv2.createTrackbar("Upper-H","Trackbars",upperBound[0],180,nothing)
            cv2.createTrackbar("Upper-S","Trackbars",upperBound[1],255,nothing)
            cv2.createTrackbar("Upper-V","Trackbars",upperBound[2],255,nothing) 
    

def colorsearch(whatcolor = orange,graphic_enable = graphic_off):
        global lowerBound
        global upperBound
        global img,x,y,w,h,peri,shape
        ret, img = cam.read()
        img = cv2.resize(img, (screen_width, screen_hight))
        #cv2.imshow("Original",img)
        shape = "unidentified"

        if(graphic_enable==graphic_on):
            lower_H = cv2.getTrackbarPos("Lower-H","Trackbars")
            lower_V = cv2.getTrackbarPos("Lower-S","Trackbars")
            lower_S = cv2.getTrackbarPos("Lower-V","Trackbars")
            upper_H = cv2.getTrackbarPos("Upper-H","Trackbars")
            upper_S = cv2.getTrackbarPos("Upper-V","Trackbars")
            upper_V = cv2.getTrackbarPos("Upper-V","Trackbars")
            lowerBound = np.array([lower_H, lower_S, lower_V])
            upperBound = np.array([upper_H, upper_S, upper_V])

        kernelOpen = np.ones((5, 5))
        kernelClose = np.ones((20, 20))
        font = cv2.FONT_HERSHEY_SIMPLEX
        bottomLeftCornerOfText = (10,500)
        fontScale = 0.6
        fontColor = (0,0,255)
        lineType=2
   
   
        for i in range(1):
            ret, img = cam.read()
            img = cv2.resize(img, (screen_width, screen_hight))
              
            imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
            mask = cv2.inRange(imgHSV, lowerBound, upperBound)
            maskOpen = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernelOpen)
            maskClose = cv2.morphologyEx(maskOpen, cv2.MORPH_CLOSE, kernelClose)

            maskFinal = maskClose
            conts, h = cv2.findContours(maskFinal.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(img, conts, -1, (255, 0, 0), 3)
            for i in range(len(conts)):
                x, y, w, h = cv2.boundingRect(conts[i])
                if(w>15 ) :
                    for cnt in conts:
                        area=cv2.contourArea(conts[0])  
                        peri = cv2.arcLength(cnt, True)
                        approx = cv2.approxPolyDP(cnt, 0.04 * peri, True)
                        if(len(approx)==4):
                            shape='square'
                        elif (len(approx)==3):
                            shape='triangle'
                        elif(len(approx)==5):
                            shape='pentagon'
                        elif(len(approx)==6):
                            shape='hexagon'
                        elif(len(approx)>=7):
                            shape='polygon'
                else:
                    x=0
                    y=0
                    h=0
                    w=0
                    shape=0
                    peri=0
                    area=0
                    
            if(len(conts)==0):
                x=0
                y=0
                h=0
                w=0
                shape=0
                peri=0
                area=0
            return(x,y,h,w,shape,peri,whatcolor,area)   
                        
def showInMovedWindow(winname, img, x, y,graphic_enable= graphic_off):
    if(graphic_enable==graphic_on):
        cv2.namedWindow(winname)        # Create a named window
        cv2.moveWindow(winname, x, y)   # Move it to (x,y)
        cv2.imshow(winname,img)                       


def colorsequence(graphic_enable=graphic_off):
    font = cv2.FONT_HERSHEY_SIMPLEX
    bottomLeftCornerOfText = (10,500)
    fontScale = 0.6
    fontColor = (0,0,255)
    lineType=2
    arrayx1=np.array([])
    arrayy1=np.array([])
    arrayh1=np.array([])
    arrayw1=np.array([])
    arrayp1=np.array([])
    arrayx2=np.array([])
    arrayy2=np.array([])
    arrayh2=np.array([])
    arrayw2=np.array([])
    arrayp2=np.array([])
    success=0
        
    colorchoice(orange,graphic_enable)
    x1,y1,h1,w1,shape1,peri1,color1,area1=colorsearch(orange,graphic_enable)
    if(x1!=0 and y1!=0 and h1>=5 and w1>=5 and shape1=='square' and area1>100):
        
        colorchoice(blue,graphic_enable)
        x2,y2,h2,w2,shape2,peri2,color2,area2=colorsearch(blue,graphic_enable)
        if(x2!=0 and y2!=0 and h2>=5 and w2>=5 and shape1=='square'and area1>100):
            if(x1!=0) and (x2!=0):
                cv2.putText(img, 'Detect1', (x1, y1 - 10), font, fontScale, (155,255,0), 2)
                cv2.rectangle(img, (x1, y1), (x1 + w1, y1 + h1), (0, 0, 255), 2)
                #cv2.putText(img,"["+ str(round(x1,0)) +" ,  "+ str(round(y1,0)) +"]", (x1+w1+10, y1+10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,255), 1)
                #cv2.putText(img,"["+ str(round(h1,0)) +" ,  "+ str(round(w1,0)) +"]", (x1+w1+10, y1+30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,125,255), 1)

                cv2.putText(img, 'Detect2', (x2, y2 - 10), font, fontScale, (155,255,), 2)
                cv2.rectangle(img, (x2, y2), (x2 + w2, y2 + h2), (0, 0, 255), 2)
                #cv2.putText(img,"["+ str(round(x2,0))+ " ,  "+ str(round(y2,0)) +"]", (x2+w2+10, y2+10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,255), 1)
                #cv2.putText(img,"["+ str(round(h2,0))+ " ,  "+ str(round(w2,0)) +"]", (x2+w2+10, y2+30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,125,255), 1)
                #cv2.putText(img,"y2-y1["+ str(y2-y1) +"]"                 , (x2+w2+10, y2+h2+50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,125,255), 1)
                    
                if(math.isclose(y1+30,y2, abs_tol=60)):     # simple match as long as y2 is > y1 by +-30 pixels.                        
                    cv2.putText(img, 'Match within 60pixels', (x2, y2 + h2 + 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,0,255), 2)
                    success = 1
                else :
                    success = 0
                    x1=0
                    y1=0
                    h1=0
                    w1=0
                    area1=0
    cv2.line(img, (int(screen_width/2),10), (int(screen_width/2),screen_hight-10),(115,115,115), 1)
    cv2.line(img, (10,int(screen_hight/2)), (screen_width-10,int(screen_hight/2)),(115,115,115), 1)
    cv2.rectangle(img, (int(screen_width*0.4),int(screen_hight*0.4)), (int(screen_width*0.6),int(screen_hight*0.6)),(155,155,155), 1)
    showInMovedWindow("Detect",img,320,20,graphic_enable)
    x1=x1+w1/2
    y1=y1+h1/2
    return(success,x1,y1,h1,w1,area1) 

def camera_close():
    cam.release()
    cv2.destroyAllWindows()        


if(__name__=="__main__"):
    camera_setup()
    colorchoice(orange,graphic_on)
    fps = FPS().start()
    while(1):
        colorsearch(orange,graphic_on)
        showInMovedWindow("Orange",img,320,20,graphic_on)
        fps.update()
        k=cv2.waitKey(1) 
        if k == ord('x'):
            cv2.destroyWindow("Orange")
            break
    fps.stop()
    print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
    print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))        

    colorchoice(blue,graphic_on)
    fps = FPS().start()
    while(1):
        colorsearch(blue,graphic_on)
        showInMovedWindow("Blue",img,320,20,graphic_on)
        fps.update()
        k=cv2.waitKey(1) 
        if k == ord('x'):
            cv2.destroyWindow("Blue")
            break
    fps.stop()
    print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
    print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))        
    
    fps = FPS().start()
    for i in range(1000):
        colorsequence(graphic_on)
        fps.update()
        k=cv2.waitKey(1) 
        if k == ord('q'):
            break
        
    camera_close()
    fps.stop()
    print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
    print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))        
