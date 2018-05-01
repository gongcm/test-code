import cv2 as cv
import numpy

src = cv.imread('D:\\Lena.jpg', cv.IMREAD_COLOR)

new_img = numpy.zeros(src.shape, numpy.uint8)

src_bak = src.copy()

cv.imshow("new img", new_img)
cv.imshow("bak", src_bak)

w = src.shape[1]
h = src.shape[0]

for i in range(0, w):
    for j in range(0,h):
        src_bak[i,j,0] = src_bak[i,j,0]*0.2
        src_bak[i,j,1] = src_bak[i,j,1]*0.2
        src_bak[i,j,2] = src_bak[i,j,2]*0.2

cv.imshow("bak_1", src_bak)

# grey = numpy.zeros(src_bak.shape,numpy.uint8)
grey = cv.cvtColor(src_bak,cv.COLOR_RGB2GRAY)
grey1 = cv.cvtColor(src,cv.COLOR_RGB2GRAY)
cv.imshow("grey",grey)
cv.imshow("grey1",grey1)

print(src.shape)
print(src.shape[0])
print(src.__doc__)
print(type(src))
print(src)

print("grey :")
print(grey)
print("shape :" )
print(grey.shape)
cv.imshow("src", src)

cv.imwrite("D:\\new_img.jpg",new_img,[cv.IMWRITE_JPEG_QUALITY,95]) # jpg 图片质量 数值越大质量越高
cv.imwrite("D:\\new_img.png",new_img,[cv.IMWRITE_PNG_COMPRESSION,9]) # 压缩 0 ~ 9 压缩级别越高图像越小
cv.imwrite("D:\\grey_Lena.jpg",grey1) # 默认jpg 质量 95

print("random : ")
print(numpy.random.random(3))
'''
INTER_AREA = 3
INTER_BITS = 5
INTER_BITS2 = 10
INTER_CUBIC = 2
INTER_LANCZOS4 = 4
INTER_LINEAR = 1
'''
resize_img = cv.resize(src,(1024,512),fx=0.5,fy=0.5,interpolation=cv.INTER_NEAREST) # 图片缩放
cv.imshow("resize",resize_img)
cv.waitKey(0)
