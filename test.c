#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() 
{
	int health = 100;
	int money = 500;
	
	printf("health addr: %p\n", (void*)&health);
	printf("money addr:  %p\n", (void*)&money);
	
	while (1) {
		printf("\n[Health: %d] [Money: %d]\n", health, money);
		printf("1) Take damage (-10 hp)\n");
		printf("2) Spend money (-50)\n");
		printf("3) Show addresses\n");
		printf("> ");
		
		int choice;
		scanf("%d", &choice);
		
		switch (choice) {
			case 1:
				health -= 10;
				printf("Ouch! Health is now %d\n", health);
				break;
			case 2:
				money -= 50;
				printf("Spent! Money is now %d\n", money);
				break;
			case 3:
				printf("health addr: %p\n", (void*)&health);
				printf("money addr:  %p\n", (void*)&money);
				break;
		}
	}
	return 0;
}