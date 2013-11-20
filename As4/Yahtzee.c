#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structs:
typedef struct die_node{
 	struct die_node* next; // Next node
	short reroll_bool;     // 0=False, !0=True
	short die_number;      // 1-5 
	short die_value;       // 1-6
} die_node; 

//Prototypes:
void play_game();
void init_nodes();
void roll_dice();
void print_dice();
short get_rand_die();
void additional_rolls();
void reset_dice();
void score_upper();
void score_lower();
void print_total_score();
void print_scoreboard();
int comp (const void *, const void *);
int sl_kind(int, int*);
int sl_straight(int, int*);


//Globals:
die_node* first_node;
int upper_section[6];
int upper_bonus;
int lower_section[7];


//Main:
int main(){

	//Create and Initialize Linked List of Dice:
 	first_node = malloc(sizeof(die_node) * 5);
 	init_nodes(first_node); //initialize linked list

 	//Start the fun!
	play_game();

	return 0;
}

/*
 *  Game Loop
 */
 void play_game(){

	//Initializations:
 	int total_score = 0;
 	int current_turn = 1;
 	int i, j;
 	int selection;
 	die_node* node;

	//Game Loop:
 	while(current_turn <= 13){

 		printf("\nYour first roll:\n");
		roll_dice(); //Get dice roll
		print_dice(); //Print dice roll
 		
 		additional_rolls(); //Rerolls 2 more times if User chooses to do so
		
		// * * *  Dice are now finalized  * * * //

 		//Let the user decide how to score:
 		printf("Place dice into section:\n");
 		printf("1) Upper Section\n2) Lower Section\n");
 		printf("Selection? ");
 		scanf("%d", &selection); //get user input

 		printf("\nPlace dice into: \n");
 		if(selection == 1){//Upper section
 			printf("1) Ones\n2) Twos\n3) Threes\n4) Fours\n5) Fives\n6) Sixes\n");
 			printf("Selection? ");
			scanf("%d", &selection);
			score_upper(selection); //Handle Scoreboard
 		} else if (selection == 2) { //Lower Section
 			printf("1) Three of a kind\n2) Four of a kind\n3) Small straight\n4) Large straight\n5) Full house\n6) Yahtzee\n7) Chance\n");
			printf("Selection? ");
			scanf("%d", &selection);
			score_lower(selection);
 		}

		print_total_score(); //Print score
		print_scoreboard(); //Display scoreboard (upper and lower sections)

		//Loop 13 times//
		current_turn++;
		//Reset all dice (Need rerolled!)
		reset_dice();
	}

}

/*
 *  Prints scoreboard 
 */
void print_scoreboard(){

	printf("\nOnes: %d \t\t\tFours: %d\nTwos: %d\t\t\t\tFives: %d\nThrees: %d\t\t\tSixes: %d\n", upper_section[0], upper_section[3], upper_section[1], upper_section[4], upper_section[2], upper_section[5]);
	printf("Upper Section Bonus: %d\n", upper_bonus);
 	printf("Three of a kind: %d\t\tFour of a kind: %d\nSmall straight: %d\t\tLarge straight: %d\nFull house: %d\t\t\tYahtzee: %d\nChance: %d\n", lower_section[0], lower_section[1], lower_section[2], lower_section[3], lower_section[4], lower_section[5], lower_section[6]);

}

/*
 *  Prints total score
 */
void print_total_score(){
	int i;
	int total = 0;

	//Sum Upper section scores
	for(i = 0; i < 6; i++){ //6 is max in upper section
		total += upper_section[i];
	}

	//Bonus for upper section
	if(total >= 63){
		upper_bonus = 35;
		total += upper_bonus;
	}

	//Sum Lower section scores
	for(i = 0; i < 7; i++){ //7 is max in lower section
		total += lower_section[i];
	}

	printf("\nYour score so far is: %d\n", total);
}

/*
 *  (s)core (l)ower / 3,4,5 of a kind 
 *	Returns BOOLEAN -- 1 if _ of a kind exists, 0 if it does not
 */
int sl_kind(int kind, int *values){
	int run, i, j;

	for (j = 1; j <= 6; j++){ //cycle all possible dice values
		run = 0;
		for (i = 0; i < 5; i++){ //cycle all dice
			if (values[i] == j){
				run++;
			}
		}
		if (run >= kind){
			return 1; //TRUE
		}
	}

	return 0; //FALSE
}

/*
 * Returns BOOLEAN if small/large straight exists
 */
int sl_straight(int target_length, int *values){
	int run, start, i, j, k, n;

	for(i=0;i<2;i++){ //cycle first two dice in sorted array (run of 4 or 5 only!)
		start = values[i];
		k = i; 
		for (j=start; j<start+target_length; j++){ //try to parse a straight
			if (j != values[k++]){
				printf("Lost it!!!!!!");//DEBUG
				return 0; //FALSE
			}
			run++;
		}
		if (run == target_length){
			return 1; //TRUE
		}
	}

	return 0;//FALSE

} 

