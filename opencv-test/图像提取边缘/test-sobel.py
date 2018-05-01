import cv2 as cv
# import numpy as np
import math
import matplotlib.pyplot as plt
# print("opencv version : ",cv.version)

'''
sobel 算子 离散微分算子 用来计算图像灰度的近似梯度，梯度越大越有可能是边缘
'''
print('opencv version :',cv.__version__)
img = cv.imread("D:\Lena.jpg")
cv.imshow("src",img)

dst = cv.GaussianBlur(img,(3,3),0,borderType=cv.BORDER_DEFAULT)

dst = cv.cvtColor(dst,cv.COLOR_BGR2GRAY)

xgrad = cv.Sobel(dst,cv.CV_16S,1,0,ksize=3)
ygrad = cv.Sobel(dst,cv.CV_16S,0,1,ksize=3)

# cv.imshow("xgrad-1",xgrad)
# cv.imshow("ygrad-1",ygrad)

xgrad = cv.Sobel(dst,cv.CV_16S,1,0,ksize=-1)
ygrad = cv.Sobel(dst,cv.CV_16S,0,1,ksize=-1)


dst = cv.addWeighted(xgrad,0.5,ygrad,0.5,0)

dst1 = dst.copy()
for i in range(dst.shape[1]):
    for j in range(dst.shape[0]):
        # x = xgrad[i][j] + ygrad[i][j]
        if(xgrad[i][j] > 0 ):
            if( ygrad[i][j] > 0):
                x = xgrad[i][j] * xgrad[i][j] + ygrad[i][j] * ygrad[i][j]
                x = math.sqrt(x)
                print("x : ",x)
                dst1[i][j] = int(x)
        else:
            dst1[i][j] = 0
plt.subplot(2,2,1).imshow(xgrad)
plt.subplot(2,2,2).imshow(ygrad)
plt.subplot(2,2,3).imshow(dst)
plt.subplot(2,2,4).imshow(dst1)
plt.show()


# cv.imshow("xgrad",xgrad)
# cv.imshow("ygrad",ygrad)
# cv.imshow("dst",dst)
# cv.imshow("dst1",dst1)

cv.waitKey(0)


