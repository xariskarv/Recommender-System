# Recommender-System

## Purpose
This project is a recommender system. It is created to recommend the best books to 5 random users, performing Jaccard Similarity and Dice Coefficient statistic metrics to the union/intersection of keywords and authors. So the main criteria for deciding which book must be recommended to a user is based on the highest rated books that a user has rated in the past.

### Explanation
The project uses 4 large datasets (.csv files) with information about:
  * Users
  * Books
  * Ratings
  * Books' keywords (which will help to analyze users' preferences).

The files which are being used are the following:
  1) "BX-Book-Ratings.csv" which provides information about all the ratings that have been done from all the users for every book and whose information consists of:
       * User-id: which is the id of the user.
       * ISBN: which is the unique id number of every book.
       * Book-Rating: which is the rating that the user has done to the book with the above ISBN.
       
  2) "BX-Users" which provides information for all the users and whose information consists of:
       * User-id
       * Location: the user's location.
       * Age: the user's age.
       
  3) "BX-Books.csv" which provides information for all the books and whose information consists of:
       * ISBN
       * Book-Title
       * Book-Author
       * Year-of-Publication
       * Publisher
       * Image-url-s: its a url which shows a small image of the book.
       * Image-url-m: same as above but shows a medium image.
       * Image-url-l: same as above but shows a large image
       
  4) "ISBN Key Words.csv" which provides information about 2 specific keywords for every book and whose information consists of:
       * ISBN
       * 1st-Keyword: shows the first important keyword of the book's title.
       * 2nd_Keyword: shows the second important keyword of the book's title.
       
  *Also an extra file is used which is the "Extended ISBN Key Words.csv" which has exactly the same information as "ISBN Key Words.csv" but 2 more columns have been      added.
   1) Book-Author
   2) Year-of-Publication
   
both from the "BX-Books.csv" file. 

### The project implements 2 parts:
 #### 1st Part
 It makes a preference profile for 5 random users based on the top three highest rated books by these users. It combines all the keywords and all the authors of these 3 books so these data can be parsed later to create the recommended file (which will recommend the best books which hasnt been rated from these users). Its results are being shown in the file "Preference Profile.csv".
 
 #### 2nd Part 
 It makes a recommender system for these 5 users, based on their preference profile and by performing Jaccard Similarity and Dice Coefficient metrics to the union of keywords/authors. The final results are being shown in the file "Recommended Books.csv" which shows for every user his recommended books and for every book its jaccard/dice coefficient score.
  
