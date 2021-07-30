#将文件夹下的图片全部缩放，裁减，并按新文件名存储
import os
import cv2

path = 'D:\code\py-readpic'        # 原文件夹路径
newpath = 'D:\code\py-readpic1'  # 新文件夹路径
files = os.listdir(path)           # 获取文件名列表
for i, file in enumerate(files):   # 展开文件名的列表和索引
    if file.endswith('.png'):
        imgName = os.path.join(path, file)      # 获取文件完整路径
        img = cv2.imread(imgName)                 # 读图
        imgNew = cv2.resize(img, (1200, 1200))  # 缩放
        # imgNew = imgNew[60:552,:]             # 截取一部分区域
        newName = os.path.join(newpath, 'img_%03d'%(0+i)+'.jpg')  # 设置新的文件名
        print(newName)
        cv2.imwrite(newName,imgNew)             # 存储按新文件名命令的图片
