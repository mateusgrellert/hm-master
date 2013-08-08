from os import system

def wrapResults(path):
	system("mkdir results_"+path)
	system("mv *csv *hsv ./results_"+path)


sequence_list = ['Kimono']

nFrames = ['15','15','50','50','50']
QP_list = ['22','32']

MaxPartitionDepth_list = ['1','2','3']
QuadtreeTUMaxDepthInter_list = ['3','2','1']
SearchRange_list = ['32','16','8']
HadamardME_list = ['0']
AMP_list = ['0']
RDOQ_list = ['0']

execLine = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main.cfg'

i = 0

for sequence in sequence_list:
	execLine += ' -c ../cfg/per-sequence/'+sequence+'_cropped.cfg'
	execLine += ' --FramesToBeEncoded=' + nFrames[i]
	i += 1

	for QP in QP_list:
		execLine += ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		for MaxPartitionDepth in MaxPartitionDepth_list:
			print execLine+' --MaxPartitionDepth='+MaxPartitionDepth
			system(execLine+' --MaxPartitionDepth='+MaxPartitionDepth)
			wrapResults(sequence+"_"+QP+"_"+"maxDepth_"+MaxPartitionDepth)

		for QuadtreeTUMaxDepthInter in QuadtreeTUMaxDepthInter_list:
			print execLine+' --QuadtreeTUMaxDepthInter='+QuadtreeTUMaxDepthInter
			system(execLine+' --QuadtreeTUMaxDepthInter='+QuadtreeTUMaxDepthInter)
			wrapResults(sequence+"_"+QP+"_"+"TUDepth_"+QuadtreeTUMaxDepthInter)

		for SearchRange in SearchRange_list:
			print execLine+' --SearchRange='+SearchRange
			system(execLine+' --SearchRange='+SearchRange)
			wrapResults(sequence+"_"+QP+"_"+"SerchRange_"+SearchRange)

		for HadamardME in HadamardME_list:
			print execLine+' --HadamardME='+HadamardME
			system(execLine+' --HadamardME='+HadamardME)
			wrapResults(sequence+"_"+QP+"_"+"Had_"+HadamardME)

		for AMP in AMP_list:
			print execLine+' --AMP='+AMP
			system(execLine+' --AMP='+AMP)
			wrapResults(sequence+"_"+QP+"_"+"AMP_"+AMP)

		for RDOQ in RDOQ_list:
			print execLine+' --RDOQ='+RDOQ
			system(execLine+' --RDOQ='+RDOQ)
			wrapResults(sequence+"_"+QP+"_"+"RDOQ_"+RDOQ)

		system("mkdir QP_"+QP)
		system("mv results_* ./QP_"+QP)

	system("mkdir "+ sequence)
	system("mv QP_* ./"+sequence)

