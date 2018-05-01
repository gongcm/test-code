

import cv2 as cv
import numpy as np



# cv.namedWindow('dst')

char = list("abcdijpkkppjfpafpkfdpdjpajodjfoejfpeajfpefjdjfpa.$#%^&*!@#~,")

print(char)
cv.waitKey(0)
def getchar(x):
    if(x == 0):
        return ''
    len_c = len(char)
    # l = np.uint8(len_c)
    # d = np.divide(x,l)
    uint = (256.0 + 1)/len_c
    return char[int(x / uint)]

img = cv.imread("D:\Airplane.jpg",cv.IMREAD_COLOR)

cv.resize()
dst = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
print(img,'\nimg.shape : ',img.shape)

cv.namedWindow("src")
cv.imshow("src",img)

# print(dst)
# print('len : ',len(char))

txt =''
for i in range(img.shape[0]):
    for j in range(img.shape[1]):
        pixel = dst[i][j]
        # printprintprint(dst[i][j],'type [][][]:',type(dst[0][0]))
        txt += getchar(pixel)
        # print("asscii :",t)
    txt +='\n'
print('len : ',len(char),'type : ',type(char))
print('char len : ',char.__len__())

with open("t.txt","w+") as f:
    f.write(txt)

