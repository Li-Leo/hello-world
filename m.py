import subprocess

p=subprocess.Popen('iarbuild gpio_outputinput.ewp -make Release',shell=True,stdin=subprocess.PIPE,stdout=subprocess.PIPE) 
# print(p.stdout.readlines())
# out=''
while p.poll() is None:
    line = p.stdout.readline()
    line = line.decode('utf-8')
    if line:
    	# out += line
    	print(line, end='')
# print('out',out)

# print(p1.communicate())