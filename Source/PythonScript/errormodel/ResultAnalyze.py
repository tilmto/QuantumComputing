from pylab import *
import re

result=open('result','r')
rate=[]

while 1:
	str=result.readline()
	if not str:
		break
	elif str.find('costB / costA') == 0:
		line=re.findall(r'\d\.?\d*',str)
		rate.append(line[0])		

rate=list(map(float,rate))
print("Benchmark Num: ")
print(len(rate))
print("Average Rate: ")
print(mean(rate))

hist(rate,50)
axis([0.6,1.2,0,13])
xlabel('cost_new/cost_old')
ylabel('count')
show()