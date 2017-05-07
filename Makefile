binspy: binspy.c
	$(CC) -o binspy binspy.c

.PHONY: test
test: binspy.c
	$(CC) -o binspy_tests binspy.c -DTESTING
	./binspy_tests
