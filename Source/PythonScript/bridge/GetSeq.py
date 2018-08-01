import re
import sys
infile=open(sys.argv[1], 'r')
outfile=open('seq_'+sys.argv[1], 'w')
while 1:
    str=infile.readline()
    if not str:
        break
    elif str.find('cx')==0:
        p = re.compile(r'\d+')
        list=p.findall(str)
        print(list)
        outfile.write(list[0])
        outfile.write(' ')
        outfile.write(list[1])
        outfile.write('\n')
infile.close()
outfile.close()
