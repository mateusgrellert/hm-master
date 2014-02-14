import os
from os import system

def wrapResults(path):
	system("mkdir -p results_"+path)
	system("mv *csv *hsv ./results_"+path)


sequence_list = ['PeopleOnStreet']

nFrames = '64'

QP_list = ['22','27','32','37']


i = 0

for sequence in sequence_list:
	for QP in QP_list:

		strSeq = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main.cfg -c ../cfg/per-sequence/'
		strSeq += sequence
			

		if sequence == 'NebutaFestival' or sequence == 'SteamLocomotiveTrain':
			strSeq += '_10bit.cfg'
		else:
			strSeq += '.cfg'
		
		if sequence == 'RaceHorsesC':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence[:-1]+'*8*').readlines()
		elif sequence in ['Kimono','BasketballDrive','Cactus','BQTerrace']:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/cropped/'+sequence[:-1]+'*').readlines()
		else:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
		
		seq_path = seq_path[0].strip('\n')

		strSeq += ' --InputFile=\"'+seq_path+'\"'
		strSeq += ' --FramesToBeEncoded=' + nFrames

		qpLine = strSeq + ' --QP='+QP
		
		# PS 0
		finalLine = qpLine
		print finalLine
		system(finalLine)
		wrapResults(sequence+"_"+QP+"_"+"PS0")

		# PS 1
		finalLine = qpLine + ' --SearchRange=32'
		print finalLine
		system(finalLine)
		wrapResults(sequence+"_"+QP+"_"+"PS1")

		# PS 2
		finalLine = qpLine + ' --SearchRange=32'
		finalLine += ' --AMP=0'
		finalLine += ' --QuadtreeTUMaxDepthInter=1'
		print finalLine
		system(finalLine)
		wrapResults(sequence+"_"+QP+"_"+"PS2")
		
		# PS 3
		strSeq = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main.cfg -c ../cfg/per-sequence/'
		if sequence in ['Kimono','BasketballDrive','Cactus','BQTerrace']:
			strSeq += 'cropped/'
		strSeq += sequence

		if sequence == 'NebutaFestival' or sequence == 'SteamLocomotiveTrain':
			strSeq += '_10bit.cfg'
		else:
			strSeq += '.cfg'
			
		if sequence == 'RaceHorsesC':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence[:-1]+'*8*').readlines()
		elif sequence in ['Kimono','BasketballDrive','Cactus','BQTerrace']:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/cropped/'+sequence[:-1]+'*').readlines()
		else:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
			
		seq_path = seq_path[0].strip('\n')

		strSeq += ' --InputFile=\"'+seq_path+'\"'
		strSeq += ' --FramesToBeEncoded=' + nFrames
		qpLine = strSeq + ' --QP='+QP


		finalLine = qpLine + ' --SearchRange=16'
		finalLine += ' --AMP=0'
		finalLine += ' --QuadtreeTUMaxDepthInter=1'
		finalLine += ' --HadamardME=0'
		finalLine += ' --MaxPartitionDepth=3'
		print finalLine
		system(finalLine)
		wrapResults(sequence+"_"+QP+"_"+"PS3")
"""
		# PS 4
		strSeq = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main_maxref1.cfg -c ../cfg/per-sequence/'
		if sequence in ['Kimono','BasketballDrive','Cactus','BQTerrace']:
			strSeq += 'cropped/'
		strSeq += sequence

		if sequence == 'NebutaFestival' or sequence == 'SteamLocomotiveTrain':
			strSeq += '_10bit.cfg'
		else:
			strSeq += '.cfg'
			
		if sequence == 'RaceHorsesC':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence[:-1]+'*8*').readlines()
		elif sequence in ['Kimono','BasketballDrive','Cactus','BQTerrace']:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/cropped/'+sequence[:-1]+'*').readlines()
		else:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
			
		seq_path = seq_path[0].strip('\n')

		strSeq += ' --InputFile=\"'+seq_path+'\"'
		strSeq += ' --FramesToBeEncoded=' + nFrames
		qpLine = strSeq + ' --QP='+QP

		finalLine = qpLine + ' --SearchRange=8'
		finalLine += ' --AMP=0'
		finalLine += ' --QuadtreeTUMaxDepthInter=1'
		finalLine += ' --HadamardME=0'
		finalLine += ' --MaxPartitionDepth=3'
		print finalLine
		system(finalLine)
		wrapResults(sequence+"_"+QP+"_"+"PS4")

		system("mkdir -p QP_"+QP)
		system("mv results_* ./QP_"+QP)
		"""

	system("mkdir -p "+ sequence)
	system("mv QP_* ./"+sequence)

