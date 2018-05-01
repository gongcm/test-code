import cv2 as cv

'''
利用opencv 训练好的xml 标识人脸
'''


windowname = '人脸识别'

img = cv.imread("D:\Lena.jpg")

cv.namedWindow(windowname)

cascade = cv.CascadeClassifier("haarcascade_eye.xml") # 训练好的模型

rects = cascade.detectMultiScale(img)

print(rects)
while True:

    for x,y,width,height in rects:
        cv.rectangle(img,(x,y),(x+width,y+height),(0,0,255),1)

        cv.imshow(windowname,img)

    if(cv.waitKey(0) == 27):
        break


cv.destroyAllWindows()
# exit()