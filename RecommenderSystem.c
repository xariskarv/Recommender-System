#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//The maximum storage for buffer.
#define MaxLine 1450

//A structure which contains the elements of the field "Data" of the linked list.
typedef struct ListElementType{
    int highest_rating;
    char *highest_isbn;
}ListElementType;

/*A linked list data structure which will be used to store the best values
  (highest ratings) for a specific user.*/
typedef struct ListNode *ListPointer;
typedef struct ListNode{
	ListElementType Data;
    ListPointer Next;
}ListNode;

/*A structure for the user. This structure is used to store all the data
  needed from the file "BX-Users.csv" which contains the users' information. */
typedef struct {
    char *user_id;
    char *location;
    char *age;
}UserType;

/*A structure for the book. This structure is used to store all the data
  needed from the file "BX-Books.csv" which contains the books' information. */
typedef struct {
    char *isbn;
    char *book_Title;
    char *book_Author;
    char *year_of_publication;
    char *publisher;
    char *image_url_s;
    char *image_url_m;
    char *image_url_l;
}BookType;

/*A structure for the keyword. This structure is used to store all the data
   needed from the file "ISBN Key Words.csv" which contains up to 2 keywords for every book. */
typedef struct {
    char *isbn;
    char *first_keyword;
    char *second_keyword;
}KeywordType;

/*A structure for the rating. This structure is used to store all the data
   needed from the file "BX-Book-Ratings.csv" which contains all the ratings the users have
   done to every book they have read. */
typedef struct {
    char *user_id;
    char *isbn;
    char *rating;
}RatingType;

/*A structure for the user's preferences based on their preference profile. This structure contains all the
  information (top-3 criticized books, user's id, keywords and books' authors) for every one of the 5 random users.*/
typedef struct {
    char *top_criticized_books;
    char *user_id;
    char *keywords;
    char *book_authors;
    char *year_of_publication;
}PreferenceType;

typedef enum {
    FALSE = 0,
    TRUE = 1
}boolean;

//Linked List basic functions(insertion,deletion,traversal).
void createList(ListPointer *List);
boolean emptyList(ListPointer List);
void linkedInsert(ListPointer *List,int rating,char *isbn, ListPointer PredPtr);
void linkedDelete(ListPointer *List, ListPointer PredPtr);
void linkedTraverse(ListPointer List);

//Linked list sorting functions
/*Insertion sort will be used (The number of elements which will be sorted is small <10)
  so insertion sort will not delay the performance of the program. */
void insertionsort(ListPointer *List, ListPointer *Sorted);
void sortedInsert(ListPointer newPtr, ListPointer *Sorted);

//General functions
void findTopThreeRatings(ListPointer *topRatings, int input_value, char *currentISBN, ListPointer PredPtr);
void getRandomUser(UserType *aUser);
void searchUserRatings(UserType aUser, ListPointer *topRatings, FILE *outfile);
void getISBNkeywords(ListPointer topRatings,FILE *outfile);
void getBookAuthor(ListPointer topRatings,FILE *outfile);
void createRecommenderFile();
void readUserPreferenceProfile(PreferenceType *aPreferenceProfile,int *number_of_profile);
void calculateJaccardAndDiceValues(PreferenceType aPreferenceProfile,double *union_of_keywords, double *union_of_authors,FILE *outfile);
void printInfo();
int printRandoms(int lower, int upper);

