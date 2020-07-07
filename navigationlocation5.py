import cv2
import numpy as np
import matplotlib.pyplot as plt
import math
from imutils.video import FPS
import pytesseract
from pyzbar import pyzbar

#pytesseract = pytesseract.tesseract_cmd = r'C:\\Program Files (x86)\\Tesseract-OCR\\tesseract.exe'

graphic_enable=0
graphic_off = 0
graphic_on = 1
screen_width = 720 #720 #1280 #720 #640 #500
screen_hight =  405 #405 #720 #405 #360 #281
textsetting ="-c tessedit","_char_whitelist=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"," --psm 10"," -l osd"," " # detect single character
def camera_setup():
    print("Start Camera QR Code & Text Recognition!")
    global cam
    cam = cv2.VideoCapture(0)  

def camerasearch(graphic_enable = graphic_off):
        global lowerBounddd
        #global upperBound
        #global img,ret,gray
        #global contours,edged,auto,blurred
        #ret, img = cam.read()
        #for i in range(1):
        #    ret, img = cam.read()
        #    img = cv2.resize(img, (screen_width, screen_hight))
        #    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 
        #    gray = cv2.GaussianBlur(gray, (3, 3), 0)     
         #   #gray = cv2.bitwise_not(gray)
         #  #ret,gray = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)
            #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,11,2)
            #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,17,4)
            
            #edged = cv2.Canny(gray, 130, 200) 
            #contours, hierarchy = cv2.findContours(edged, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            #cv2.drawContours(img, contours, -1, (0, 0, 255), 2) 
            
def camera_close():
    cam.release()
    cv2.destroyAllWindows()     

def readqr(graphic_enable = 0):
        global lowerBound
        global upperBound
        global img,ret,gray
        global contours,edged,auto,blurred
        ret, img = cam.read()
        for i in range(1):
            ret, img = cam.read()
            img = cv2.resize(img, (screen_width, screen_hight))
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 
            gray = cv2.GaussianBlur(gray, (3, 3), 0)     
            #gray = cv2.bitwise_not(gray)
            #ret,gray = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)
            #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,11,2)
            #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,17,4)
            
            #edged = cv2.Canny(gray, 130, 200) 
            #contours, hierarchy = cv2.findContours(edged, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            #cv2.drawContours(img, contours, -1, (0, 0, 255), 2) 
        global qx,qy,qw,qh,barcodeData
        barcodes = pyzbar.decode(gray)
        if barcodes is None:
            x=0
            y=0
            h=0
            w=0
            area=0
            barcodeData='-'
        for barcode in barcodes:
            (x, y, w, h) = barcode.rect
            qx=x+round(w/2)
            qy=y+round(h/2)
            cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
            cv2.rectangle(img, (qx-5, qy-5), (qx+5, qy+5), (0, 255, 0), 2)
            barcodeData = barcode.data.decode("utf-8")
            barcodeType = barcode.type
            text = "{} ({})".format(barcodeData, barcodeType)
            cv2.putText(img, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX,0.5, (0, 0, 255), 2)
            #print("Found {} barcode: {}".format(barcodeType, barcodeData))
            print(barcodeData)
            #cv2.imshow("QRcode", img)
            showInMovedWindow('QRCode',img,10,50,graphic_enable)
            print(qx,qy)
            return(qx,qy,barcodeData,h,w)


def charsearch(what=49, graphic_enable=graphic_on):
    global ch,cw,cy,cx,b,img,count,barray
    #text = pytesseract.image_to_string(gray,config='--psm 13')
    cx=0
    cy=0
    ch=0
    cw=0
    barray=np.array([])
    count=0
    boxes= pytesseract.image_to_boxes(gray,config='--psm 13')
    h, w, c  = img.shape
    barray=boxes
    for b in boxes.splitlines():
        b = b.split(' ')
        if (b[0]==chr(what)):
            img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
            cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
            cx=int(b[1])
            cy=h-int(b[2])
            ch=int(b[4])-int(b[2])
            cw=int(b[3])-int(b[1])
            count=count+1          
            print("cx,cy,ch,cw:",cx,cy,ch,cw)
    
    showInMovedWindow('Char',img,710,50,graphic_enable)      
    print(count)
        

