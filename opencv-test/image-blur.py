#conding utf-8


'''滤波'''


import cv2 as cv
import numpy as np
import scipy.signal as signal



def addnoise(img,num):

    '''
    添加椒盐噪声
    :param img: 图像
    :param num: 噪声的个数
    :return:
    '''

    w = img.shape[1]
    h = img.shape[0]

    row = range(w)
    for i in range(0,num) :

        x =int(np.random.random()*w)
        y = int(np.random.random()*h)
        print("x : %d" %x)
        print("y : %d" %y)

        if(img.ndim == 2):
            img[x,y] = 255

        # bgr = img[x,y]
        # print(type(bgr))
        # print(bgr)
        if(img.ndim == 3):
            img[x,y] =(255,255,255)
            img[x,y,:] = 255

def func(x,y,sigma = 1):
    return 10*(1/(2*np.pi*sigma))*np.exp(-((x-2)**2+(y-2)**2)/(2.0*sigma**2))

def addGaussianNiose(img,sigma=1):

    # print("w : %d h : %d" %(w,h))
    # print("range(w) : %d" %(range(w)))

    dst = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    a = np.fromfunction(func, (5, 5),sigma=1)
    img = signal.convolve2d(dst,a,mode="same")
    return img



def callback(x):
    print("x is %s x value: %d" %(type(x),x))
    dst = cv.medianBlur(img, 2 * x + 1)
    cv.imshow("medianBlur",dst)


if __name__ == "__main__":
    img = cv.imread("D:\\Lena.jpg", cv.IMREAD_COLOR)
    # print(type(img))
    # print(img.ndim)
    cv.imshow("src",img)


    # addnoise(img,1000*10)
    img = addGaussianNiose(img,2)
    img = (img / float(img.max())) * 255
    cv.imshow("noise", img)
    # point = (0,0)
    # dst = np.array(img)
    # cv.blur(img,(5,5),dst,point)
    #
    # cv.imshow("blur",dst)
    #
    # cv.GaussianBlur(img,(5,5),2,dst,0.8)
    # cv.imshow("GaussianBlur",dst)

    cv.namedWindow("medianBlur")
    cv.createTrackbar("ksize ","medianBlur",0,15,callback)
    cv.imshow("medianBlur", img)
    # while(1):
    #     position = cv.getTrackbarPos("ksize","medianBlur")
    #     print("position : %d" %(position))
    #     cv.waitKey(200)
    #     # if(position % 2):
    #     #     cv.medianBlur(img,position,dst)
    #     #     cv.imshow("medianBlur",dst)

    cv.waitKey(0)