int main()
{
    int i;
    FILE *outfile;
    ListPointer topRatings;  //A list pinter which points to the first element of the linked list.(The list will store the top 3 highest ratings).
    ListPointer currPtr;     //A list pointer that points the current node of the linked list.
    UserType aUser;

    outfile = fopen("Preference Profile.csv", "a"); //The file which will be used to create the preference profile of 5 random users.
    fprintf(outfile, "\"Top-3-Critisized-Books\";\"User-ID\";\"Union-of-Keywords\";\"Book-Authors\"\n");

    //Creating a preference profile for 5 random users.
    for(i=1; i<=5; i++)
    {
        //Getting a random user (user id).
        getRandomUser(&aUser);

        //Creating the list which will store the top ratings of the user.
        createList(&topRatings);

        /*Searching the ratings that the specific user has done
          in every book he/she has read, in the "BX-Book-Ratings.csv" file*/
        searchUserRatings(aUser,&topRatings,outfile);

        currPtr = topRatings;

        //Printing the top three books' isbn that the user has rated.
        while(currPtr != NULL)
        {
            fprintf(outfile, "\"%s\" ", currPtr->Data.highest_isbn);
            currPtr = currPtr->Next;
        }

        fprintf(outfile, "\";\"");
        fprintf(outfile, "%s", aUser.user_id);
        fprintf(outfile, "\";\"");

        /*Parsing the "ISBN Key Words.csv" file to examine if the top three rated ISBN
          books exist in the file. If they exist, this function gets their keywords. */
        getISBNkeywords(topRatings,outfile);

        fprintf(outfile, "\";\"");

        /*Parsing the "BX-Books.csv" file to examine if the top three rated ISBN
          books exist in the file. If they exist, this function gets their book authors. */
        getBookAuthor(topRatings,outfile);

        fprintf(outfile, "\"");

        fprintf(outfile," \n");

    }
    fclose(outfile);

    /*Create a recommender file which will recommend some of the best books
      that the specific users can read based on the jaccard and dice coefficient statistics.*/
    createRecommenderFile();

    //Printing the final information.
    printInfo();
}

/*This function parses the file "BX-Users.csv" to get a random user id. It reads lines of the file
  (the amount of lines which must be read is being given from the function printRandoms()) and it
  inserts the user id to the function argument (*aUser). */
void getRandomUser(UserType *aUser)
{
    int i, lower, upper;
    FILE *infile;
    int random_user;
    char buffer[MaxLine];
    char *token;
    int number_of_token;

    //From range 2 to 22817. (22817 is the maximum number of lines of the file "BX-Users.csv".
    lower = 2;
    upper = 22817;

    //Contains uniquely all the users combined with their age and location.
    infile = fopen("BX-Users.csv", "r");
    fgets(buffer,MaxLine,infile);

    //Getting a random integer between 2 and 22817.
    random_user = printRandoms(lower,upper);

    number_of_token = 0;

    i=1;
    //Getting the user id, location and age of a random user (Reading all the lines in the file till we reach the user (random_user).
    while(i<=random_user)
    {
        fgets(buffer,MaxLine,infile);
        token = strtok(buffer,"\"");
        number_of_token = 0;

        while(token)
        {
            switch(number_of_token) {
                case(0):
                    /*Initializing the char * variable (Giving memory space to it). Needing the
                      length of token's size + 1 (for the character \0).*/
                    aUser->user_id = (char *) malloc(strlen(token)+1);
                    strcpy(aUser->user_id,token);
                    break;
                case(1):
                    aUser->location = (char *) malloc(strlen(token)+1);
                    strcpy(aUser->location,token);
                    break;
                case(2):
                    aUser->age = (char *) malloc(strlen(token)+1);
                    strcpy(aUser->age,token);
                    break;
                default:
                    break;
            }
            token = strtok(NULL,"\";\"");
            number_of_token++;
        }
        i++;
    }
    fclose(infile);
}

/*This function parses the file "BX-Book-Ratings.csv" in order to examine if the
  user id which is passed as an argument in this function (aUser) exists in the ratings
  file.If it exists, it calls the function findTopThreeRatings to calculate user's top 3
  highest ratings. */
void searchUserRatings(UserType aUser,ListPointer *topRatings, FILE *outfile)
{
    FILE *infile;
    char buffer[MaxLine], *token;
    int input_value, number_of_token;
    RatingType aRating;
    ListPointer PredPtr;  //A list pointer which shows to the previous list's element.

    infile = fopen("BX-Book-Ratings.csv", "r");  //Contains all the ratings for all the books from all the users.
    fgets(buffer,MaxLine,infile);
    PredPtr = NULL;

    number_of_token = 0;

    while(fgets(buffer,MaxLine,infile) != NULL)
    {
        token = strtok(buffer,"\"");
        number_of_token = 0;

        while(token)
        {
             switch(number_of_token) {
                case(0):
                    aRating.user_id = (char *) malloc(strlen(token)+1);
                    strcpy(aRating.user_id,token);
                    break;
                case(1):
                    aRating.isbn = (char *) malloc(strlen(token)+1);
                    strcpy(aRating.isbn,token);
                    break;
                case(2):
                    aRating.rating = (char *) malloc(strlen(token)+1);
                    strcpy(aRating.rating,token);
                    break;
                default:
                    break;
             }
             token = strtok(NULL,"\";\"");
             number_of_token++;
        }

        /* If the user id found in the ratings file is the same with user_id
           which was found in the users file (The user's ratings have been found).*/
        if(strcmp(aUser.user_id,aRating.user_id) == 0)
        {
            input_value = atoi(aRating.rating);
            //Get the three highest user's ratings.
            findTopThreeRatings(&(*topRatings),input_value,aRating.isbn,PredPtr);
        }
    }
    fclose(infile);
}

