obj-m += test_task.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
start_test:
	sudo dmesg -C
	sudo insmod test_task.ko

test1:
	make start_test
	echo '1' > /proc/test_task
	echo '2' > /proc/test_task
	cat /proc/test_task
	echo '3' > /proc/test_task
	cat /proc/test_task
	make end_test

test2:
	make start_test
	echo "123456789" > /proc/test_task
	cat /proc/test_task
	make end_test

test3:
	make start_test
	echo "12345678" > /proc/test_task
	cat /proc/test_task
	echo "" > /proc/test_task
	make end_test


end_test:
	sudo rmmod test_task