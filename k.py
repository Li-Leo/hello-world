import keyboard, os, time

# keyboard.add_hotkey('space', print, args=['space was pressed'])

L=[i for i in os.listdir('.') if os.path.isfile(i) and os.path.splitext(i)[1] =='.txt']
print(L)

# lll=[]
# lll=os.popen('git --version').readlines()
# print(lll)
'''
for n in L:
	name = 'subl.exe' + ' ' + n
	os.system(name)
	time.sleep(3)


for n in range(len(L)):
	keyboard.send('ctrl+shift+s')
	time.sleep(2)
	keyboard.write(L[len(L)-n-1] + '.rb')
	keyboard.send('enter')
	time.sleep(2)	
	keyboard.send('ctrl+w')
	time.sleep(1)
'''
N = [i for i in os.listdir('.') if os.path.isfile(i) and os.path.splitext(i)[1] =='.txt1']
for i in range(len(L)):
	nn = N[i]
	os.rename(N[i], nn[:len(nn)-3]+'.rb')
print(N)