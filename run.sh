i=1
while [ $i -le 4 ]
do
	cd bin$i 
	screen python runEvaluationSims.py &
	cd ..
	let i++
done
