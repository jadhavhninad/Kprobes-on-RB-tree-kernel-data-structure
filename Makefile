TEST = rb_test
obj-m := rbt530_drv.o RBprobe.o

all:
	make -Wall -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -Wall -o $(TEST) main.c -pthread

clean:
	make -Wall -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm -f *.ko
	rm -f *.o
	rm -f Module.symvers
	rm -f modules.order
	rm -f *.mod.c
	rm -rf .tmp_versions
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f \.*.cmd
	rm -f Module.markers
	rm -f $(TEST) 

