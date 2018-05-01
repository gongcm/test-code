import cv2 as cv
'''
g(x) = (1 - a) * f1(x) + a * f2(x) 
'''



img1 = cv.imread("../test.png")
img2 = cv.imread("D:\Lena.jpg")

print('size :',img1.size)
print('shape',img1.shape,"type(shape)",type(img1.shape))
# img1 转化成和 img2 一样大
img1 = cv.resize(img1,(img2.shape[0],img2.shape[1]),interpolation=cv.INTER_LINEAR)
print('shape',img1.shape,"type(shape)",type(img1.shape))
cv.imshow("img1",img1)

dst = cv.add(img1,img2)
cv.imshow("add img",dst)

dst = cv.multiply(img1,img2)
cv.imshow("multiply img",dst)

cv.namedWindow("weighted")
def callback(x):
    print("value x :", x)
    dst = cv.addWeighted(img1,x/10,img2,(1 - x/10),0) # 混合图像大小必须一致
    cv.imshow("weighted",dst)

cv.createTrackbar(" weighted :","weighted",0,10,callback)
cv.imshow("weighted",dst)

cv.waitKey(0)