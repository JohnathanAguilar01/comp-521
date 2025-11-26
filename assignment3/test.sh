make clean
make
sudo dmesg -C
sudo insmod merge_sort.ko my_size=10 my_data=7,12,19,3,18,4,2,6,15,8
sudo dmesg
sudo rmmod merge_sort
make clean
ls
