/*
Author: Victor Uriel Robles Mendoza
Start Date: 04-18-2025
End Date: 05-09-2025

Program information: This program tries to very poorly mimicate the UNO gameplay and it fails to do so in many ways.
The program ask the user for the amount of players (there is a cap), the program shuffles a deck and distributes it
between the players given 7 cards to each player. The main game loop consist of if conditions with most possible cases
and answers to them by using the functions given. Such as what happens if the user uses a special card, but does not
pair it, etc. Once the deck is empty or a player has empty his hand completely the user with the fewest cards wins.
If the deck ends and two people have the same amount of cards, then the first player is detected as a winner. When
a winner is detected the user is prompted with the question to play again, if they want to play again, pretty much
the whole game restarts including names and all. If no, then the program ends.

Comments: I originally wanted to add more UNO like features to this game, such as the ability to challenge special
draw 4 cards, saying uno before the last card or you will have to draw some cards, etc. I ended up not implimenting
most of them since, well this project is already a few days late and I would not want being more late. I did implement
the draw a card whenever you feel like it and is your turn.

Disclaimer: I am working by myself.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>
#include <stdbool.h>
#include <tgmath.h>
#include <ctype.h>

#define DECK_SIZE 100
#define MAX_PLAYERS 14
#define HAND_SIZE 100 // hypotetical maximum amount of cards a player can have

typedef struct card_t {
    char name; // '0'–'9' for number cards, 'A' for AND, 'O' for OR, 'N' for NOT, 'R' for Reverse
    char color; // 'R' for Red, 'Y' for Yellow, 'G' for Green, 'B' for Blue, 'S' for special cards
} card;

/* Extra Credit -- Optional -- Variation of Struct Above
typedef struct card_s {  
    char name; //’0’-‘9’ for number cards, ‘A’ for AND, ‘O’ for OR, ‘N’ 
    for NOT, and ‘R’ for Reverse. 
    char color; //’R’ for red, ‘Y’ for yellow, ‘G’ for green, ‘B’ for blue, 
    and ‘S’ for special cards 
    struct card_s *listp;  
    } card;
*/

typedef struct player_t {
    char playerName[128];
    card hand[HAND_SIZE]; // dynamic array (or linked list) of cards 
    //Change it to hand, because I had another thing named deck and it would have been confusing.
    int decksize; // how many cards the player currently holds
} player;

// --- Function prototypes ---

// BASIC DECK AND HAND CREATION

// Creates a standard deck of 100 cards (two sets of 0-9 for each color, plus the special cards)
void initializeDeck(card deck[], int deckSize);
// Shuffle deck
void shuffleDeck(card deck[]);
// Draw a single card from the deck and place it in a player's hand. Return 1 if the draw was successful, 0 if deck is empty.
int drawCard(card deck[], int deckSize, player *p, int *deckIndex);

// UNO GAMEPLAY RULES

// Check if a candidate card is valid to play on the top-of-pile card. Return 1 if valid, 0 otherwise.
int isValidCard(card topOfPile, card candidate);
// Handle effect of AND card. This could check the next player’s hand for the correct color AND number (return 1), or apply a penalty if
//they cannot match (return 0)
int handleAND(player currentPlayer, card topOfPile);
// Handle effect of OR card. Similar logic, except the next player must match color OR number or face the penalty.
int handleOR(player currentPlayer, card topOfPile);
// Handle effect of NOT card (aka SKIP card).
int handleNOT(int currentPlayerIndex, int playerCount, int direction);
// Handle effect of Reverse card (reverse the turn order or in a two-player game, let current player go again
void handleReverse(int *direction, int playerCount);

// DISPLAY CARDS

// Print a single card (e.g., "Red 5", "Blue 0", "AND", "Reverse", etc.).
void printCard(card c);
// Print the hand of a given player (their name + the cards).
void printPlayerHand(player p);


