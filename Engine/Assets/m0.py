f = open("m1.txt","r")
a = f.read()
f.close()
a = a.replace("\n",",")
a = a.replace(",","\n")
b = open("m1.txt","w")
b.write(a)
print(a)
b.close()
