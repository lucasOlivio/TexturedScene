#include "Application.h"

int main()
{
	Application gameApplication = Application();

	bool isInitialized = gameApplication.Initialize("scene");
	if (!isInitialized)
	{
		printf("Exiting with error");
		exit(EXIT_FAILURE);
	}

	gameApplication.Run();

	gameApplication.Exit();

	return 0;
}