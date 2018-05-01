import numpy as np
import matplotlib.pyplot as plt

'''
    单层感知器：
    二维平面坐标系，点[3，3],[4，3] bai'n
'''

#输入数据

X = np.array([[1,3,3],
              [1,4,3],
              [1,1,1]])
print(X)

#标签

Y = np.array([1,1,-1])

#权值初始化
W = (np.random.random(3) - 0.5) * 2
print('权值初始化',W)
print('\n')
#学习率设置
lr = 0.11

#计算迭代次数
n = 0

#神经网络的输出

O = 0

def update():
    global X,Y,W,lr,n

    n += 1
    O = np.sign(np.dot(X,W.T))
    W_C = lr *((Y - O.T).dot(X)/int(X.shape[0]))
    W = W + W_C

for _ in range(100):

    update()
    print('当前的权值：',W)
    print('迭代次数：',n)
    O = np.sign(np.dot(X, W.T))

    if(O == Y.T).all():
        print('Finished')
        print('epoch',n)
        break

# 正样本
x1 = [3,4]
y1 = [3,3]

#负样本

x2 = [1]
y2 = [1]

#计算分界线的斜率和截距

k = - W[1]/W[2]
d = - W[0]/W[2]

print('k = ',k)
print('d = ',d)

x = np.linspace(0,5)

plt.figure()
plt.plot(x,x * k + d,'r')
plt.plot(x1,y1,'yo')
plt.plot(x2,y2,'bo')

plt.show()
# plt.savefig()