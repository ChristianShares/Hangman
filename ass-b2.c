//-----------------------------------------------------------------------------
// ass-b2.c
//
// This program is a simple game known under the name of hangman. The user
// ... starts the game with a command-line command by adding the name of
// ... a text-file to the program-name ass-b2.c. The program will check if the
// ... file is suitable, which is the case when one word, consisting entirely
// ... of letters from the english alphabet, is in each line of the file, not
// ... seperated by blankets. If any of the above mentioned criteria is not
// ... applicable or if memory can't be allocated, a particular error will 
// ... occur and will either instruct the user to take further steps or will
// ... at least report the problem. At the start of each round, the player is
// ... shown the first letter of a word, of which he/she has to guess all 
// ... remaining letters. The amount of each missing letter is determinable
// ... by the extent of underline symbols following the first letter. If the
// ... user manages to guess the correct word after less than eleven tries,
// ... he/she win's the round and a new round will start automatically if there
// ... is still a word left in the text-file. Otherwise the game result gets
// ... revealed and the program stops.
//
// Group: 17, study assistant Oberhauser Clemens
//
// Authors: Christian Fusek 11827543
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//guess maximum
#define MAX_GUESS_COUNT 11

//function returns
#define RET_CANT_OPEN_FILE -2
#define RET_INVALID_WORD_FILE -3
#define RET_OUT_OF_MEMORY -4

typedef enum _Boolean_
{
  FALSE = 0,
  TRUE = 1
} Boolean;

typedef struct _Node_
{
  char *word_;
  uint32_t guesses_;
  struct _Node_ *next_;
} Node;

//-----------------------------------------------------------------------------
///
/// The function addWordToLinkedList creates nodes to the extend of processed
/// ... words. Each word gets assigned to the corresponding node.
///
/// @param head pointer to the first node of the LinkedList
/// @param STRING processed row from text-file
/// @param letter_location position of the first letter
/// @param number_of_letters amount of valid letters in the line
///
/// @return: int error message:  0: all worked well
/// @return:                    -4: unable to allocate memory
//


int addWordToLinkedList(Node **head, const char *STRING, uint32_t number_of_letters, 
  uint32_t letter_location)
{
  Node* new_node = malloc(sizeof(Node));
  Node* list_traverser = *head;
  char *new_word = (char *) malloc((number_of_letters + 1) * sizeof(char));
  if (new_word == NULL || new_node == NULL) 
  {
    return RET_OUT_OF_MEMORY;
  }

  uint32_t position_determiner = 0;
  uint32_t first_letter_index = letter_location;
  
  for (; letter_location < (first_letter_index + number_of_letters);
    letter_location++)
  {
    *(new_word + position_determiner) = *(STRING + letter_location);
    position_determiner++;
  }
  new_word[number_of_letters] = '\0';

  //assign value to new_node
  new_node->word_ = new_word;   

  //new node points to NULL as it's the last element
  new_node->next_ = NULL;     

  if (*head == NULL)
  {
    //when creating the list, head will be first node
    *head = new_node;        
    return 0;
  }

  while (list_traverser->next_ != NULL)
  {
    //moving to the end of list
    list_traverser = list_traverser->next_;  
  }
  //last node get's all the data from new node
  list_traverser->next_ = new_node;          
  return 0;
}

//-----------------------------------------------------------------------------
///
/// The function addGuessesToLinkedList implements the guesses for each word
/// ... in a LinkedList. 
///
/// @param node first node of the LinkedList
/// @param guess_amount amount of made guesses for each word
/// @param word_count to identify the affected node
///
/// @return function of type void, as consequence no return value
//


void addGuessesToLinkedList(Node *node, uint32_t guess_amount, 
  uint32_t *word_count)
{
  if (*word_count == 0)
  {
    node->guesses_ = guess_amount;
  }
  else
  {
    for (uint32_t node_number = 0; node_number < *word_count; node_number++)
    {
      node = node->next_;
    }
    node->guesses_ = guess_amount;    
  }
  (*word_count)++;
}

//-----------------------------------------------------------------------------
///
/// The function freeLinkedList will free dynamically allocated memory in
/// ... reference to the implementation of the LinkedList.
///
/// @param head first node position
///
/// @return function of type void, as consequence no return value
//

void freeLinkedList(Node *head) 
{
  if (head == NULL)
  {
    return;
  }

  Node* to_free = head;
  Node* next = head->next_;
  while (next) 
  {
    free(to_free->word_);
    free(to_free);
    to_free = next;
    next = to_free->next_;
  }

  free(to_free->word_);
  free(to_free);
}


