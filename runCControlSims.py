import os
from os import system

sequence_list = ['BQMall', 'BQSquare', 'BQTerrace', 'BasketballDrillText', 'BasketballDrill', 'BasketballDrive', 
				'BasketballPass', 'BlowingBubbles', 'Cactus', 'Kimono', 'ChinaSpeed', 
				'NebutaFestival', 'ParkScene', 'PartyScene', 'PeopleOnStreet', 'RaceHorsesC', 'RaceHorses', 
				'SlideEditing', 'SlideShow', 'SteamLocomotiveTrain', 'Traffic']

sequence_list = ['PeopleOnStreet','Kimono']
nFrames = ['64','64','64','64','64']
nFrames = '64'
QP_list = ['22','27','32','37']

RDO = False

fps = {}

# THESE VALUES WERE CALCULATED FOR A 2000 MHZ CPU
fps['BQMall_encoder_randomaccess_main']=['26.6','28.9','31.2','33.5']
fps['BQSquare_encoder_randomaccess_main']=['146.0','160.8','173.3','180.8']
fps['BQTerrace_encoder_randomaccess_main']=['5.4','6.5','7.3','7.8']
fps['BasketballDrillText_encoder_randomaccess_main']=['24.8','27.0','29.6','32.0']
fps['BasketballDrill_encoder_randomaccess_main']=['24.0','26.3','28.9','31.5']
fps['BasketballDrive_encoder_randomaccess_main']=['3.9','4.5','5.0','5.6']
fps['BasketballPass_encoder_randomaccess_main']=['130.9','140.7','151.6','160.6']
fps['BlowingBubbles_encoder_randomaccess_main']=['101.8','112.4','124.7','136.5']
fps['Cactus_encoder_randomaccess_main']=['5.3','6.0','6.5','6.9']
fps['ChinaSpeed_encoder_randomaccess_main']=['10.2','11.6','13.4','15.3']
fps['Kimono_encoder_randomaccess_main']=['4.0','4.6','5.1','5.7']
fps['NebutaFestival_encoder_randomaccess_main']=['1.9','2.1','2.8','3.6']
fps['ParkScene_encoder_randomaccess_main']=['5.9','6.5','7.1','7.5']
fps['PartyScene_encoder_randomaccess_main']=['24.8','27.2','30.1','33.0']
fps['PeopleOnStreet_encoder_randomaccess_main']=['2.0','2.2','2.4','2.6']
fps['RaceHorsesC_encoder_randomaccess_main']=['16.8','18.7','20.9','23.3']
fps['RaceHorses_encoder_randomaccess_main']=['78.0','84.9','93.4','104.5']
fps['SlideEditing_encoder_randomaccess_main']=['17.1','17.2','17.5','17.8']
fps['SlideShow_encoder_randomaccess_main']=['15.4','16.0','16.6','17.0']
fps['SteamLocomotiveTrain_encoder_randomaccess_main']=['2.6','3.3','3.6','3.8']
fps['Traffic_encoder_randomaccess_main']=['3.2','3.6','3.8','4.0']


fps['BQMall_encoder_lowdelay_main']=['17.3','18.9','20.5','22.0']
fps['BQSquare_encoder_lowdelay_main']=['88.2','98.4','107.9','117.5']
fps['BQTerrace_encoder_lowdelay_main']=['3.5','4.4','4.9','5.3']
fps['BasketballDrillText_encoder_lowdelay_main']=['16.9','18.3','19.9','21.5']
fps['BasketballDrill_encoder_lowdelay_main']=['16.3','17.7','19.4','21.1']
fps['BasketballDrive_encoder_lowdelay_main']=['2.6','2.9','3.3','3.7']
fps['BasketballPass_encoder_lowdelay_main']=['84.2','89.8','96.4','103.6']
fps['BlowingBubbles_encoder_lowdelay_main']=['64.5','70.8','79.1','87.9']
fps['Cactus_encoder_lowdelay_main']=['3.4','3.9','4.2','4.5']
fps['ChinaSpeed_encoder_lowdelay_main']=['7.1','7.9','9.0','10.1']
fps['Kimono_encoder_lowdelay_main']=['2.5','2.7','3.1','3.4']
fps['ParkScene_encoder_lowdelay_main']=['3.6','4.0','4.5','4.8']
fps['PartyScene_encoder_lowdelay_main']=['15.9','17.4','19.2','21.3']
fps['RaceHorsesC_encoder_lowdelay_main']=['11.1','12.1','13.4','15.0']
fps['RaceHorses_encoder_lowdelay_main']=['50.0','53.1','58.1','64.6']
fps['SlideEditing_encoder_lowdelay_main']=['11.7','11.8','12.0','12.2']
fps['SlideShow_encoder_lowdelay_main']=['10.4','10.6','11.1','11.4']

budget_list = ['5']
configs = ['encoder_lowdelay_main', 'encoder_randomaccess_main']

procFreq = '2000'
procAvail = '0.6'

for config in configs:
	i = 0

	for sequence in sequence_list:
		if sequence in ["PeopleOnStreet","NebutaFestival","SteamLocomotiveTrain","Traffic"] and config=="encoder_lowdelay_main":
			continue
		strSeq = './TAppEncoderStatic -c ../cfg/'+config+'.cfg -c ../cfg/per-sequence/'+sequence
		
		if sequence == 'NebutaFestival' or sequence == 'SteamLocomotiveTrain':
			strSeq += '_10bit.cfg'
		else:
			strSeq += '.cfg'
		

		if sequence == 'BasketballDrill':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'_*').readlines()

		if sequence == 'RaceHorsesC':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*8*').readlines()
		else:
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
		
		seq_path = seq_path[0].strip('\n')

		strSeq += ' --InputFile=\"'+seq_path+'\"'
		strSeq += ' --FramesToBeEncoded=' + nFrames
		i += 1
		j = 0
		for QP in QP_list:
			strQP = ' --QP='+QP

			strQP += ' --ProcFrequency='+procFreq
			strQP += ' --ProcAvailability='+procAvail
			strQP += ' --TargetFPS=' + fps[sequence+"_"+config][j]
			strQP += ' --BudgetAlgorithm=5'
			
			execLine = strSeq + strQP
			print execLine
			system(execLine)

			system("mkdir QP_"+QP)
			system("mv *csv ./QP_"+QP)
			j += 1

		if RDO:
			system("mkdir "+ sequence + '_' + config + '_RDO')
			system("mv "+ 'QP_* ./' + sequence + '_' + config + '_RDO')
		else:
			system("mkdir "+ sequence + '_' + config)
			system("mv "+ 'QP_* ./' + sequence + '_' + config)