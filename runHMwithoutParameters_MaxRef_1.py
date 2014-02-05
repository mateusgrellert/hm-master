from os import system

def wrapResults(path):
	system("mkdir results_"+path)
	system("mv *csv *txt ./results_"+path)


sequence_list = ['PeopleOnStreet','Kimono','BasketballDrive','BlowingBubbles','ChinaSpeed']

nFrames = ['15','15','50','50','50']
#nFrames = '64'
QP_list = ['22','27','32','37']


baseLine = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main_maxref1.cfg'

i = 0

for sequence in sequence_list:
	seqLine = baseLine + ' -c ../cfg/per-sequence/'+sequence+'.cfg'
	seqLine += ' --FramesToBeEncoded=' + nFrames[i]
	i += 1

	for QP in QP_list:
		execLine = seqLine + ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		print execLine
		system(execLine)
		wrapResults(sequence+"_"+QP+"_"+"maxRefFrames_1")

		system("mkdir QP_"+QP)
		system("mv results_* ./QP_"+QP)

	system("mkdir "+ sequence)
	system("mv QP_* ./"+sequence)

