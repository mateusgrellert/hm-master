import os
from os import system

sequence_list = ['BQMall', 'BQSquare', 'BQTerrace', 'BasketballDrillText', 'BasketballDrill', 'BasketballDrive', 
				'BasketballPass', 'BlowingBubbles', 'Cactus', 'Kimono', 
				'NebutaFestival', 'ParkScene', 'PartyScene', 'PeopleOnStreet', 'RaceHorsesC', 'RaceHorses', 
				'SlideEditing', 'SlideShow', 'SteamLocomotiveTrain', 'Traffic']

nFrames = ['64','64','64','64','64']
QP_list = ['22','27','32','37']

RDO = False

fps = {}
# class A
fps['PeopleOnStreet'] = {"22":"5","27":"6","32":"6","37":"7"}
fps['NebutaFestival'] = {"22":"5","27":"6","32":"6","37":"7"}
fps['SteamLocomotiveTrain'] = {"22":"5","27":"6","32":"6","37":"7"}
fps['Traffic'] = {"22":"5","27":"6","32":"6","37":"7"}

# class B
fps['Kimono'] = {"22":"11","27":"12","32":"14","37":"16"}
fps['ParkScene'] = {"22":"11","27":"12","32":"14","37":"16"}
fps['Cactus'] = {"22":"11","27":"12","32":"14","37":"16"}
fps['BasketballDrive'] = {"22":"11","27":"12","32":"14","37":"16"}
fps['BQTerrace'] = {"22":"11","27":"12","32":"14","37":"16"}

# class C
fps['BasketballDrill'] = {"22":"57","27":"62","32":"68","37":"74"}
fps['RaceHorsesC'] = {"22":"57","27":"62","32":"68","37":"74"}
fps['BQMall'] = {"22":"57","27":"62","32":"68","37":"74"}
fps['PartyScene'] = {"22":"57","27":"62","32":"68","37":"74"}

# class D
fps['BlowingBubbles'] = {"22":"282","27":"306","32":"335","37":"370"}
fps['BasketballPass'] = {"22":"282","27":"306","32":"335","37":"370"}
fps['BQSquare'] = {"22":"282","27":"306","32":"335","37":"370"}
fps['RaceHorses'] = {"22":"282","27":"306","32":"335","37":"370"}

# class E
fps['ChinaSpeed'] = {"22":"22","27":"25","32":"30","37":"34"}
fps['BasketballDrillText'] = {"22":"22","27":"25","32":"30","37":"34"}
fps['SlideEditing'] = {"22":"22","27":"25","32":"30","37":"34"}
fps['SlideShow'] = {"22":"22","27":"25","32":"30","37":"34"}

budget_list = ['4']
configs = ['encoder_lowdelay_main', 'encoder_randomaccess_main']

procFreq = '2000'
procAvail = '1.0'

for config in configs:
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
			strQP = ' --QP='+QP

			strQP += ' --ProcFrequency='+procFreq
			strQP += ' --ProcAvailability='+procAvail
			strQP += ' --TargetFPS=' + fps[sequence][QP]
			strQP += ' --BudgetAlgorithm=4'
			# start running each list here -- don't nest loops from this point on
			
			execLine = strSeq + strQP
			print execLine
			system(execLine)

			system("mkdir QP_"+QP)
			system("mv *csv *hsv ./QP_"+QP)

		if RDO:
			system("mkdir "+ sequence + '_' + config + '_RDO')
			system("mv "+ 'QP_* ./' + sequence + '_' + config + '_RDO')
		else:
			system("mkdir "+ sequence + '_' + config)
			system("mv "+ 'QP_* ./' + sequence + '_' + config)