int main(void) {
    srand((int)time(0)); //from zybooks
    char TestPlayerAmount[36], TestPlayerNames[MAX_PLAYERS][128], playerNames[24][128];
    int playersAmount = 0, result = 0, direction = 1, matchRun = 1, deckIndex = 0, winner = -1;
    player players[MAX_PLAYERS];
    card deck[DECK_SIZE];

    // Get number of players (2min - 14max)
    while ((result != 1) || (playersAmount < 2) || (playersAmount > 14)) {
        printf("Enter number of players: "); // 14 players maximum
        fgets(TestPlayerAmount, sizeof(TestPlayerAmount), stdin);
        result = sscanf(TestPlayerAmount, "%d", &playersAmount);
        printf("\n");
    }
    
    // To match the spelling of number on the sample output.
    const char *ordinals[] = { "first", "second", "third", "fourth", "fifth", "sixth", "seventh",
        "eighth", "ninth", "tenth", "eleventh", "twelfth", "thirteenth", "fourteenth"
    };

    // Get player names
    for (int i = 0; i < playersAmount; i++){
        printf("Enter %s player's name: ", ordinals[i]);
        fgets(TestPlayerNames[i], sizeof(TestPlayerNames[i]), stdin);
        TestPlayerNames[i][strcspn(TestPlayerNames[i], "\n")] = '\0'; // Remove newline
        strcpy(players[i].playerName, TestPlayerNames[i]);
        players[i].decksize = 0;
        printf("\n");
    }
        
    //Game loop
    char playAgain = 'Y';
    while (playAgain == 'Y' || playAgain == 'y') {
        // Setup deck and deal
        initializeDeck(deck, DECK_SIZE);
        shuffleDeck(deck);
        deckIndex = 0;
       for (int i = 0; i < playersAmount; i++) {
            players[i].decksize = 0;
            for (int j = 0; j < 7; j++) {
                drawCard(deck, DECK_SIZE, &players[i], &deckIndex);
            }
        }
        
        //Show hands
        for (int i = 0; i < playersAmount; i++) {
            printPlayerHand(players[i]);
            printf("\n");
        }
        
        // Start pile
        card pile[DECK_SIZE];
        int pileTop = 0;
        pile[pileTop].name = 0;
        pile[pileTop].color = 0;

        int currentPlayer = 0;
        direction = 1;
        int gameOver = 0;

        // Draw first card to start pile (must be a number card)
        do {
            pile[0] = deck[deckIndex];
            deckIndex++;
        } while (pile[0].color == 'S');
        pileTop = 0;

        printf("Top of card pile is: ");
        printCard(pile[pileTop]);
        printf("\n");

        //Main play loop
        while (!gameOver) {
            player *p = &players[currentPlayer];
            printf("%s, enter which card to play from 0 to %d (or 'd' to draw): ", p->playerName, p->decksize - 1);
            int choice = -1;
            char input[16];
            fgets(input, sizeof(input), stdin);

            // Check if player wants to draw a card
            if (input[0] == 'd' || input[0] == 'D') {
                if (drawCard(deck, DECK_SIZE, p, &deckIndex)) {
                    printf("%s draws a card.\n", p->playerName);
                    printPlayerHand(*p); // Show the updated hand
                    printf("\n");

                } if (deckIndex >= DECK_SIZE) {
                    printf("Deck is empty.\n");
                    // Find player with fewest cards
                    int minCards = players[0].decksize;
                    int minIndex = 0;
                    for (int i = 1; i < playersAmount; i++) {
                        if (players[i].decksize < minCards) {
                            minCards = players[i].decksize;
                            minIndex = i;
                        }
                    }
                    printf("%s wins with the fewest cards!\n", players[minIndex].playerName);
                    gameOver = 1;
                    break;
} {
                    printf("Deck is empty, cannot draw.\n");
                }
                // Skip turn after drawing
                currentPlayer = (currentPlayer + direction + playersAmount) % playersAmount;
                continue;
            }

            // Otherwise, treat input as card index
            sscanf(input, "%d", &choice);
            if (choice < 0 || choice >= p->decksize) {
                printf("Invalid choice, %s does not have %d cards\n", p->playerName, choice + 1);
                continue;
            }
            card chosen = p->hand[choice];

            // Check if valid card
            if (!isValidCard(pile[pileTop], chosen)) {
                printf("Invalid choice, cannot place ");
                printCard(chosen);
                printf(" in ");
                printCard(pile[pileTop]);
                printf("\n");
                continue;
            }

            // Handle special cards
            int skipNext = 0;
            int drawPenalty = 0;
            int nextPlayer = (currentPlayer + direction + playersAmount) % playersAmount;

            if (chosen.color == 'S') {
                if (chosen.name == 'A' || chosen.name == 'O') {
                    //Must be pair with a number card
                    int pairChoice = -1;
                    int validPair = 0;
                    if (chosen.name == 'A') {
                        printf("Choose the card to play with AND from ");
                    } else {
                        printf("Choose the card to play with OR from ");
                    }
                    for (int i = 0; i < p->decksize; i++) {
                        if (p->hand[i].color != 'S' && i != choice) {
                            printf("%d ", i);
                        }
                    }
                    printf(": ");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%d", &pairChoice);
                    if (pairChoice < 0 || pairChoice >= p->decksize || pairChoice == choice || p->hand[pairChoice].color == 'S') {
                        printf("Cannot play two special cards in one turn\n");
                        continue;
                    }
                    card paired = p->hand[pairChoice];
                    // Remove both cards from hand
                    if (pairChoice > choice) {
                        // Remove higher index first
                        for (int i = pairChoice; i < p->decksize - 1; i++)
                            p->hand[i] = p->hand[i + 1];
                        p->decksize--;
                        for (int i = choice; i < p->decksize - 1; i++)
                            p->hand[i] = p->hand[i + 1];
                        p->decksize--;
                    } else {
                        for (int i = choice; i < p->decksize - 1; i++)
                            p->hand[i] = p->hand[i + 1];
                        p->decksize--;
                        for (int i = pairChoice; i < p->decksize - 1; i++)
                            p->hand[i] = p->hand[i + 1];
                        p->decksize--;
                    }
                    pileTop++;
                    pile[pileTop] = chosen; // Place the special card (AND/OR) on the pile
                    pileTop++;
                    pile[pileTop] = paired;

                    printf("Top of card pile is: ");
                    printCard(paired);
                    if (chosen.name == 'A') {
                        printf(" with AND\n");
                    } else {
                        printf(" with OR\n");
                    }
                    
                    int hasMatch = (chosen.name == 'A')
                    ? handleAND(players[nextPlayer], paired)
                    : handleOR(players[nextPlayer], paired);
                    
                    
                    if (!hasMatch) {
                        if (chosen.name == 'A') {
                            printf("%s has no card that matches color and number\n", players[nextPlayer].playerName);
                            printf("AND penalty, Draw 4\n");
                        } else {
                            printf("%s has no card that matches color or number\n", players[nextPlayer].playerName);
                            printf("OR penalty, Draw 4\n");
                        }
                        for (int i = 0; i < 4; i++) {
                            drawCard(deck, DECK_SIZE, &players[nextPlayer], &deckIndex);
                        }
                    } else {
                        printf("Card Matches, no %s penalty\n", (chosen.name == 'A') ? "AND" : "OR");
                    }
                    currentPlayer = nextPlayer;
                    continue;
                } else if (chosen.name == 'N') {
                    // NOT card: skip next player
                    printf("Next Player’s turn skipped.\n");
                    skipNext = 1;
                } else if (chosen.name == 'R') {
                    // Reverse card
                    handleReverse(&direction, playersAmount);
                    printf("Order reversed.\n");
                }
            }
            // Remove played card from hand
            for (int i = choice; i < p->decksize - 1; i++)
                p->hand[i] = p->hand[i + 1];
            p->decksize--;

            // Place card on pile
            pile[pileTop] = chosen;
            ++pileTop;

            // Check for win
            if (p->decksize == 0) {
                printf("%s wins!\n", p->playerName);
                gameOver = 1;
                winner = currentPlayer;
                break;
            }

            // Deck exhausted?
            if (deckIndex >= DECK_SIZE) {
                printf("Deck is empty.\n");
                // Find player with fewest cards
                int minCards = players[0].decksize;
                int minIndex = 0;
                for (int i = 1; i < playersAmount; i++) {
                    if (players[i].decksize < minCards) {
                        minCards = players[i].decksize;
                        minIndex = i;
                    }
                }
                printf("%s wins with the fewest cards!\n", players[minIndex].playerName);
                gameOver = 1;
                winner = minIndex;
                break;
            }

            // Show hands and pile
            for (int i = 0; i < playersAmount; i++) {
                printPlayerHand(players[i]);
                printf("\n");
            }
            printf("Top of card pile is: ");
            printCard(pile[pileTop]);
            printf("\n");

            // Advance to next player
            if (chosen.color == 'S' && chosen.name == 'N') {
                // NOT card: skip next player
                currentPlayer = (currentPlayer + 2 * direction + playersAmount) % playersAmount;
            } else {
                currentPlayer = (currentPlayer + direction + playersAmount) % playersAmount;
            }
        }

        // Play again?
        printf("Play again? (Y/N): ");
        fgets(TestPlayerAmount, sizeof(TestPlayerAmount), stdin);
        sscanf(TestPlayerAmount, " %c", &playAgain);
    }

    printf("Bye now, Adios\n");
    return 0;
}

