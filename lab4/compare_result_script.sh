#!/bin/bash

printf "cleaning and making\n"
make clean
make

printf "enter number of threads: 1, 2, or 4\n"
read threads

printf "enter samples to skip\n"
read samples_to_skip

printf "enter test case out of: global_lock, tm, list_lock, element_lock, reduction\n"
read test_case

#randtrack
printf "running randtrack\n"
randtrack $threads $samples_to_skip > output/rt.out
printf "sorting randtrack\n"
sort -n output/rt.out > output/rt.outs

case $test_case in 
  global_lock)

    #rt global lock
    printf "running randtrack_global_lock\n"
    randtrack_global_lock $threads $samples_to_skip > output/rt_global_lock.out
    printf "sorting global lock\n"
    sort -n output/rt_global_lock.out > output/rt_global_lock.outs

    printf "checking diff\n"
    diff output/rt.outs output/rt_global_lock.outs
    ;;

  tm)

    #rt tm
    printf "running randtrack_tm\n"
    randtrack_tm $threads $samples_to_skip > output/rt_tm.out
    printf "sorting global lock\n"
    sort -n output/rt_tm.out > output/rt_tm.outs

    printf "checking diff\n"
    diff output/rt.outs output/rt_tm.outs
    ;;

  list_lock)

    #rt tm
    printf "running randtrack_list_lock\n"
    randtrack_list_lock $threads $samples_to_skip > output/rt_list_lock.out
    printf "sorting list_lock\n"
    sort -n output/rt_list_lock.out > output/rt_list_lock.outs

    printf "checking diff\n"
    diff output/rt.outs output/rt_list_lock.outs
    ;;

  element_lock)

    #rt element_lock
    printf "running randtrack_element_lock\n"
    randtrack_element_lock $threads $samples_to_skip > output/rt_element_lock.out
    printf "sorting element_lock\n"
    sort -n output/rt_element_lock.out > output/rt_element_lock.outs

    printf "checking diff\n"
    diff output/rt.outs output/rt_element_lock.outs
    ;;

  reduction)

    #rt reduction
    printf "running randtrack_reduction\n"
    randtrack_reduction $threads $samples_to_skip > output/rt_reduction.out
    printf "sorting reduction\n"
    sort -n output/rt_reduction.out > output/rt_reduction.outs

    printf "checking diff\n"
    diff output/rt.outs output/rt_reduction.outs
    ;;


  *)
  printf "unknown test case\n"
  ;;

esac