/*This function creates a list with the user's top three highest ratings */
void findTopThreeRatings(ListPointer *topRatings, int input_value, char *currentISBN, ListPointer PredPtr)
{
    int max_number_of_elements;       //The maximum number of elements which must exist in the list.
    int current_number_of_elements;   //The current number of elements which exist in the list every time this function is called.
    ListPointer currPtr;
    ListPointer sortedTopRatings;

    //Create a temporary list, which will be used to store temporarily the sorted elements.
    createList(&sortedTopRatings);

    max_number_of_elements = 2;
    current_number_of_elements = 0;

    currPtr = *topRatings;

    //Finding the current number of elements in the list.
    while(currPtr != NULL)
    {
        currPtr = currPtr->Next;
        current_number_of_elements += 1;
    }

    if(emptyList(*topRatings))
    {
        linkedInsert(&(*topRatings),input_value,currentISBN,PredPtr);
    }
    //If there are 1 or 2 elements in the linked list.
    else if(!emptyList(*topRatings) && current_number_of_elements <= max_number_of_elements)
    {
        linkedInsert(&(*topRatings),input_value,currentISBN,PredPtr);
        /*The linked list must be sorted in descending order (so the 1st element
          can be immediately compared with the new value),no need to traverse the whole list. */
        insertionsort(&(*topRatings),&sortedTopRatings);
    }
    //If the linked list has already 3 elements.
    else
    {
        if(input_value >= (*topRatings)->Data.highest_rating)
        {
            //The 1st element of the linked list must be removed.(Needing only 3 elements maximum).
            linkedDelete(&(*topRatings),PredPtr);
            //Inserting the new element which has higher rating.
            linkedInsert(&(*topRatings),input_value,currentISBN,PredPtr);
            //Sorting the linked list in descending order so the 1st element will be the lowest of the 3 ratings.
            insertionsort(&(*topRatings),&sortedTopRatings);
        }
    }
    return;
}

/*This function parses the file "ISBN Key Words.csv" in
  order to get the top three highest rated books' keywords.*/
void getISBNkeywords(ListPointer topRatings,FILE *outfile)
{
    FILE *infile;
    char buffer[MaxLine], *token;
    int number_of_token;
    KeywordType aKeyword;
    ListPointer currPtr;

    infile = fopen("ISBN Key Words.csv","r");
    fgets(buffer,MaxLine,infile);

    number_of_token = 0;

    while(fgets(buffer,MaxLine,infile) != NULL)
    {
        token = strtok(buffer,"\"");
        number_of_token = 0;
        currPtr = topRatings;

        while(token)
        {
            switch(number_of_token) {
                case(0):
                    aKeyword.isbn = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.isbn,token);
                    break;
                case(1):
                    aKeyword.first_keyword = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.first_keyword,token);
                    break;
                case(2):
                    aKeyword.second_keyword = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.second_keyword,token);
                    break;
                default:
                    break;

            }

            token = strtok(NULL,"\";\"");
            number_of_token++;
        }

        //Comparing if the book's isbn that has been read matches
        //with one of the top three isbn that the user has rated.
        while(currPtr != NULL)
        {
            if(strcmp(currPtr->Data.highest_isbn,aKeyword.isbn) == 0)
            {
                //If the first keyword doesn't exist write to the outfile only the second keyword.
                if(strcmp(aKeyword.first_keyword,"null") == 0)
                {
                    fprintf(outfile, "%s ", aKeyword.second_keyword);
                }
                //If the second keyword doesn't exist write to the outfile only the first keyword.
                else if(strcmp(aKeyword.second_keyword,"null") == 0)
                {
                    fprintf(outfile, "%s ", aKeyword.first_keyword);
                }
                else
                {
                    fprintf(outfile, "%s %s ", aKeyword.first_keyword,aKeyword.second_keyword);
                }
            }
            currPtr = currPtr->Next;
        }
    }
    fclose(infile);
}

