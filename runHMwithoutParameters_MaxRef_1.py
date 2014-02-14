import os
from os import system

def wrapResults(path):
	system("mkdir results_"+path)
	system("mv *csv *txt ./results_"+path)


sequence_list = ['BasketballDrill']
nFrames = ['15','50']
#nFrames = '64'
QP_list = ['32','37']


config = 'encoder_randomaccess_main_maxref1'

i = 0

for sequence in sequence_list:

	strSeq = './TAppEncoderStatic -c ../cfg/'+config+'.cfg -c ../cfg/per-sequence/'+sequence
		
	if sequence == 'NebutaFestival' or sequence == 'SteamLocomotiveTrain':
		strSeq += '_10bit.cfg'
	else:
		strSeq += '.cfg'
		
	if sequence == 'RaceHorsesC':
		seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence[:-1]+'*8*').readlines()
	else:
		seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
		
	seq_path = seq_path[0].strip('\n')

	strSeq += ' --InputFile=\"'+seq_path+'\"'
	strSeq += ' --FramesToBeEncoded=' + nFrames[i]

	i += 1

	for QP in QP_list:
		execLine = strSeq + ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		print execLine
		system(execLine)
		wrapResults(sequence+"_"+QP+"_"+"maxRefFrames_1")

		system("mkdir QP_"+QP)
		system("mv results_* ./QP_"+QP)

	system("mkdir "+ sequence)
	system("mv QP_* ./"+sequence)

