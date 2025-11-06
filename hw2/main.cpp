#include "Header.h"

int main()
{
	printf("1: RGB to Binary\n");
	printf("2: Morphology to reserve the road\n");
	printf("3: Connected Component\n");
	printf("4: Caculate property\n");
	printf("5: Bonus\n");
	printf("6: rotation estimation\n");
	printf("7: opencv\n");
	char num;
	printf("Enter the question number to select output result:");
	scanf("%c", &num);
	switch (num)
	{
	case '1': a();
		break;
	case '2': b();
		break;
	case '3': c();
		break;
	case '4': d();
		break;
	case '5': e();
		break;
	case '6': f();
		break;
	case '7': 
		a_opencv();
		b_opencv();
		c_opencv();
		d_opencv();
		f_opencv();
		break;
	default: printf("wrong number");
	}
}