/*This function parses the file "BX-Books.csv" in order
  to get the top three highest rated books' book authors.*/
void getBookAuthor(ListPointer topRatings,FILE *outfile)
{
    FILE *infile;
    char buffer[MaxLine], *token;
    int number_of_token;
    BookType aBook;
    ListPointer currPtr;

    infile = fopen("BX-Books.csv","r");
    fgets(buffer,MaxLine,infile);

    number_of_token = 0;

    while(fgets(buffer,MaxLine,infile) != NULL)
    {
        token = strtok(buffer,"\"");
        number_of_token = 0;
        currPtr = topRatings;

        while(token)
        {
            switch(number_of_token) {
                case(0):
                    aBook.isbn = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.isbn,token);
                    break;
                case(1):
                    aBook.book_Title = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.book_Title,token);
                    break;
                case(2):
                    aBook.book_Author = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.book_Author,token);
                    break;
                case(3):
                    aBook.year_of_publication = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.year_of_publication,token);
                    break;
                case(4):
                    aBook.publisher = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.publisher,token);
                    break;
                case(5):
                    aBook.image_url_s = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.image_url_s,token);
                    break;
                case(6):
                    aBook.image_url_m = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.image_url_m,token);
                    break;
                case(7):
                    aBook.image_url_l = (char *) malloc(strlen(token)+1);
                    strcpy(aBook.image_url_l,token);
                    break;
                default:
                    break;
            }
            token = strtok(NULL,"\";\"");
            number_of_token++;
        }

        //Comparing if the book's isbn that has been read matches
        //with one of the top three isbn that the user has rated.
        while(currPtr != NULL)
        {

            if(strcmp(currPtr->Data.highest_isbn,aBook.isbn) == 0)
            {
                fprintf(outfile, "%s,", aBook.book_Author);
            }
            currPtr = currPtr->Next;
        }
    }
    fclose(infile);
}

/*This function creates a recommender file based on 2 statistical metrics
  (Jaccard Similarity and Dice Coefficient). The files recommends to a user
  who has been selected randomly, which books he can read and will probably like.*/
void createRecommenderFile()
{
    FILE *outfile;
    int number_of_profiles, i, k;
    double union_of_keywords, union_of_authors;
    PreferenceType aPreferenceProfile;

    outfile = fopen("Recommended Books.csv", "a");
    fprintf(outfile, "\"User-ID\";\"Top-Rated ISBNs\";\"Jaccard-Value\";\"Dice-Coefficient\"\n");

    union_of_keywords = 0;
    union_of_authors = 0;

    //The number of preference profiles needed to be parsed.
    number_of_profiles = 5;

    for(i=1; i<= number_of_profiles; i++)
    {
        //Reading the preference profile for every one of the 5 random users.
        readUserPreferenceProfile(&aPreferenceProfile,&i);

         //Calculating the initial union of keywords.
        for(k=0; aPreferenceProfile.keywords[k] != '\0'; k++)
        {
            if(aPreferenceProfile.keywords[k] == ' ')
            {
                union_of_keywords += 1;
            }
        }
        //Calculating the initial union of authors.
        for(k=0; aPreferenceProfile.book_authors[k] != '\0'; k++)
        {
            if(aPreferenceProfile.book_authors[k] == ',')
            {
                union_of_authors += 1;
            }
        }

        calculateJaccardAndDiceValues(aPreferenceProfile,&union_of_keywords,&union_of_authors,outfile);

        union_of_keywords = 0;
        union_of_authors = 0;
    }
    fclose(outfile);
}

/*This function reads the preference profile for every user */
void readUserPreferenceProfile(PreferenceType *aPreferenceProfile,int *number_of_profile)
{
    int i;
    FILE *infile;
    char buffer[MaxLine];
    char *token;
    int number_of_token;

    infile = fopen("Preference Profile.csv","r");
    fgets(buffer,MaxLine,infile);

    //Reading lines of the file till we read every preference profile for each of the 5 users.
    for(i=0; i<*number_of_profile; i++)
    {
        fgets(buffer,MaxLine,infile);
    }

    token = strtok(buffer,";");
    number_of_token = 0;

    while(token)
    {
        switch(number_of_token) {
            case(0):
                aPreferenceProfile->top_criticized_books = (char *) malloc(strlen(token)+1);
                strcpy(aPreferenceProfile->top_criticized_books,token);
                break;
            case(1):
                aPreferenceProfile->user_id = (char *) malloc(strlen(token)+1);
                strcpy(aPreferenceProfile->user_id,token);
                break;
            case(2):
                aPreferenceProfile->keywords = (char *) malloc(strlen(token)+1);
                strcpy(aPreferenceProfile->keywords,token);
                break;
            case(3):
                aPreferenceProfile->book_authors = (char *) malloc(strlen(token)+1);
                strcpy(aPreferenceProfile->book_authors,token);
            default:
                break;
        }
        token = strtok(NULL,"\";\"");
        number_of_token++;
    }
    fclose(infile);
}

