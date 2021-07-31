#将文件夹下的图片全部缩放并存储到新文件夹
import os
import cv2

path = 'D:\code\py-readpic'       
newpath = 'D:\code\py-readpic1'  
files = os.listdir(path)           
for i, file in enumerate(files):   
    if file.endswith('.png'):
        imgName = os.path.join(path, file)      
        img = cv2.imread(imgName)                 
        imgNew = cv2.resize(img, (1200, 1200))  
        # imgNew = imgNew[60:552,:]             
        newName = os.path.join(newpath, 'img_%03d'%(0+i)+'.jpg')  
        print(newName)
        cv2.imwrite(newName,imgNew)            
