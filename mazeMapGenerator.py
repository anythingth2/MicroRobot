import random

SIZE = 9
'''
BOTTOM-LEFT-TOP-RIGHT
'''
def generateRow(size):
    return [random.randint(0,0b1111+1) for x in range(size)]
lines = []
lines.append(''.join([chr(ord('A')+0b1000) for x in range(SIZE+2)])+'\n')
for i in range(SIZE):
    line = []
    line.append(0b0001)
    line.extend( generateRow(SIZE))
    line.append(0b0100)
    line = [chr(ord('A')+ele) for ele in line]
    lines.append(''.join(line)+'\n')

lines.append(''.join([chr(ord('A')+0b0010) for x in range(SIZE+2)])+'\n')

with open('testMapPython.txt','w') as f:
    f.writelines(lines)