/*This function calculates the Jaccard Similarity and Dice coefficient values for every book
  a specific user hasn't rated. It prints to the recommender file some of the books which have scored
  decent scores in these metrics so the probability that the user will enjoy these books is high. */
void calculateJaccardAndDiceValues(PreferenceType aPreferenceProfile,double *union_of_keywords, double *union_of_authors,FILE *outfile)
{
    FILE *infile;
    char buffer[MaxLine];
    char *token, *second_token;
    int number_of_token;
    double intersection_of_keywords, intersection_of_authors;
    char *book_author, *year_of_publication;
    KeywordType aKeyword;
    double jaccard_keywords_score, jaccard_authors_score, jaccard_total_score, temp_union_of_keywords, temp_union_of_authors;
    double dice_coefficient_keywords_score, dice_coefficient_authors_score, dice_coefficient_total_score;

    infile = fopen("Extended ISBN Key Words.csv","r");
    fgets(buffer,MaxLine,infile);

    number_of_token = 0;

    jaccard_keywords_score = 0;
    jaccard_authors_score = 0;
    jaccard_total_score = 0;
    dice_coefficient_keywords_score = 0;
    dice_coefficient_authors_score = 0;
    dice_coefficient_total_score = 0;
    intersection_of_authors = 0;
    intersection_of_keywords = 0;

    while(fgets(buffer,MaxLine,infile) != NULL)
    {
        token = strtok(buffer,"\"");
        number_of_token = 0;
        /*Using a temporary variable for the union of keywords.
          This is being used because the value of the pointer
         *union_of_keywords must not change inside the function */
        temp_union_of_keywords = *union_of_keywords;
        temp_union_of_authors = *union_of_authors;

        while(token)
        {
            switch(number_of_token) {
                case(0):
                    aKeyword.isbn = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.isbn,token);
                    break;
                case(1):
                    aKeyword.first_keyword = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.first_keyword,token);
                    break;
                case(2):
                    aKeyword.second_keyword = (char *) malloc(strlen(token)+1);
                    strcpy(aKeyword.second_keyword,token);
                    break;
                case(3):
                    book_author = (char *) malloc(strlen(token)+1);
                    strcpy(book_author,token);
                    break;
                case(4):
                    year_of_publication = (char *) malloc(strlen(token)+1);
                    strcpy(year_of_publication,token);
                    break;
                default:
                    break;
            }

            token = strtok(NULL,"\";\"");
            number_of_token++;
        }

        //If the book exists already in the preference profile, skip this iteration.
        if(strstr(aPreferenceProfile.top_criticized_books,aKeyword.isbn) != NULL)
        {
            continue;
        }

        second_token = strtok(aPreferenceProfile.keywords," ");
        while(second_token)
        {
            //Intersections are being increased every time a book's element is equal with the elements of the preference profile.
            if(strcmp(aKeyword.first_keyword,second_token) == 0 || strcmp(aKeyword.second_keyword,second_token) == 0)
            {
                    intersection_of_keywords+=1;
            }
            second_token = strtok(NULL,"\";\"");
        }

        second_token = strtok(aPreferenceProfile.book_authors,",");
        while(second_token)
        {
            if(strcmp(book_author,second_token) == 0 || strcmp(book_author,second_token) == 0)
            {
                intersection_of_authors+=1;
            }
            second_token = strtok(NULL,",\"");
        }

        //Calculating jaccard similarity and dice coefficient scores.(both for keywords and authors).
        jaccard_keywords_score = (intersection_of_keywords / temp_union_of_keywords) * 0.5;
        dice_coefficient_keywords_score = ((2 * intersection_of_keywords) / (*union_of_keywords + intersection_of_keywords)) * 0.5;

        jaccard_authors_score = (intersection_of_authors / temp_union_of_authors) * 0.5;
        dice_coefficient_authors_score = ((2 * intersection_of_authors) / (*union_of_authors + intersection_of_authors)) * 0.5;

        jaccard_total_score = jaccard_authors_score + jaccard_keywords_score;
        dice_coefficient_total_score = dice_coefficient_authors_score + dice_coefficient_keywords_score;

        //Getting only the books which have scored some amount of jaccard and dice coefficient values.
        if(jaccard_total_score != 0)
        {
            fprintf(outfile,"\"%s\";\"%s\";\"%f\";\"%f\"\n", aPreferenceProfile.user_id, aKeyword.isbn, jaccard_total_score, dice_coefficient_total_score);
        }
        intersection_of_keywords = 0;
        intersection_of_authors = 0;
     }
     fclose(infile);
}

