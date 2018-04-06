Advanced Database Organization - HW 1 - STORAGE MANAGER

— Team 1 —

— Members —

1. Snehal Mahendrakumar Prajapati   A20400105	
2. Suchita Kiranbhai Patel 			A20410967
3. Mayur Piyushkumar Mehta      	A20405901
4. Rutvik Margeshbhai Pathak		A20405233

— Agenda —

1. Overview for Problem
2. Procedure for executing the function
3. Brief summary of the functions
4. Extra Test cases if any


RUNNING THE SCRIPT
=========================

1) Go to Project root (assign1) using Terminal.

2) Type ls to list the files and check that we are in the correct directory.

3) Type "make clean" to delete old compiled .o files.

4) Type "make" to compile all project files including "test_assign1_1.c" file 

5) Type "./test_assign1" to run executable file.


SOLUTION DESCRIPTION
===========================

MakeFile was made using following tutorial -
http://mrbook.org/blog/tutorials/make/

We have ensured proper memory management while making this storage manager by closing the file stream whenever required and freeing any reserved space.

 
— Brief summary of the functions —


++++++++++++++++++++
|initStorageManager| 
++++++++++++++++++++

--> This function initializes the file pointer variable, which is storage manager, to NULL.

++++++++++++++++
|createPageFile| 
++++++++++++++++

--> This function creates a page file in the memory, which will have file name as specified in the function parameter.
--> fopen() C function is used to create a file with 'w+' mode which creates a new file and opens it for both reading and writing.
--> If the file is not found then error code 'RC_FILE_NOT_FOUND' is returned and 'RC_OK' if everything goes proper.  

++++++++++++++
|openPageFile|
++++++++++++++

--> fopen() C function is used to open the file and 'r' mode is used to open the file in read only mode.
--> We are assigning values to struct fileHandle's variables curPagePos and totalNumPages.
--> fstat() C function is used to retrieve various information regarding the file. Here, we are calculating the size of file using fstat() function.
--> Error code 'RC_FILE_NOT_FOUND' if file could not be opened and 'RC_OK' if everything goes proper.


+++++++++++++++
|closePageFile|
+++++++++++++++

--> This function closes the file, which is pointed by fileHandle's mgmtInfo, when returns 0.
--> It will return the success message if operation is executed otherwise will throw an error if the file is not found.

+++++++++++++++++
|destroyPageFile|
+++++++++++++++++

--> This function deletes the file and checks status if it is successfully deleted or not. 
--> If the file is not found it will throw an error.

+++++++++++
|readBlock|
+++++++++++

--> Check if the page number is valid or not. Page number should be non negative and no more than total number of pages.
--> Create a pointer to the fileName and throw an error if it is NULL.
--> Navigate to given location using fseek() function and a file pointer.
--> If we found the location that we were looking for, read the data from the specified page number  and store it starting at the location pageHandle.

+++++++++++++
|getBlockPos|
+++++++++++++

--> This function returns the current page position which is retrieved from FileHandle's curPagePos.

++++++++++++++++
|readFirstBlock|
++++++++++++++++

-->We call the readBlock(...) function by providing the pageNum argument as 0.
 
+++++++++++++++++++
|readPreviousBlock|
+++++++++++++++++++

--> We call the readBlock(...) function by providing the pageNum argument as (current page position - 1)

++++++++++++++++++
|readCurrentBlock|
++++++++++++++++++

--> We call the readBlock(...) function by providing the pageNum argument as (current page position).  

+++++++++++++++
|readNextBlock|
+++++++++++++++

--> We call the readBlock(...) function by providing the pageNum argument as (current page position + 1)

+++++++++++++++
|readLastBlock|
+++++++++++++++

--> We call the readBlock(...) function by providing the pageNum argument as (total number of pages - 1)
 
++++++++++++
|writeBlock|  
++++++++++++ 

--> Write Block comprises of following C functions to write blocks of data from the disk (memory) to the page file : fseek() and fwrite().

--> Intially we examine whether the page number is valid or not.
--> Here page number should be greater than 0 and less than total number of pages.
--> Check the availablity of pointer to the page file.
--> With the help of valid file pointer, we navigate it to the given location using fseek() function.
--> If the functionality of fseek() is successful, we write the data to the appropriate location using fwrite() function and store into the memPage passed in the paramter.

+++++++++++++++++++
|writeCurrentBlock|
+++++++++++++++++++

--> We call this function with pageNum = current page position as the paramter.

++++++++++++++++++
|appendEmptyBlock|
++++++++++++++++++

--> Create an empty block with size = PAGE_SIZE.
--> Move the pointer(cursor) of the file stream to the last page.
--> Increment total number of pages by 1, write empty block at the end of newly added page

++++++++++++++++
|ensureCapacity|
++++++++++++++++

--> We first check wheather the required number of pages is more than the total number of pages.
--> Calculate number of pages required and add that much number of empty blocks.
--> Add the empty blocks using appendEmptyBlock(..) function

— ADDITIONAL TEST CASES —

Apart from the given test cases we have added few more test cases to test the below functionality:
-createPageFile
-openPageFile
-writeBlock
-writeCurrentBlock
-readFirstBlock
-ensureCapacity
-destroyPageFile






