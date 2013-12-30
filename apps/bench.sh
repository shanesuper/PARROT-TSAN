function after_run {
	filename=$1
	values=(`cat $filename`)
	cmd="("
	for value in ${values[@]}
	do
		cmd="${cmd}${value}+"
	done
	echo "mean" | tee -a $filename
	mean=`echo "${cmd}0)/${#values[@]}"|bc -l`
	echo $mean | tee -a $filename
	cmd2="("
    for value in ${values[@]}
    do  
        cmd2="${cmd2}($value-$mean)*($value-$mean)+"
    done
    echo "sem" | tee -a $filename
    sem=`echo "${cmd2}0)/sqrt(${#values[@]})"|bc -l`
    echo $sem | tee -a $filename
}
