import numpy as np
import cv2

mapData = []
size = 11
with open('map.txt','r') as f:
    mapData = [line[:len(line)-1] for line in f.readlines()]
    # mapData = [ for line in mapData]
# print(mapData)

class SimulateMap:
    
    def __init__(self,nGrid,size):
        self.size = size
        self.nGrid = nGrid
        self.image = np.zeros((nGrid*size,nGrid*size),dtype = 'float32')
        self.arrowSize = int(self.size*0.7)
        self.arrow = np.zeros((self.arrowSize,self.arrowSize),dtype='float32')
        self.arrow[int(size*0.3):int(size*0.7),size//2 - int(size*0.05):size//2 + int(size*0.05)] = 1.0
        self.arrow[int(size*0.5):int(size*0.7),size//2 - int(size*0.2):size//2 + int(size*0.2)] = 1.0
        
    def drawGrid(self,x,y,_type):
        radius = int(self.size *1.0)
        thickness = 5
        _type = ord(_type) - ord('A')
        x *= self.size
        y *= self.size
        if _type & 0b1000:
            self.image[y + radius :y + radius + thickness,x:x + self.size] = 1.0
        if _type & 0b0100:
            self.image[ y  :y + self.size, x + self.size - radius :x + self.size - radius + thickness] = 1.0
        if _type & 0b0010:
            self.image[ y + self.size - radius :y + self.size - radius + thickness,x:x + self.size] = 1.0
        if _type & 0b0001:
            self.image[ y  :y + self.size,x + radius :x + radius + thickness] = 1.0
    
    def readMap(self,mapData):
        for y,row in enumerate(mapData):
            for x,block in enumerate(row):
                self.drawGrid(x,y,block)
    def showMap(self,t):
        cv2.imshow('f',self.image)
        cv2.waitKey(t)
    def showMapWithCar(self,t,x,y,direction):
        tempMap = self.image.copy()
        angle = 0
        if direction == 'N':
            angle = 0
        elif direction == 'W':
            angle = 90
        elif direction == 'S':
            angle = 180
        elif direction == 'E':
            angle = 270
            
        rows,cols = self.arrow.shape

        M = cv2.getRotationMatrix2D((cols/2,rows/2),angle,1)
        tempArrow = cv2.warpAffine(self.arrow,M,(cols,rows))

        tempMap[y:y+self.arrowSize,x:x+self.arrowSize] = tempArrow
        
        cv2.imshow('f',tempMap)
        cv2.waitKey(t)
        pass
simulateMap = SimulateMap(size,50)
simulateMap.readMap(mapData)
simulateMap.showMapWithCar(0,4,4,'E')
