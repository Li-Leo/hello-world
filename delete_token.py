
with open('Read222Code.txt', 'r') as f_in, open('code.txt', 'w') as f_out:
    for line in f_in:
       	if '@' not in line:
            f_out.write(line)

# with open('Read222Code.txt', 'r') as f1, open('code.txt', 'r') as f2:
# 	n = 0
# 	m = 0
# 	p = 0
    
# 	for line in f1:
# 		if '@' in line:
# 			m += 1
#             # f_out.write(line)
# 		n += 1
	
# 	for line in f2:
# 		p += 1
# 	print(n, m, p)