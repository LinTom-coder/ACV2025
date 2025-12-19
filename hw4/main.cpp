#include "Header.h"

int main()
{
	printf("1: feature matching\n");
	printf("2: diff_scale feature matching\n");
	printf("3: \n");
	printf("4: \n");
	printf("5: \n");
	//printf("6: \n");
	//printf("7: \n");
	char num;
	printf("Enter the question number to select output result:");
	scanf("%c", &num);
	switch (num)
	{
		case '1': 
			a();
			break;
		case '2': 
			b();
			break;
		case '3':
			c();
			break;
		case '4': 
			break;
		case '5': 
			break;
		case '6': 
			break;
		case '7':
			break;
	default: printf("wrong number");
	}
	system("pause");
	return 0;
}