/*
 *  Calculates score for LOWER section
 */
void score_lower(int selection){
	int index = selection-1;
	int total = 0;
	die_node* node = first_node;
	int values[5];
	int i;

	//Sort the dice:
	i = 0;
	while(node != NULL){
		values[i++] = node->die_value; //store dice into array to be sorted
		node = node->next;
	}
	qsort (values, 5, sizeof(int), comp); //sort array

	// * * *  Dice are now sorted in values[]  * * *

	//Determine score:
	switch(selection){
		case 1: //Three of a kind: if good, +total of all dice
			if(sl_kind(3, values)){ //if good
				node = first_node;
				while(node != NULL){ //+total all dice
					total += node->die_value;
					node = node->next;
				}
			}
		break;

		case 2: //Four of a kind: if good, +total of all dice
			if(sl_kind(4, values)){
				node = first_node;
				while(node != NULL){ //+total all dice
					total += node->die_value;
					node = node->next;
				}
			}
		break;

		case 3: //Small Straight: if good, +30
			if(sl_straight(4, values)){ //if four in a row
				total = 30;
			}
		break;

		case 4: //Large Straight: if good, +40
			if(sl_straight(5, values)){ //if five in a row
				total = 40;
			}
		break;

		case 5: //Full House: if good, +25
			total = 25;
		break;

		case 6: //Yahtzee!: if good, +50
			if(sl_kind(5, values)){
				node = first_node;
				while(node != NULL){ //+total all dice
					total += node->die_value;
					node = node->next;
				}
			}
		break;

		case 7: //Chance: +Total of all dice
			node = first_node;
			while(node != NULL){ //+total all dice
				total += node->die_value;
				node = node->next;
			}
		break;

		//Do nothing
	}

	lower_section[index] = total; //Store score
}

/*
 *  Calculates and stores score for the UPPER section
 */ 
void score_upper(int selection){
	int index = selection-1;
	int quantity;
	die_node* node = first_node;
	
	//Get quantity:
	while(node != NULL){
		if(node->die_value == selection){
			quantity++;
		}
		node = node->next;
	}

	//Calculate and store score
	upper_section[index] = selection * quantity; //Example: User has three 1's:  upper_section[0] = 1 * 3;
}

/*
 *  Resets ALL dice for rerolling
 */
void reset_dice(){
	die_node* node = first_node;

	while (node != NULL){
		node->reroll_bool = 1; //True, must reroll!
		node = node->next;
	}


}

/*
 *  Handles the logic for roll 2 and 3 if the user chooses to reroll any dice
 */
void additional_rolls(){
	int j, i, selection;
	int reroll_input[6];
	die_node* node;

 		for(j=0; j<2; j++){
 			printf("Which dice to reroll? [Enter one at a time] [0 when done]\n");
 			//Parse input
 				i = 0;
 				do{
 					scanf("%d", &selection);
 					reroll_input[i++] = selection;
 				} while (selection != 0);

 			//Check if user entered 0 as the first input
 			if(reroll_input[0] == 0){ //user does not want to reroll any dice
 				break; //BREAK FOR Loop
 			}

			//If input == 0, move on, else roll again//
 			i = 0; 
 			do {
 				selection = reroll_input[i++];
	
				//parse thru linked list of die
 				if (selection != 0){
					node = first_node;//init before while
					while (node != NULL){
						if (node->die_number == selection){
							node->reroll_bool = 1; //True, need to reroll!
						}
						node = node->next;
					}
				}
			} while (selection != 0);

			//Reroll dice:
			printf("Your %s roll:\n", (j==1)?"final":"second");
			roll_dice(); //Get dice roll
			print_dice(); //Print dice roll

		} //End Rerollings
}

/*
 *  Returns one random die roll
 */
short get_rand_die(){
	return ((rand() % 6) + 1);
}

/*
 *  Prints the dice values
 */
void print_dice(){
	die_node* node = first_node;
	
	while (node != NULL){
		printf(" %d ", node->die_value);
		node = node->next;
	}
	printf("\n\n");

}

/*
 *  Fills the linked list with dice rolls
 */
void roll_dice(){
	die_node* node = first_node;

	while (node != NULL){
		if(node->reroll_bool != 0){ //!0 = True, so reroll it!
			node->die_value = get_rand_die();
			node->reroll_bool = 0; //False, do not reroll
		}
		node = node->next;
	}

}

/*
 *  Initializes a linked-list of 5 dice
 */
void init_nodes(){
	die_node* node = first_node;
	short die_number;
	void* next;

	for (die_number = 1; die_number<=5; die_number++ ){
		next = node+sizeof(die_node);
		node->next = next;
		node->reroll_bool = 1;
		node->die_number = die_number;
		node->die_value = 0;
		node = next;
	}
	node = node-sizeof(die_node);
	node->next = NULL;

}

/*
 *  Comparison function for qsort
 *  Copied from User: RERUN on stackOverflow:
 *  http://stackoverflow.com/questions/1787996/c-library-function-to-do-sort
 */ 
int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}