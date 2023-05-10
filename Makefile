obj-m += up_driver.o
obj-m += down_driver.o
obj-m += zoom_driver.o
obj-m += capture_driver.o
RESULT = termproject_code

all:
	make -C ~/working/kernel M=$(PWD) modules

clean:
	make -C ~/working/kernel M=$(PWD) clean
	rm -rf *.ko
	rm -rf *.o
