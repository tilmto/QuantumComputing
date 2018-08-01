import re
import os

rootdir='/home/tilmto/Ericpy/QuantumComputing/bridge/benchmark'

list=os.listdir(rootdir)

for filename in list:
	filepath=os.path.join(rootdir,filename)
	infile=open(filepath, 'r')
	outfile=open('//home/tilmto/Ericpy/QuantumComputing/bridge/seq/seq_'+filename, 'w')
	
	for i in range(4):
		str=infile.readline()

	while 1:
		str=infile.readline()

		if not str:
			break

		elif str.find('cx') == 0:
			list=re.findall(r'\d+',str)
			outfile.write(list[0])
			outfile.write(' ')
			outfile.write(list[1])
			outfile.write('\n')

		elif str.find('rz') == 0:
			list=re.findall(r'\d+\.?\d*',str)
			outfile.write('-1')
			outfile.write(' ')
			outfile.write(list[1])
			outfile.write('\n')
		
		elif str.find('h') == 0:
			list=re.findall(r'\d+',str)
			outfile.write('-2')
			outfile.write(' ')
			outfile.write(list[0])
			outfile.write('\n')
			
		else:
			list=re.findall(r'\d+',str)
			outfile.write('-1')
			outfile.write(' ')
			outfile.write(list[0])
			outfile.write('\n')

	infile.close()
	outfile.close()
