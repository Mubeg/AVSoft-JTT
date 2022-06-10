obj-m += test_task.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test1:
	sudo dmesg -C
	sudo insmod test_task.ko

	echo '1' > /proc/test_task
	echo '2' > /proc/test_task
	cat /proc/test_task
	echo '3' > /proc/test_task
	cat /proc/test_task

	sudo rmmod test_task