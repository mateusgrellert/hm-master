import os
from os import system

sequence_list = ['BQMall', 'BQSquare', 'BQTerrace', 'BasketballDrillText', 'BasketballDrill', 'BasketballDrive', 
				'BasketballPass', 'BlowingBubbles', 'Cactus', 'Kimono', 'ChinaSpeed', 
				'NebutaFestival', 'ParkScene', 'PartyScene', 'PeopleOnStreet', 'RaceHorsesC', 'RaceHorses', 
				'SlideEditing', 'SlideShow', 'SteamLocomotiveTrain', 'Traffic']

nFrames = ['64','64','64','64','64']
nFrames = '64'
QP_list = ['22','27','32','37']

RDO = False

fps = {}

# THESE VALUES WERE CALCULATED FOR A 2000 MHZ CPU
fps["BQMall_encoder_randomaccess_main"] = ["39.5","42.8","46.3","49.9"]
fps["BQSquare_encoder_randomaccess_main"] = ["240.6","261.0","278.8","289.8"]
fps["BQTerrace_encoder_randomaccess_main"] = ["8.2","9.8","10.9","11.8"]
fps["BasketballDrillText_encoder_randomaccess_main"] = ["36.1","39.1","42.9","46.7"]
fps["BasketballDrill_encoder_randomaccess_main"] = ["34.7","37.8","41.6","45.6"]
fps["BasketballDrive_encoder_randomaccess_main"] = ["5.3","6.1","6.9","7.7"]
fps["BasketballPass_encoder_randomaccess_main"] = ["199.0","213.5","230.3","245.0"]
fps["BlowingBubbles_encoder_randomaccess_main"] = ["151.4","165.8","183.4","201.2"]
fps["Cactus_encoder_randomaccess_main"] = ["7.8","8.8","9.6","10.3"]
fps["ChinaSpeed_encoder_randomaccess_main"] = ["14.1","16.1","19.0","21.9"]
fps["Kimono_encoder_randomaccess_main"] = ["5.6","6.3","7.1","8.0"]
fps["NebutaFestival_encoder_randomaccess_main"] = ["3.0","3.3","4.4","5.6"]
fps["ParkScene_encoder_randomaccess_main"] = ["9.0","10.0","10.8","11.4"]
fps["PartyScene_encoder_randomaccess_main"] = ["36.9","40.2","44.5","49.0"]
fps["PeopleOnStreet_encoder_randomaccess_main"] = ["2.8","3.0","3.3","3.6"]
fps["RaceHorsesC_encoder_randomaccess_main"] = ["22.5","25.0","28.1","31.4"]
fps["RaceHorses_encoder_randomaccess_main"] = ["108.4","117.5","129.0","144.8"]
fps["SlideEditing_encoder_randomaccess_main"] = ["25.5","25.9","26.5","27.0"]
fps["SlideShow_encoder_randomaccess_main"] = ["22.2","23.4","24.6","25.6"]
fps["SteamLocomotiveTrain_encoder_randomaccess_main"] = ["4.0","4.9","5.4","5.8"]
fps["Traffic_encoder_randomaccess_main"] = ["5.1","5.6","6.0","6.3"]


fps["BQMall_encoder_lowdelay_main"] = ["24.6","26.8","29.1","31.4"]
fps["BQSquare_encoder_lowdelay_main"] = ["141.1","155.0","168.4","182.4"]
fps["BQTerrace_encoder_lowdelay_main"] = ["5.2","6.4","7.1","7.7"]
fps["BasketballDrillText_encoder_lowdelay_main"] = ["24.0","25.8","28.0","30.4"]
fps["BasketballDrill_encoder_lowdelay_main"] = ["22.9","24.8","27.1","29.6"]
fps["BasketballDrive_encoder_lowdelay_main"] = ["3.3","3.8","4.3","4.9"]
fps["BasketballPass_encoder_lowdelay_main"] = ["121.6","129.7","139.6","151.1"]
fps["BlowingBubbles_encoder_lowdelay_main"] = ["91.5","99.8","111.2","123.5"]
fps["Cactus_encoder_lowdelay_main"] = ["4.9","5.5","6.0","6.5"]
fps["ChinaSpeed_encoder_lowdelay_main"] = ["9.5","10.7","12.3","14.0"]
fps["Kimono_encoder_lowdelay_main"] = ["3.2","3.6","4.0","4.5"]
fps["ParkScene_encoder_lowdelay_main"] = ["5.2","5.8","6.4","6.9"]
fps["PartyScene_encoder_lowdelay_main"] = ["23.1","25.0","27.6","30.6"]
fps["RaceHorsesC_encoder_lowdelay_main"] = ["14.3","15.6","17.4","19.5"]
fps["RaceHorses_encoder_lowdelay_main"] = ["65.9","70.0","76.5","85.4"]
fps["SlideEditing_encoder_lowdelay_main"] = ["16.9","17.2","17.5","18.0"]
fps["SlideShow_encoder_lowdelay_main"] = ["14.5","15.0","16.0","16.6"]
####

budget_list = ['4']
configs = ['encoder_lowdelay_main', 'encoder_randomaccess_main']

procFreq = '2000'
procAvail = '0.5'

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
		

		if sequence == 'RaceHorsesC':
			seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence[:-1]+'*8*').readlines()
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
			strQP += ' --BudgetAlgorithm=4'
			
			execLine = strSeq + strQP
			print execLine
			system(execLine)

			system("mkdir QP_"+QP)
			system("mv *csv *hsv ./QP_"+QP)
			j += 1

		if RDO:
			system("mkdir "+ sequence + '_' + config + '_RDO')
			system("mv "+ 'QP_* ./' + sequence + '_' + config + '_RDO')
		else:
			system("mkdir "+ sequence + '_' + config)
			system("mv "+ 'QP_* ./' + sequence + '_' + config)