// --- Function definitions ---
void initializeDeck(card deck[], int deckSize) {
    int index = 0;
    char colors[] = {'R', 'Y', 'G', 'B'};
    // Two sets of 0-9 for each color
    for (int c = 0; c < 4; c++) { // c as in color
        for (int n = 0; n <= 9; n++) { // n as in number. Altough is obvious
            for (int set = 0; set < 2; set++) { // two sets of each
                deck[index].name = '0' + n; //changes the value of ASCII to the correct one
                deck[index].color = colors[c];
                index++;
            }
        }
    }

    // 5 AND, 5 OR, 5 NOT, 5 Reverse. The special cards.
    for (int t = 0; t < 4; t++) { // t as in type
        for (int i = 0; i < 5; i++) {
            if (t == 0) {
                deck[index].name = 'A';
                deck[index].color = 'S';
                index++;
            } else if (t == 1) {
                deck[index].name = 'O';
                deck[index].color = 'S';
                index++;
            } else if (t == 2) {
                deck[index].name = 'N';
                deck[index].color = 'S';
                index++;
            } else if (t == 3) {
                deck[index].name = 'R';
                deck[index].color = 'S';
                index++;
            }
        }
    }
}

void shuffleDeck(card deck[]) {
    int shuffleTimes = rand() + 10000; // random number of shuffles, at least 10,000
    for (int i = 0; i <= shuffleTimes; i++) {
        int index1 = (rand() % (DECK_SIZE + 1));
        int index2 = (rand() % (DECK_SIZE + 1));
        card temp = deck[index1];
        deck[index1] = deck[index2];
        deck[index2] = temp;
    }
}