//-----------------------------------------------------------------------------
///
/// The exchangeGuessedLetters function replaces underline symbols with the 
/// ... user input, if the seeked word includes the letter - irrespective of
/// ... its' case-sensitivity.
///
/// @param compare_string char-array, which serves as a reference to determine
///        ... if an user-input matches the seeked word.
/// @param display_string char_array, which prints out the first letter by
///        ... default and all guessed letters. Unidentified letters are
///        ... symbolized by underlines.
/// @param LETTER_COUNT_FOR_OTHER_FUNCTIONS amount of letters of each word
///
/// @return int value: made guesses for each word
//

int exchangeGuessedLetters(char *compare_string, char *display_string, 
  const uint32_t *LETTER_COUNT_FOR_OTHER_FUNCTIONS)
{
  char user_input;
  char input_uppercase;
  char input_lowercase;
  uint32_t guesses = 0;
  Boolean is_correct_guess_ = FALSE;

  while (strcmp(compare_string, display_string) != 0 && guesses != MAX_GUESS_COUNT)
  {  
    printf("%s (%d)\nYour guess: ", display_string, guesses);
    scanf(" %c", &user_input);

    input_uppercase = isupper(user_input) ? user_input : toupper(user_input);
    input_lowercase = isupper(user_input) ? tolower(user_input) : user_input;

    for (uint32_t position = 0; position < *LETTER_COUNT_FOR_OTHER_FUNCTIONS;
      position++)
    {
      if (input_uppercase == *(compare_string + position) || 
        input_lowercase == *(compare_string + position))
      {
        *(display_string + position) = *(compare_string + position);
        is_correct_guess_ = TRUE;
      }
    }
    if (!is_correct_guess_)
    {
      guesses++;
    }
    is_correct_guess_ = FALSE;
  }
  return guesses;
}

//-----------------------------------------------------------------------------
///
/// The function printWord initiates the print of all word-parts which should
/// ... be displayed to the user. It triggers the functions
/// ... exchangeGuessedLetters and addGuessesToLinkedList and informs about the
/// ... results of each round. 
///
/// @param head pointer to the first node of the LinkedList
/// @param letter_count_for_other_functions amount of letters of each word
/// @param node_position corresponding node, which holds the particular word
/// @param games_won amount of won games
/// @param games_total amount of played games
///
/// @return int error message:  0: all worked well
/// @return                    -4: unable to allocate memory
//


int printWord(Node *head, uint32_t *letter_count_for_other_functions, 
                uint32_t *node_position, uint32_t *games_won, 
                  uint32_t *games_total)
{
  //to identify each word in function addGuessToLinkedList
  uint32_t word_number = 0;

  (*node_position)++;

  if (*node_position != 1)
  {
    for (uint32_t node_counter = 0; node_counter < (*node_position - 1);
           node_counter++)
    {
      head = head->next_;
    }
  }
  char *compare_string = head->word_;
  // + 1 for \0 byte
  char *display_string = (char *) 
    malloc((*letter_count_for_other_functions + 1) * sizeof(char));

  if (display_string == NULL)
  {
    return RET_OUT_OF_MEMORY;
  }

  *display_string = *compare_string;

  for (uint32_t make_underlined_counter = 1; 
    make_underlined_counter < *letter_count_for_other_functions ;
      make_underlined_counter++)
  {
    display_string[make_underlined_counter] = '_';
  }

  compare_string[*letter_count_for_other_functions] = '\0';
  display_string[*letter_count_for_other_functions] = '\0';

  uint32_t guesses_for_each_word = exchangeGuessedLetters(compare_string,
    display_string, letter_count_for_other_functions);

  if (guesses_for_each_word < MAX_GUESS_COUNT)
  {
    printf("%s (%d)\n", display_string, guesses_for_each_word);
    (*games_won)++;
  }
  else
  {
    printf("%s (x_x)\n", compare_string);
  }
  (*games_total)++;

  addGuessesToLinkedList(head, guesses_for_each_word, &word_number);
  free(display_string);
  return 0;
}

//-----------------------------------------------------------------------------
/// 
/// The function getFileValidity examines the validity of the text document.  
/// ... The file is valid, when there is only one word in each row. The
/// ... word solely contains letters from the English alphabet and is not
/// ... seperated by blankets. Blankets before and after the word are no 
/// ... restrictions.
///
/// @param ROW pointer to the first letter of each processed row
/// @param letter_count_for_other_functions counts the amount of letters of
///        ... each line, if the text-file is valid
/// @param first_letter_position_for_other_functions saves the position of
///        ... the first letter for each transferred line
///
/// @return int error message:  0: all worked well
/// @return                    -3: word-file is invalid
//

