#include "Header.h"

int main()
{
	printf("1: IPM\n");
	printf("2: line detecter\n");
	printf("3: bonus\n");
	printf("4: IPM opencv\n");
	printf("5: line detecter opencv\n");
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
			RunHoughLaneDetection();
			break;
		case '3':
			bonus_run();
			break;
		case '4': 
			ipm_cv();
			break;
		case '5': 
			RunHoughLaneDetection_OpenCV();
			break;
		case '6': 
			runImageStitchingOpenCV();
			break;
		case '7':
			runAutoStitch();
			break;
	default: printf("wrong number");
	}
	system("pause");
	return 0;
}