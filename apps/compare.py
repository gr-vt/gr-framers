import numpy as np

data1=np.fromfile("out.dat",np.dtype(np.uint8))
data2=np.fromfile("out2.dat",np.dtype(np.uint8))

#data1=data1[data1>1]
#data2=data2[data2>1]
#print len(data1)
#print len(data2)
#for i in range(min(len(data1),len(data2))):
for i in range(len(data1)):
        print hex(data1[i])+"\t"+ str(data1[i])+"\t"+\
            hex(data1[i])+"\t"+ str(data1[i])
