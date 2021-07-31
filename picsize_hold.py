#图像缩放到一张纸上不改变长宽比
# *_* coding : UTF-8 *_*
import os
import cv2
import numpy as np

path = r'D:\code\py-readpic'         
newpath = r'D:\code\py-readpic1'    
files = os.listdir(path)             
c_w ,c_h = 800,800           

for i, file in enumerate(files):
    img_zeros = np.zeros((c_w, c_h, 3), np.uint8) 
    if file.endswith('.png'):
        imgName = os.path.join(path, file)        
        img = cv2.imread(imgName)                 
        h, w , _ = img.shape                      
        
        if max(w,h) > c_w:
            ratio = c_w / max(w,h)
            imgcrop = cv2.resize(img, (round(w * ratio) , round(h * ratio)))
            
            img_zeros[0:round(h * ratio), 0:round(w * ratio)] = imgcrop
        else:
            img_zeros[0:h, 0:w] = img
        # imgNew = imgNew[60:552,:]     

        newName = os.path.join(newpath, 'img_%03d'%(0+i)+'.jpg')
        print(newName)
        cv2.imwrite(newName,img_zeros)           


