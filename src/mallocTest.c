#include <stdio.h>
#include <stdlib.h>
#include "../headers/dumalloc.h"

void test() {
	printf("\nduMalloc a0\n");
	char** a0 = (char**)duManagedMalloc(64);
	if (a0 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\nduMalloc a1\n");
	Managed_t(char *) a1 = (Managed_t(char*))duManagedMalloc(48);
	if (a1 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	Managed(a1)[0] = 'D';
	Managed(a1)[1] = 'e';
	Managed(a1)[2] = 'n';
	Managed(a1)[3] = 'v';
	Managed(a1)[4] = 'e';
	Managed(a1)[5] = 'r';
	Managed(a1)[6] = '\0';
	printf("\nMemory access is: %s\n", Managed(a1));

	printf("\nduMalloc a2\n");
	Managed_t(char*) a2 = (Managed_t(char*))duManagedMalloc(64);
	if (a2 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\nduMalloc a3\n");
	Managed_t(char*) a3 = (Managed_t(char*))duManagedMalloc(24);
	if (a3 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\nduMalloc a4\n");
	Managed_t(char*) a4 = (Managed_t(char*))duManagedMalloc(88);
	if (a4 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\nduMalloc a5\n");
	Managed_t(char*) a5 = (Managed_t(char*))duManagedMalloc(80);
	if (a5 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\nduFree a0\n");
	duManagedFree((void**)a0);
	duMemoryDump();

	printf("\nduFree a3\n");
	duManagedFree((Managed_t(void*))a3);
	duMemoryDump();

	printf("\nduMalloc a6\n");
	Managed_t(char*) a6 = (Managed_t(char*))duManagedMalloc(160);
	if (a6 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\n********* MINOR COLLECTION ***********\n");
	minorCollection();
	duMemoryDump();

	printf("\nMemory access is: %s\n", Managed(a1));

	printf("\nduFree a2\n");
	duManagedFree((Managed_t(void*))a2);
	duMemoryDump();

	printf("\nduMalloc a7\n");
	Managed_t(char*) a7 = (Managed_t(char*))duManagedMalloc(16);
	if (a7 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\n********* MINOR COLLECTION ***********\n");
	minorCollection();
	duMemoryDump();

	printf("\nduMalloc a8\n");
	Managed_t(char*) a8 = (Managed_t(char*))duManagedMalloc(56);
	if (a8 == NULL) {
		printf("Call to DuMalloc failed\n");
		exit(1);
	}
	duMemoryDump();

	printf("\n********* MINOR COLLECTION **********yay*\n");
	minorCollection();
	duMemoryDump();

	printf("\nduFree a4\n");
	duManagedFree((Managed_t(void*))a4);
	duMemoryDump();

	printf("\nduFree a5\n");
	duManagedFree((Managed_t(void*))a5);
	duMemoryDump();

	printf("\n********* MAJOR COLLECTION ***********\n");
	majorCollection();
	duMemoryDump();

}

int main(int argc, char* argv[]) {

	printf("\nduInitMalloc\n");
	duManagedInitMalloc(FIRST_FIT);
	duMemoryDump();

	test();
}
