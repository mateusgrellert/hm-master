from os import system

def wrapResults(path):
	system("mkdir results_"+path)
	system("mv *csv *txt ./results_"+path)


sequence_list = ['PeopleOnStreet','Kimono','BasketballDrive','BlowingBubbles','ChinaSpeed']

nFrames = ['15','15','50','50','50']
nFrames = '64'
QP_list = ['22','27','32','37']

MaxPartitionDepth_list = ['1','2','3']
QuadtreeTUMaxDepthInter_list = ['3','2','1']
SearchRange_list = ['32','16','8']
HadamardME_list = ['0']
AMP_list = ['0']
RDOQ_list = ['0']


baseLine = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main.cfg'

i = 0

for sequence in sequence_list:
	execLine = baseLine + ' -c ../cfg/per-sequence/'+sequence+'_cropped.cfg'
	execLine += ' --FramesToBeEncoded=' + nFrames
	i += 1

	for QP in QP_list:
		execLine += ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		print execLine+' --SearchRange=32 --AMP=0 --QuadtreeTUMaxDepthInter=1'
		system(execLine+' --SearchRange=32 --AMP=0 --QuadtreeTUMaxDepthInter=1')
		wrapResults(sequence+"_"+QP+"_"+"PS1")

		print execLine+' --SearchRange=16 --AMP=0 --QuadtreeTUMaxDepthInter=1 --HadamardME=0'
		system(execLine+' --SearchRange=16 --AMP=0 --QuadtreeTUMaxDepthInter=1 --HadamardME=0')
		wrapResults(sequence+"_"+QP+"_"+"PS2")

		print execLine+' --SearchRange=8 --AMP=0 --QuadtreeTUMaxDepthInter=1 --HadamardME=0 --MaxPartitionDepth=2'
		system(execLine+' --SearchRange=8 --AMP=0 --QuadtreeTUMaxDepthInter=1 --HadamardME=0 --MaxPartitionDepth=2')
		wrapResults(sequence+"_"+QP+"_"+"PS3")

		system("mkdir QP_"+QP)
		system("mv results_* ./QP_"+QP)

	system("mkdir "+ sequence)
	system("mv QP_* ./"+sequence)

