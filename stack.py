#!/usr/bin/python
import sys

class Stack:
    def __init__(self, size):
        self.size = size
        self.top = 0
        self.values = []
    
    def push(self, item):
        if not (self.top < self.size):
            print "Stack overflow"
            sys.exit(1)
        else:
            self.values.append(item)
            self.top += 1

    def pop(self):
        if (self.top < 1):
            print "Stack is empty"
            sys.exit(1)
        else:
            res = self.values[self.top-1]
            self.top -= 1
            self.values = self.values[:self.top+1]
            return res

test = Stack(5)
vals = [10, 43, -30, 12, 548]

for i in vals:
    test.push(i)

#test.push(14)

for i in range(5):
    print test.pop()

#print test.pop()