def charsearchdata(what=49, graphic_enable=graphic_on):
    data= pytesseract.image_to_data(gray,config='--psm 13')
    print(data)   

def showInMovedWindow(winname, img, x, y,graphic_enable= graphic_off):
    if(graphic_enable==graphic_on):
        cv2.namedWindow(winname)        # Create a named window
        cv2.moveWindow(winname, x, y)   # Move it to (x,y)
        cv2.imshow(winname,img)                       



if (__name__=="__main__"):
    camera_setup()
    while 1:
        #camerasearch(graphic_on)
        readqr(graphic_on)
        #showInMovedWindow('QRCode',img,10,50,graphic_on)
        k=cv2.waitKey(1) 
        if k == ord('x'):
            cv2.destroyWindow("QRCode")
            break
        elif k != -1 and k!=ord('s'):
            print(chr(k))
            charsearch(k)
            #xyz=np.where(barray==ord('4'))
            #print(xyz)
            #print(barray) 
            #showInMovedWindow('Char',img,710,50,graphic_on)
        elif k == ord('s'):
            print("Your character 's'")
            charsearchdata(1)
            #charsearch(ord('4'))
            #showInMovedWindow('Char',img,710,50,graphic_on)

    camera_close()
    


if(__name__=="__main__"):
    camera_setup()
    while(1):
        camerasearch(graphic_on)
        cv2.imshow("Contour",img)
        cv2.imshow("Gray",gray)
        k=cv2.waitKey(1) 
        if k == ord('x'):
            break
        elif k== ord('2'):
            #text = pytesseract.image_to_string(gray,config='--psm 13 ')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 13 ')
            h, w, c  = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='2'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)
        elif k== ord('1'):
            #text = pytesseract.image_to_string(gray,config='--psm 13')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 13')
            h, w, c = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='1'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)
        elif k== ord('3'):
            #text = pytesseract.image_to_string(gray,config='--psm 13')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 13')
            h, w, c  = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='3'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)
        elif k== ord('l'):
            #text = pytesseract.image_to_string(gray,config='--psm 13')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 13')
            h, w, c  = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='L'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)                    
        elif k== ord('6'):
            #text = pytesseract.image_to_string(gray,config='--psm 12')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 12')
            h, w, c  = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='6'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)                    
        elif k== ord('g'):
            #text = pytesseract.image_to_string(gray,config='--psm 13')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 13')
            h, w, c  = img.shape
            for b in boxes.splitlines():
                b = b.split(' ')
                if (b[0]=='G'):
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 2)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)                    
        elif k== ord('a'):
            #text = pytesseract.image_to_string(gray,config='--psm 11 --oem 3 -c tessedit_char_whitelist=0123456789')
            #boxes= pytesseract.image_to_boxes(gray,config='--psm 11 --oem 3 -c tessedit_char_whitelist=0123456789')
            #text = pytesseract.image_to_string(gray,config='--psm 12 --oem 2')
            boxes= pytesseract.image_to_boxes(gray,config='--psm 12 --oem 2')
            h, w, c = img.shape
            for b in boxes.splitlines():
                if (b[0]!='~' ):  #and b[0].isdigit()
                    b = b.split(' ')
                    img = cv2.rectangle(img, (int(b[1]), h - int(b[2])), (int(b[3]), h - int(b[4])), (255, 0, 0), 1)
                    cv2.putText(img, str(b[0]), (int(b[1]),h - int(b[2])+10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255), 1)
                    cv2.imshow("Boxed",img)
                    print(b)
        elif k==ord('q'):
            barcodes = pyzbar.decode(gray)
            #print(barcodes)
            for barcode in barcodes:
                (x, y, w, h) = barcode.rect
                qx=x+round(w/2)
                qy=y+round(h/2)
                cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
                cv2.rectangle(img, (qx-5, qy-5), (qx+5, qy+5), (0, 255, 0), 2)
                barcodeData = barcode.data.decode("utf-8")
                barcodeType = barcode.type
                text = "{} ({})".format(barcodeData, barcodeType)
                cv2.putText(img, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX,0.5, (0, 0, 255), 2)
                print("Found {} barcode: {}".format(barcodeType, barcodeData))
                #
                # print(barcodeData)
                cv2.imshow("QRcode", img)

    camera_close()
