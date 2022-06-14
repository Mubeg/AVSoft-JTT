obj-m += test_task.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
start_test:
	sudo dmesg -C
	sudo insmod test_task.ko
test:
	./test.sh 1>/dev/null

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
	cat ./Tests/test1.txt > /proc/test_task
	cat /proc/test_task
	make end_test

test4:
	make start_test
	cat ./Tests/test2.txt > /proc/test_task
	cat /proc/test_task
	make end_test

test5:
	make start_test
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat ./Tests/test1.txt > /proc/test_task
	cat /proc/test_task
	make end_test

test6:
	make start_test
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat ./Tests/test2.txt > /proc/test_task
	cat /proc/test_task
	make end_test


end_test:
	sudo rmmod test_task