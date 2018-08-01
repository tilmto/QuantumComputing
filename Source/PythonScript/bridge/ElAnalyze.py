import re
import csv

exeinfo=open('exeinfo','r')
headers=['Benchmark','Gate Cost','Time']
rows=[]

while 1:
	str=exeinfo.readline()

	if not str:
		break;

	elif str.find('Circuit name')!=-1:
		benchmark=str[14:]
		benchmark=benchmark[:-21]

	elif str.find('elementary gates')!=-1:
		gate=re.findall(r'\d+',str)

	elif str.find('seconds')!=-1:
		time=re.findall(r'\d+.?\d*e?-?\d*',str)
		rows.append({'Benchmark':benchmark,'Gate Cost':gate[0],'Time':time[0]})

with open('Elayer.csv','w') as f:
	fcsv=csv.DictWriter(f,headers)
	fcsv.writeheader()
	fcsv.writerows(rows)
