import cv2 as cv

img = cv.imread("D:\Lena.jpg")

# cv.imwrite("..\Lena.jpg",img)

img1 = img.copy()
img = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
cv.namedWindow("src")
cv.imshow("src",img)

def onChange(x):
    retval,dst = cv.threshold(img,x,255,cv.THRESH_OTSU)
    print("retval : ",retval)
    # print(dst)
    cv.imshow("src",dst)


cv.createTrackbar("threshold :","src",0,255,onChange)

cv.waitKey(0)