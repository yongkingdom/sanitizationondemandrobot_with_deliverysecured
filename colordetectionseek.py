import cv2
import numpy as np
graphic_off = 0
graphic_on = 1
screen_width = 500 #720 #640 #500
screen_hight = 281 #405 #360 #281
detectionsizewidth = 10
# increase GPU RPI from 256 to 512MB, everything running faster.
def nothing(x):
    #
    pass        

def camera_setup():
    print("Start Camera!")
    global cam
    cam = cv2.VideoCapture(0)
    
    

def camera_search(graphic = graphic_off):
    #print("Activate Search !")
    # detect lime green
    #lowerBound = np.array([35, 100, 100])
    #upperBound = np.array([64, 255, 255])

    # BGR to HSV [13, 255, 180]
    #color = np.uint8([[[0,80,180]]])
    #hsv_color = cv2.cvtColor(color,cv2.COLOR_BGR2HSV)
    #print(hsv_color)
    
    #orange
    #lowerBound = np.array([16, 115, 200])
    #upperBound = np.array([31, 255, 255])

    #green lime
    #lowerBound = np.array([35, 100, 100])
    #upperBound = np.array([64, 255, 255])
    if(graphic!=0):
        cv2.namedWindow("Trackbars")
        #Detect Orange color
        cv2.createTrackbar("Lower-H","Trackbars",10,180,nothing)
        cv2.createTrackbar("Lower-S","Trackbars",127,255,nothing)
        cv2.createTrackbar("Lower-V","Trackbars",153,255,nothing)

        cv2.createTrackbar("Upper-H","Trackbars",24,180,nothing)
        cv2.createTrackbar("Upper-S","Trackbars",255,255,nothing)
        cv2.createTrackbar("Upper-V","Trackbars",255,255,nothing)


    else:  #Orange
        lower_H = 16
        lower_V = 115
        lower_S = 200
        upper_H = 31
        upper_S = 255
        upper_V = 255

    if(graphic!=0):
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
    fontScale = 0.8
    fontColor = (0,0,255)
    lineType=2
    #font = cv2.cv.InitFont(cv2.cv.CV_FONT_HERSHEY_SIMPLEX, 2, 0.5, 0, 3, 1)
    for x in range(1):
        ret, img = cam.read()
        img = cv2.resize(img, (screen_width, screen_hight))
        
        # convert BGR to HSV
        imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        # create the Mask
        mask = cv2.inRange(imgHSV, lowerBound, upperBound)
        # morphology
        maskOpen = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernelOpen)
        maskClose = cv2.morphologyEx(maskOpen, cv2.MORPH_CLOSE, kernelClose)

        maskFinal = maskClose
        conts, h = cv2.findContours(maskFinal.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(img, conts, -1, (255, 0, 0), 3)
        for i in range(len(conts)):
            x, y, w, h = cv2.boundingRect(conts[i])
            if(w>detectionsizewidth) :
                if(graphic!=0):
                    cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
                    #print("Detect:",x,y,w,h)
                    cv2.putText(img, 'Detect', (x, y - 10), font, fontScale, fontColor, 2)
        
        if(graphic!=0):
            #cv2.imshow("maskClose", maskClose)
            cv2.imshow("maskOpen", maskOpen)
            #cv2.imshow("mask", mask)
            cv2.imshow("cam", img)
            

        if(len(conts)==0) :
            #print(0,0,0,0)
            x=0
            y=0
            w=0
            h=0
            return(x,y,w,h)
        else:
            if(w>detectionsizewidth):
                return(x,y,w,h)
            else:
                x=0
                y=0
                w=0
                h=0
                return(x,y,w,h)

def camera_close():
    cam.release()
    cv2.destroyAllWindows()

if(__name__=='__main__'):
    camera_setup()
    while 1:
        camera_search(1)
        if cv2.waitKey(33) & 0xFF == ord('q'):
            break
    cam.release()
    cv2.destroyAllWindows()
    #camera_close
