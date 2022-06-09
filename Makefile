obj-m += test_task.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test1:
	sudo dmesg -C
	sudo insmod test_task.ko
	echo '10' > /proc/test_task
	cat /proc/test_task
	dmesg
	sudo rmmod test_task