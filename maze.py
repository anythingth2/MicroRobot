import numpy as np
import cv2

mapData = []
size = 11
with open('map.txt','r') as f:
    mapData = [line[:len(line)-1] for line in f.readlines()]
    for i in range(len(mapData)):
        data = []
        for j in range(len(mapData[i])):
            data.append(ord(mapData[i][j]) - ord('A'))
        mapData[i] = data


class SimulateMap:
    
    def __init__(self,nGrid,size,name):
        self.size = size
        self.nGrid = nGrid
        self.name = name
        self.image = np.zeros((nGrid*size,nGrid*size),dtype = 'float32')
        self.arrowSize = int(self.size*0.7)
        self.arrow = np.zeros((self.arrowSize,self.arrowSize),dtype='float32')
        self.arrow[int(size*0.3):int(size*0.7),size//2 - int(size*0.05):size//2 + int(size*0.05)] = 1.0
        self.arrow[int(size*0.5):int(size*0.7),size//2 - int(size*0.2):size//2 + int(size*0.2)] = 1.0
        
    def drawGrid(self,x,y,_type):
        radius = int(self.size *1.0)
        thickness = 5
        x *= self.size
        y *= self.size
        if _type & 0b10000:
            self.image[ y  :y + self.size,x :x +self.size] = 0.0
        else:
            self.image[ y  :y + self.size,x :x +self.size] = 0.0
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
        cv2.imshow(self.name,self.image)
        cv2.waitKey(t*10**3)
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
        baseX = int(x*(self.size+0.5))
        baseY = int(y*(self.size+0.5)) 
        tempMap[baseY:baseY+self.arrowSize ,baseX:baseX+self.arrowSize] = tempArrow
        cv2.imshow(self.name,tempMap)
        if cv2.waitKey(t*10**3) == ord('q'):
            cv2.destroyAllWindows()
            exit()

class Path:
    def __init__(self,x,y,direction,goTo):
        self.x = x
        self.y = y
        self.direction = direction
        self.goTo = goTo

        
carX =8
carY = 8
carDirection = 'N'
carMap = [[0b10000 for i in range(size)] for j in range(size)]
pathStack = [i for i in range(100)]
topOfStack = 0

def pushPath(goTo):
    global pathStack, topOfStack
    pathStack[topOfStack] = Path(carX,carY,carDirection,goTo)
    topOfStack += 1
def popPath():
    topOfStack -= 1
    return pathStack[topOfStack+1]
realSimulateMap = SimulateMap(size,50,'REAL MAP')
realSimulateMap.readMap(mapData)

carSimulateMap = SimulateMap(size,50,'CAR MAP')
carSimulateMap.readMap(carMap)

def move():
    global carX,carY,carDirection
    if carDirection == 'N':
        carY -= 1
    elif carDirection == 'W':
        carX -= 1
    elif carDirection == 'S':
        carY += 1
    elif carDirection == 'E':
        carX += 1
def moveBack():
    global carX,carY,carDirection
    if carDirection == 'N':
        carY += 1
    elif carDirection == 'W':
        carX += 1
    elif carDirection == 'S':
        carY -= 1
    elif carDirection == 'E':
        carX -= 1
def moveBackToLastPath():
    lastPath = popPath()
    while lastPath.goTo == 2:
        move
def turnLeft():
    global carDirection
    if carDirection == 'N':
        carDirection = 'W'
    elif carDirection == 'W':
        carDirection = 'S'
    elif carDirection == 'S':
        carDirection = 'E'
    elif carDirection == 'E':
        carDirection = 'N'

def turnRight():
    global carDirection
    if carDirection == 'N':
        carDirection = 'E'
    elif carDirection == 'W':
        carDirection = 'N'
    elif carDirection == 'S':
        carDirection = 'W'
    elif carDirection == 'E':
        carDirection = 'S'
'''
BOTTOM-LEFT-TOP-RIGHT
'''
def getRealDirection(direction,expect):
    if expect == 'L':
        if direction == 'N':
            return 'W'
        elif direction == 'W':
            return 'S'
        elif direction == 'S':
            return 'E'
        elif direction == 'E':
            return 'N'
    elif expect == 'F':
        return direction
    elif expect == 'R':
        if direction == 'N':
            return 'E'
        elif direction == 'W':
            return 'N'
        elif direction == 'S':
            return 'W'
        elif direction == 'E':
            return 'S'
def checkLeft():
    global carX,carY,carDirection
    block = mapData[carY][carX]
    if carDirection == 'N' and block & 0b0100:
        return True
    elif carDirection == 'W' and block & 0b1000:
        return True
    elif carDirection == 'S' and block & 0b0001:
        return True
    elif carDirection == 'E' and block & 0b0010:
        return True
    return False
def checkFront():
    global carX,carY,carDirection
    block = mapData[carY][carX]
    if carDirection == 'N' and block & 0b0010:
        return True
    elif carDirection == 'W' and block & 0b0100:
        return True
    elif carDirection == 'S' and block & 0b1000:
        return True
    elif carDirection == 'E' and block & 0b0001:
        return True
    return False
def checkRight():
    global carX,carY,carDirection
    block = mapData[carY][carX]
    if carDirection == 'N' and block & 0b0001:
        return True
    elif carDirection == 'W' and block & 0b0010:
        return True
    elif carDirection == 'S' and block & 0b0100:
        return True
    elif carDirection == 'E' and block & 0b1000:
        return True
    return False
    
def runLeftHandMethod():
    isHasLeft = checkLeft()
    isHasFront = checkFront()
    isHasRight = checkRight()
    getRealDirection(carDirection,)
    carMap[carY][carX] = getRealDirection

    if not checkLeft():
        turnLeft()
        move()
        pushPath(0)
    elif not checkFront():
        move()
        pushPath(1)
    elif not checkRight():
        turnRight()
        move()
        pushPath(2)
    else:
        turnLeft()
        turnLeft()
        move()
        turnRight()
        turnRight()

for i in range(100):
    runLeftHandMethod()
    
    realSimulateMap.showMapWithCar(0,carX,carY,carDirection)
    carSimulateMap.readMap(carMap)
    carSimulateMap.showMap(0)