
DIRS = blink button

all:
	for i in $(DIRS); do make -C $$i || exit 1; done


clean:
	for i in $(DIRS); do make -C $$i clean || exit 1; done
