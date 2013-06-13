from sys import os

sequence_list = ['PeopleOnStreet','Kimono','BasketballDrive','BlowingBubbles','ChineSpeed']

nFrames = ['15','15','50','50','50']
QP_list = ['22','32']

MaxPartitionDepth_list = ['3','2','1']
QuadtreeTUMaxDepthInter_list = ['3','2','1']
SearchRange_list = ['32','16','8']
HadamardME_list = ['0']
AMP_list = ['0']
RDOQ_list = ['0']

execLine = './bin/TAppEncoderStatic -c ./cfg/encoder_randomaccess_main.cfg'

for sequence in sequence_list:
	execLine += ' -c ./cfg/per-sequence/'+sequence+'.cfg'

	for QP in QP_list:
		execLine += ' --QP='+QP

		# start running each list here -- don't nest loops from this point on
		for MaxPartitionDepth in MaxPartitionDepth_list:
			os(execLine+' --MaxPartitionDepth='+MaxPartitionDepth)
			wrapResults(sequence+"_"+qp+"_"+"maxDepth_"+MaxPartitionDepth)

		for QuadtreeTUMaxDepthInter in QuadtreeTUMaxDepthInter_list:
			os(execLine+' --QuadtreeTUMaxDepthInter='+QuadtreeTUMaxDepthInter)
			wrapResults(sequence+"_"+qp+"_"+"TUDepth_"+QuadtreeTUMaxDepthInter)

		for SearchRange in SearchRange_list:
			os(execLine+' --SearchRange='+SearchRange)
			wrapResults(sequence+"_"+qp+"_"+"SerchRange_"+SerchRange)

		for HadamardME in HadamardME_list:
			os(execLine+' --HadamardME='+HadamardME)
			wrapResults(sequence+"_"+qp+"_"+"Had_"+HadamardME)

		for AMP in AMP_list:
			os(execLine+' --AMP='+AMP)
			wrapResults(sequence+"_"+qp+"_"+"AMP_"+AMP)

		for RDOQ in RDOQ_list:
			os(execLine+' --RDOQ='+RDOQ)
			wrapResults(sequence+"_"+qp+"_"+"RDOQ_"+RDOQ)