int getFileValidity(const char *ROW, uint32_t *letter_count_for_other_functions, 
  uint32_t *first_letter_position_for_other_functions)
{
  uint32_t letter_count = 0;
  uint32_t char_count = 0;
  Boolean is_uppercase_;
  Boolean is_lowercase_;
  uint32_t first_letter_position = 0;
  uint32_t end_of_word = 0;
  Boolean is_valid_char_;

  while(*(ROW + char_count) != '\0' && *(ROW + char_count) != EOF)
  {
    is_uppercase_ = isupper(*(ROW + char_count));
    is_lowercase_ = islower(*(ROW + char_count));

    if (is_lowercase_ || is_uppercase_)
    {
      letter_count++;
      if (letter_count == 1)
      {
        first_letter_position = char_count;
      }
    }
  char_count++;
  }
  end_of_word = first_letter_position + letter_count;

  for (uint32_t letter_position = first_letter_position; 
    (letter_position < end_of_word); letter_position++)
  { 
    char c  = *(ROW + letter_position);
    is_valid_char_ = isupper(c) || islower(c);

    if (c == ' ' || !is_valid_char_)  
    {
      return RET_INVALID_WORD_FILE;
    }
  }
  *letter_count_for_other_functions = letter_count;
  *first_letter_position_for_other_functions = first_letter_position;
  return 0;
}

//-----------------------------------------------------------------------------
///
/// The function readWordList opens the text-file cited in the command-line
/// ... command and triggers the functions getFileValidity, addWordToLinkedList
/// ... and printWord. Once these functions are executed, it either returns
/// ... int error messages or prints the game results.
///
/// @param filename the respective word-file, which should be read
///
/// @return int error message:  0: all worked well
/// @return                    -2: unable to open text-file
/// @return                    -3: word-file is invalid
/// @return                    -4: unable to allocate memory
//


int readWordList(char *filename)
{
  //communication between the file and the program (pointer of type file)
  FILE *word_list = fopen(filename, "r");

  Node *stack = NULL;
  uint32_t first_letter_position;
  uint32_t letter_amount;

  //initialization to know the respective node in function printWord
  uint32_t node_position = 0;
  //initialization to be able to count the number of won games in function printWord
  uint32_t games_won = 0;
  //initialization to be able to count the number of all played games in function printWord
  uint32_t games_total = 0;

  if (word_list == NULL)
  {
    return RET_CANT_OPEN_FILE;
  }

  int32_t character_amount = 0;

  while (getc(word_list) != EOF)
  {
    character_amount++;
  }

  char *line = (char *) malloc((character_amount) * sizeof(char));

  if (line == NULL)
  {
    return RET_OUT_OF_MEMORY;
  }

  // resets file pointer to start (reread file)
  rewind(word_list); 
  
  while (fgets(line, character_amount, word_list))
  {
    if (getFileValidity(line, &letter_amount, 
      &first_letter_position) == RET_INVALID_WORD_FILE)
    {
      free(line);
      return RET_INVALID_WORD_FILE;
    }
    else
    {
      if (addWordToLinkedList(&stack, line, letter_amount, 
        first_letter_position) == RET_OUT_OF_MEMORY)
      {
        free(line);
        return RET_OUT_OF_MEMORY;
      }
      if (printWord(stack, &letter_amount, &node_position, 
        &games_won, &games_total) == RET_OUT_OF_MEMORY)
      {
        free(line);
        return RET_OUT_OF_MEMORY;
      }
    }
  }
  printf("won (%d/%d)", games_won, games_total);
  free(line);
  freeLinkedList(stack);
  fclose(word_list);
  return 0;
}

//-----------------------------------------------------------------------------
///
/// The main function calls the function readWordList, if the program is
/// ... executed correctly. The program is executed correctly, when there are
/// ... only two command line values: the program name and the text file.
/// ... Depending on the return value of the function readWordList, either an
/// ... error gets reported in a controlled manner or the program will be 
/// ... further executed.
///
/// @param argc command-line command argument count
/// @param argv command-line command argument values
///
/// @return int error message:  0: all worked well
/// @return                     1: wrong command-line command
/// @return                     2: unable to open text-file
/// @return                     3: word-file is invalid
/// @return                     4: unable to allocate memory
//

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("usage: %s filename\n", argv[0]);
    return 1;
  }
  //holds the return value of function readWordList
  int32_t function_return = readWordList(argv[1]);

  switch(function_return)
  {
    case RET_CANT_OPEN_FILE:
      printf("ERROR: cannot open file %s \n", argv[1]);
      return 2;
      case RET_INVALID_WORD_FILE:
      printf("ERROR: file %s invalid\n", argv[1]);
      return 3;
    case RET_OUT_OF_MEMORY:
      printf("ERROR: Out of Memory\n");
      return 4;
    default:
      return 0;
  }
}