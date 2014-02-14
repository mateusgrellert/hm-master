from sys import argv
from math import floor

path = argv[1]
print path

if '10bit' in path:
	print 'Skipping 10-bit cfgs'
	exit()

cfg = open(path,'r')
new_cfg = open(path.split('.')[0]+'.cfg','w')

lines = cfg.readlines()
cfg.close()
sw,sh = -1,-1

CTUsize = 16 # MINIMUM CTU size

for line in lines:
	if 'InputFile' in line:
		tokens = line.split('origCfP/')
		inp_line = tokens[0] + 'origCfP/cropped/'
		inp_yuv = tokens[1].split('_')[0]
	elif 'SourceWidth' in line:
		tokens = line.split(':')
		sw = int(tokens[1].split()[0])
		sw = int(floor(sw/CTUsize))*CTUsize
		print >> new_cfg, tokens[0] + ' : ' + str(sw)
	
	elif 'SourceHeight' in line:
		tokens = line.split(':')
		sh = int(tokens[1].split()[0])
		sh = int(floor(sh/CTUsize))*CTUsize
		print >> new_cfg, tokens[0] + ' : ' + str(sh)
	elif sw != -1 and sh != -1:
		print >> new_cfg, inp_line+inp_yuv+'_'+str(sw)+'x'+str(sh)+'.yuv'
	else:
		print >> new_cfg, line
