from pylab import *
import re
import csv

result=open('result','r')
headers=['Benchmark','Gate Cost','Time']
rows=[]
rate=[]

while 1:
	str=result.readline()

	if not str:
		break

	elif str.find('qasm')!=-1:
		benchmark=str[4:]
		benchmark=benchmark[:-2]

	elif str.find('Total Cost of HardwareB is:')!=-1:
		gate=re.findall(r'\d+',str)

	elif str.find('Execution Time')!=-1:
		time=re.findall(r'\d\.?\d*e?-?\d*',str)

	elif str.find('costB / costA')!=-1:
		r=re.findall(r'\d\.?\d*',str)
		rate.append(r[0])
		rows.append({'Benchmark':benchmark,'Gate Cost':gate[0],'Time':time[0]})		

result.close()

print(rows)

with open('BridgeAnalysis.csv','w') as f:
	fcsv=csv.DictWriter(f,headers)
	fcsv.writeheader()
	fcsv.writerows(rows)
	

rate=list(map(float,rate))

print("Benchmark Num: ")
print(len(rate))
print("Average Rate: ")
print(mean(rate))

hist(rate,50)
xlabel('cost_new/cost_old')
ylabel('count')
show()

