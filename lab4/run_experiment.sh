#!/bin/bash
printf "running the experiment"

printf "cleaning and making\n"
make clean
make

printf "enter samples to skip\n"
read samples_to_skip

##randtrack
printf "running randtrack\n"
/usr/bin/time randtrack 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack 1 $samples_to_skip > experiment/rt.out
printf "sorting randtrack\n"
sort -n experiment/rt.out > experiment/rt.outs

#rt global lock
printf "running randtrack_global_lock 1 thread\n"
/usr/bin/time randtrack_global_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 1 $samples_to_skip > experiment/rt_global_lock_1.out
printf "running randtrack_global_lock 2 thread\n"
/usr/bin/time randtrack_global_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 2 $samples_to_skip > experiment/rt_global_lock_2.out

printf "running randtrack_global_lock 4 thread\n"
/usr/bin/time randtrack_global_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_global_lock 4 $samples_to_skip > experiment/rt_global_lock_4.out


printf "sorting global_lock\n"
sort -n experiment/rt_global_lock_1.out > experiment/rt_global_lock_1.outs
sort -n experiment/rt_global_lock_2.out > experiment/rt_global_lock_2.outs
sort -n experiment/rt_global_lock_4.out > experiment/rt_global_lock_4.outs

printf "verifying correctness of global_lock\n"
diff experiment/rt.outs experiment/rt_global_lock_1.outs
diff experiment/rt.outs experiment/rt_global_lock_2.outs
diff experiment/rt.outs experiment/rt_global_lock_4.outs

#rt tm
printf "running randtrack_tm 1 thread\n"
/usr/bin/time randtrack_tm 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 1 $samples_to_skip > experiment/rt_tm_1.out

printf "running randtrack_tm 2 thread\n"
/usr/bin/time randtrack_tm 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 2 $samples_to_skip > experiment/rt_tm_2.out

printf "running randtrack_tm 4 thread\n"
/usr/bin/time randtrack_tm 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_tm 4 $samples_to_skip > experiment/rt_tm_4.out


printf "sorting tm\n"
sort -n experiment/rt_tm_1.out > experiment/rt_tm_1.outs
sort -n experiment/rt_tm_2.out > experiment/rt_tm_2.outs
sort -n experiment/rt_tm_4.out > experiment/rt_tm_4.outs

printf "verifying correctness of tm\n"
diff experiment/rt.outs experiment/rt_tm_1.outs
diff experiment/rt.outs experiment/rt_tm_2.outs
diff experiment/rt.outs experiment/rt_tm_4.outs

#rt list_lock
printf "running randtrack_list_lock 1 thread\n"
/usr/bin/time randtrack_list_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 1 $samples_to_skip > experiment/rt_list_lock_1.out

printf "running randtrack_list_lock 2 thread\n"
/usr/bin/time randtrack_list_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 2 $samples_to_skip > experiment/rt_list_lock_2.out

printf "running randtrack_list_lock 4 thread\n"
/usr/bin/time randtrack_list_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_list_lock 4 $samples_to_skip > experiment/rt_list_lock_4.out


printf "sorting list_lock\n"
sort -n experiment/rt_list_lock_1.out > experiment/rt_list_lock_1.outs
sort -n experiment/rt_list_lock_2.out > experiment/rt_list_lock_2.outs
sort -n experiment/rt_list_lock_4.out > experiment/rt_list_lock_4.outs

printf "verifying correctness of list_lock\n"
diff experiment/rt.outs experiment/rt_list_lock_1.outs
diff experiment/rt.outs experiment/rt_list_lock_2.outs
diff experiment/rt.outs experiment/rt_list_lock_4.outs

#rt element_lock
printf "running randtrack_element_lock 1 thread\n"
/usr/bin/time randtrack_element_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 1 $samples_to_skip > experiment/rt_element_lock_1.out

printf "running randtrack_element_lock 2 thread\n"
/usr/bin/time randtrack_element_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 2 $samples_to_skip > experiment/rt_element_lock_2.out

printf "running randtrack_element_lock 4 thread\n"
/usr/bin/time randtrack_element_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_element_lock 4 $samples_to_skip > experiment/rt_element_lock_4.out


printf "sorting element_lock\n"
sort -n experiment/rt_element_lock_1.out > experiment/rt_element_lock_1.outs
sort -n experiment/rt_element_lock_2.out > experiment/rt_element_lock_2.outs
sort -n experiment/rt_element_lock_4.out > experiment/rt_element_lock_4.outs

printf "verifying correctness of element_lock\n"
diff experiment/rt.outs experiment/rt_element_lock_1.outs
diff experiment/rt.outs experiment/rt_element_lock_2.outs
diff experiment/rt.outs experiment/rt_element_lock_4.outs

#rt reduction
printf "running randtrack_reduction 1 thread\n"
/usr/bin/time randtrack_reduction 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 1 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 1 $samples_to_skip > experiment/rt_reduction_1.out

printf "running randtrack_reduction 2 thread\n"
/usr/bin/time randtrack_reduction 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 2 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 2 $samples_to_skip > experiment/rt_reduction_2.out

printf "running randtrack_reduction 4 thread\n"
/usr/bin/time randtrack_reduction 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 4 $samples_to_skip > /dev/null
/usr/bin/time randtrack_reduction 4 $samples_to_skip > experiment/rt_reduction_4.out


printf "sorting reduction\n"
sort -n experiment/rt_reduction_1.out > experiment/rt_reduction_1.outs
sort -n experiment/rt_reduction_2.out > experiment/rt_reduction_2.outs
sort -n experiment/rt_reduction_4.out > experiment/rt_reduction_4.outs

printf "verifying correctness of reduction\n"
diff experiment/rt.outs experiment/rt_reduction_1.outs
diff experiment/rt.outs experiment/rt_reduction_2.outs
diff experiment/rt.outs experiment/rt_reduction_4.outs