int drawCard(card deck[], int deckSize, player *p, int *deckIndex) {
    if (*deckIndex >= deckSize) {
        // Deck is empty
        return 0;
    }
    p->hand[p->decksize] = deck[*deckIndex];
    p->decksize++;
    (*deckIndex)++;
    return 1;
}


int isValidCard(card topOfPile, card candidate) {
    // Special cards can always be played
    if (candidate.color == 'S') {
        return 1;
    }
    // If top card is special, allow any card since they do not have a number or specific color
    if (topOfPile.color == 'S') {
        return 1;
    }
    // match color or number
    if (candidate.color == topOfPile.color || candidate.name == topOfPile.name) {
        return 1;
    }
    return 0;
}


int handleAND(player currentPlayer, card topOfPile) {
    for (int i = 0; i < currentPlayer.decksize; i++) {
        if (currentPlayer.hand[i].color == topOfPile.color && currentPlayer.hand[i].name == topOfPile.name) {
            return 1;
        }
    }
    return 0;
}

int handleOR(player currentPlayer, card topOfPile) {
    for (int i = 0; i < currentPlayer.decksize; i++) {
        if (currentPlayer.hand[i].color == topOfPile.color || currentPlayer.hand[i].name == topOfPile.name) {
            return 1;
        }
    }
    return 0;
}

int handleNOT(int currentPlayerIndex, int playerCount, int direction) {
    if (playerCount == 2) {

        return currentPlayerIndex;
    } else if (playerCount > 2) {
        if (direction == 1) {
            return ((currentPlayerIndex - 2 + playerCount) % playerCount); // skips a player going in reverse hence the -2
            //add the playerCount to prevent negatives
        } else if (direction == 0) {
            return ((currentPlayerIndex + 2) % playerCount); // skips a player going in the normal direction hence the +2
        }
    }
}

void handleReverse(int *direction, int playerCount) {
    if (playerCount > 2) {
        *direction *= -1;
    }
}

void printCard(card c) {
    if (c.color == 'S'){
        if (c.name == 'A') {
            printf("AND");
        } else if (c.name == 'O') {
            printf("OR");
        } else if (c.name == 'N') {
            printf("NOT");
        } else if (c.name == 'R') {
            printf("Reverse");
        }
    } else {
        if (c.color == 'R'){
            printf("Red %c",c.name);
        } else if (c.color == 'Y') {
            printf("Yellow %c",c.name);
        } else if (c.color == 'G') {
            printf("Green %c",c.name);
        } else if (c.color == 'B') {
            printf("Blue %c",c.name);
        }
    }
}

void printPlayerHand(player p) {
    printf("%s's hand:\n", p.playerName);
    for (int i = 0; i < p.decksize; i++) {
        printCard(p.hand[i]);
        printf("\n");
    }

}
