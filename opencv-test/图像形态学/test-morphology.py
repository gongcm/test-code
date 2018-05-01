import cv2 as cv


# img = cv.imread("../test.png")
img1 = cv.imread("D:\Lena.jpg")

img = cv.cvtColor(img1,cv.COLOR_BGR2GRAY)
cv.namedWindow("src")
cv.imshow("src",img)
cv.resizeWindow("src",img.shape[0]*2,img.shape[1])
# img2 = img.copy()
# cv.imshow("src1",img)

'''
膨胀：黑白图片 黑色多 黑色膨胀， 黑色变多，白色变少
腐蚀：黑白图片 白色少 白色腐蚀黑色 
'''
def callback(x):
    dst = cv.dilate(img, (x+1, x+1),) # 最大（偏白） 开始膨胀
    # cv.getStructuringElement()
    cv.imshow("src",dst)

def callback1(x):
    dst = cv.erode(img, (x+1, x+1),) # 最小像素 开始 腐蚀
    # cv.getStructuringElement()
    cv.imshow("src",dst)
cv.createTrackbar("kernel size","src",1,10,callback)
cv.createTrackbar("erode kernel size","src",1,10,callback1)

kernel = cv.getStructuringElement(cv.MORPH_RECT,(5,5))
print("kernel :",kernel)
dst = cv.morphologyEx(img1,cv.MORPH_OPEN,kernel) # 开操作 先腐蚀 后膨胀
cv.imshow("open",dst)
cv.imwrite("open_lena.jpg",dst)
dst = cv.morphologyEx(img1,cv.MORPH_CLOSE,kernel) # 闭操作 先膨胀 后腐蚀
cv.imshow("close",dst)
cv.imwrite("close_lena.jpg",dst)
'''
形态学梯度：膨胀减去腐蚀
内部梯度： 原图减去 腐蚀
方向梯度 ： x 和 y 进行计算
'''
dst = cv.morphologyEx(img1,cv.MORPH_GRADIENT,kernel)# 膨胀减去 腐蚀
cv.imshow("gradient",dst)
cv.imwrite("gradient_lena.jpg",dst)
dst = cv.morphologyEx(img1,cv.MORPH_TOPHAT,kernel) # 原图像与开操作之间的差值图像
cv.imshow("top hat",dst)
cv.imwrite("TopHat_lena.jpg",dst)
dst = cv.morphologyEx(img1,cv.MORPH_BLACKHAT,kernel)# 闭操作 与 原图像的差值
cv.imshow("black hat",dst)
cv.imwrite("BlackHat_lena.jpg",dst)

cv.waitKey(0)