import numpy as np
import cv2
import time
import imutils

graphice_enable = 0
screen_width = 500  # 720 #640 #500
screen_height = 281  # 405 #360 #281
img = 0
initframe =0
ret = 0
cap = 0
img2= 0
frame = 0
delta = 0
current_time = 0
delta_time = 0
init_time = 0
status = ""
x = 0
y = 0
h = 0
w = 0
area =0

def camerasetup():
    global initframe,ret, cap,init_time,img0
    print("Start Motion Detection")
    cap = cv2.VideoCapture(0)
    
    for i in range(10):
        ret, img0 = cap.read()
        if ret is None :
            print("No Camera")
            break
    time.sleep(0.1)
    img0 = cv2.resize(img0, (screen_width, screen_height))
    initframe = cv2.cvtColor(img0, cv2.COLOR_BGR2GRAY)
    init_time = time.time()


def motion(graphic_enable=0,refresh=30):
    global img,img2, frame, delta, initframe,status
    global current_time, delta_time, init_time
    global x,y,w,h, area

    font = cv2.FONT_HERSHEY_SIMPLEX
    bottomLeftCornerOfText = (10, 500)
    fontScale = 0.8
    fontColor = (255, 125, 125)
    lineType = 2

    for i in range(1):
        ret, img = cap.read()                               #video stream read
        if ret is None :
            print("No Camera")
            break

        img = cv2.resize(img, (screen_width, screen_height))
        img2=img
        frame = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)       # convert to grayscale for easier calculation
        delta = cv2.absdiff(initframe, frame)               #subtraction absolute first frame and current frame

        delta = cv2.dilate(delta, None, iterations=2)
        delta = cv2.threshold(delta, 50, 255, cv2.THRESH_BINARY)[1]     #making higher differention pixels for contour.

        contours, hierarchy = cv2.findContours(delta,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        cnts = cv2.findContours(delta.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        cnts = imutils.grab_contours(cnts)

     #   cv2.drawContours(img2, contours, -1, (0, 255, 255), 1)
     #       if (len(contours)==0):
      #      status = "X"
       #     x = 0
        #    y = 0
         #   h = 0
          #  w = 0
          #  area = 0

        for c in contours:
            if cv2.contourArea(c) > 200:
                #print(cv2.contourArea(c))
                area=cv2.contourArea(c)
                (x, y, w, h) = cv2.boundingRect(c)
                cv2.rectangle(img2, (x, y), (x + w, y + h), (0, 255, 0), 2)
                text = "Motion"
               # cv2.putText(img2, "Area Status: {}".format(text), (10, 50),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 255), 1)
                status = text
            else:
                status = "X"
                area =0
                x = 0
                y = 0
                h = 0
                w = 0
                continue

        current_time = time.time()
        delta_time = current_time - init_time
        if delta_time > refresh:                #refresh the initframe after certain duration
            ret, initframe = cap.read()
            initframe = cv2.resize(initframe, (screen_width, screen_height))
            initframe = cv2.cvtColor(initframe, cv2.COLOR_BGR2GRAY)
            init_time = time.time()
            #print("Clear Image")

        return (area,status, x, y, h, w)

def closecamera():
    cap.release()
    cv2.destroyAllWindows()

def loopautodetect():
    camerasetup()
    while True:
        detect=motion(1,2)[0]
        if(detect>0):
            print("Detect",detect)
            break

    closecamera()


if (__name__ == "__main__"):

    camerasetup()
    while True:
        detect=motion(1,2)[0]
        if(detect>0):
            print("detect",detect)
       # cv2.imshow("Init Frame", img0)
        #cv2.imshow('Current Real Frame', img)
       # cv2.imshow('Delta Frame', delta)
       # cv2.imshow('Delta Countour', img2)

        k=cv2.waitKey(1)
        if k == ord('x'):       # exit
            break
        elif k == ord('r'):     #manual refresh initframe
            ret, initframe = cap.read()
            initframe = cv2.resize(initframe, (screen_width, screen_height))
            initframe = cv2.cvtColor(initframe, cv2.COLOR_BGR2GRAY)
            print("Manual Refresh")
    closecamera()