/*This function prints the final information to the console,
  explaining to which files the results have been saved. */
void printInfo()
{
    printf("The preference profile for 5 random users has been created in the file Preference Profile.csv \n");
    printf("The top recommended books for these 5 users are being shown in the file Recommended Books.csv ");
    return;
}

/*This function returns a random integer between lower and upper arguments.*/
int printRandoms(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;

    return num;
}

/*This function creates an empty linked list.*/
void createList(ListPointer *List)
{
	*List = NULL;
}

/*This function checks if a linked list is empty.*/
boolean emptyList(ListPointer List)
{
	return (List==NULL);
}

/*This function inserts a rating and an isbn in a linked list after the PredPtr
  which shows to the previous element of the linked list . */
void linkedInsert(ListPointer *List,int rating,char *isbn, ListPointer PredPtr)
{
	ListPointer TempPtr;

    TempPtr= (ListPointer)malloc(sizeof(struct ListNode));
    TempPtr->Data.highest_rating = rating;
    TempPtr->Data.highest_isbn = isbn;

	if (PredPtr==NULL)
    {
        TempPtr->Next = *List;
        *List = TempPtr;
    }
    else {
        TempPtr->Next = PredPtr->Next;
        PredPtr->Next = TempPtr;
    }
}

/*This function traverses a linked list*/
void linkedTraverse(ListPointer List)

{
	ListPointer CurrPtr;

    if (emptyList(List))
    {
        printf("EMPTY LIST\n");
    }
    else
    {
   	    CurrPtr = List;
        while ( CurrPtr!=NULL )
        {
             printf("%d %s \n",CurrPtr->Data.highest_rating, CurrPtr->Data.highest_isbn);
             CurrPtr = CurrPtr->Next;
        }
    }
}

/*This function deletes an element of a linked list which is located after the PredPtr */
void linkedDelete(ListPointer *List, ListPointer PredPtr)
{
    ListPointer TempPtr;

    if (emptyList(*List))
    {
        printf("EMPTY LIST\n");
    }
    else
    {
   	    if (PredPtr == NULL)
        {
      	      TempPtr = *List;
              *List = TempPtr->Next;
        }
        else
        {
      	     TempPtr = PredPtr->Next;
             PredPtr->Next = TempPtr->Next;
        }
        free(TempPtr);
    }
}

/*This function uses insertion sort to a linked list.*/
void insertionsort(ListPointer *List, ListPointer *Sorted)
{

    ListPointer currentPtr;
    ListPointer nextPtr;

    // Traverse the given linked list and insert every
    // node to sorted
    currentPtr = *List;
    while (currentPtr != NULL) {

        nextPtr = currentPtr->Next;

        // insert current in sorted linked list
        sortedInsert(currentPtr,&(*Sorted));

        // Update current
        currentPtr = nextPtr;
    }
    *List = *Sorted;
}

/*This function swaps the elements in a linked list in order
  to perform an insertion sort.*/
void sortedInsert(ListPointer newPtr, ListPointer *Sorted)
{

    ListPointer current;
    /* Special case for the head end */
    if (*Sorted == NULL || (*Sorted)->Data.highest_rating >=  newPtr->Data.highest_rating) {
         newPtr->Next = *Sorted;
        *Sorted = newPtr;
    }
    else {
        current = *Sorted;
        /* Locate the node before the point of insertion
         */
        while (current->Next != NULL
               && current->Next->Data.highest_rating < newPtr->Data.highest_rating) {
            current = current->Next;
        }
        newPtr->Next = current->Next;
        current->Next = newPtr;
    }
}




