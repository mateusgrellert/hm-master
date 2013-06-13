from os import system

def wrapResults(path):
	system("mkdir results_"+path)
	system("mv *csv ./results_"+path)


sequence_list = ['PeopleOnStreet','Kimono','BasketballDrive','BlowingBubbles','ChinaSpeed']

nFrames = ['15','15','50','50','50']
QP_list = ['22','32']

MaxPartitionDepth_list = ['3','2','1']
QuadtreeTUMaxDepthInter_list = ['3','2','1']
SearchRange_list = ['32','16','8']
HadamardME_list = ['0']
AMP_list = ['0']
RDOQ_list = ['0']

execLine = './TAppEncoderStatic -c ../cfg/encoder_randomaccess_main.cfg'

for sequence in sequence_list:
	execLine += ' -c ../cfg/per-sequence/'+sequence+'.cfg'

	for QP in QP_list:
		execLine += ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		for MaxPartitionDepth in MaxPartitionDepth_list:
			system(execLine+' --MaxPartitionDepth='+MaxPartitionDepth)
			wrapResults(sequence+"_"+QP+"_"+"maxDepth_"+MaxPartitionDepth)

		for QuadtreeTUMaxDepthInter in QuadtreeTUMaxDepthInter_list:
			system(execLine+' --QuadtreeTUMaxDepthInter='+QuadtreeTUMaxDepthInter)
			wrapResults(sequence+"_"+QP+"_"+"TUDepth_"+QuadtreeTUMaxDepthInter)

		for SearchRange in SearchRange_list:
			system(execLine+' --SearchRange='+SearchRange)
			wrapResults(sequence+"_"+QP+"_"+"SerchRange_"+SearchRange)

		for HadamardME in HadamardME_list:
			system(execLine+' --HadamardME='+HadamardME)
			wrapResults(sequence+"_"+QP+"_"+"Had_"+HadamardME)

		for AMP in AMP_list:
			system(execLine+' --AMP='+AMP)
			wrapResults(sequence+"_"+QP+"_"+"AMP_"+AMP)

		for RDOQ in RDOQ_list:
			system(execLine+' --RDOQ='+RDOQ)
			wrapResults(sequence+"_"+QP+"_"+"RDOQ_"+RDOQ)

		system("mkdir QP_"+QP)
		system("mv results_* ./QP_"+QP)

	system("mkdir "+ sequence)
	system("mv QP_* ./"+sequence)
