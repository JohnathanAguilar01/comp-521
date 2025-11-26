make clean
make
sudo dmesg -C
sudo insmod merge_sort.ko my_size=20 my_data=87,23,45,12,67,89,34,56,78,91,5,42,73,29,61,99,14,38,52,76
sudo dmesg
sudo rmmod merge_sort
make clean
ls
