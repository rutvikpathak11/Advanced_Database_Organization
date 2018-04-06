#include "storage_mgr.h"

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>

FILE *filePointer;

extern void initStorageManager (void) 
{
	filePointer = NULL;
}

//Creates an empty page in memory
extern RC createPageFile (char *fileName)
{
	int retCode = RC_OK;
	//'w+' mode in fopen creates an empty file 'fileName' for read and write purpose
	filePointer = fopen(fileName, "w+");

	//Checking if the file, stored in 'filePointer' is successufully opened or not, if yes then  
	if(filePointer != NULL)
	{
		//Creating blank page in memory, to store no. of 'PAGE_SIZE' elements, each of size of character 								
		SM_PageHandle blankPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
		
		//fwrite(buffer, item size, max no of lines, file handle where to write the data) returns the size of data written
		if(fwrite(blankPage, sizeof(char), PAGE_SIZE, filePointer) < PAGE_SIZE)
			printf("\n File Write Failed ! \n");
		else
			printf("\n File Write Succeessfull \n");

		//Closing output file stream so that all buffers are flushed
		fclose(filePointer);

		//memory deallocation to ensure proper memory management
		free(blankPage);

		retCode = RC_OK;
	}
	else
		retCode = RC_FILE_NOT_FOUND;

	return retCode;
}

//Open the file in read mode and set the values in file structure
extern RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
	int retCode = RC_OK;
	//Open the file in read mode
	filePointer = fopen(fileName, "r");

	if(filePointer != NULL)
	{
		//Check the no of pages
		struct stat st_file_info;
		int errCode = fstat(fileno(filePointer), &st_file_info);
		if(errCode >= 0)
		{
			//To set the total no of pages in the file
			int totalPages = st_file_info.st_size/PAGE_SIZE;
			//Check if the last page is partially written, then count that page also
			if((st_file_info.st_size) % PAGE_SIZE > 0)
				fHandle->totalNumPages = totalPages++;
			else
				fHandle->totalNumPages = totalPages;
			retCode = RC_OK;
		}
		else
			retCode = RC_TOTAL_PAGES_NOT_INIT;

		//Set the file information
		fHandle->fileName = fileName;
		fHandle->curPagePos = 0;
		fHandle->mgmtInfo = filePointer;

	}
	else
		retCode = RC_FILE_NOT_FOUND;

	if(filePointer != NULL)
		fclose(filePointer);
	return retCode;
}

//If the file is opened for reading or writing then it will be closed 
extern RC closePageFile (SM_FileHandle *fHandle)
{
	int retCode = RC_OK;
	if(filePointer != NULL)
	{
		filePointer = NULL;
		retCode = RC_OK;
	}	
	else
		retCode = RC_FILE_NOT_CLOSED;
	return retCode;
}

//Delete the page file
extern RC destroyPageFile(char *fileName)
{	
	int retCode = RC_OK;
	// Deleting the given filename and checking the status if it is correctly deleted or not
	int fileStatus = remove(fileName);
	if(fileStatus == -1)
		retCode = RC_FILE_NOT_FOUND;
	else
		retCode = RC_OK;
	return retCode;
}

//Read from file to block in memory
extern RC readBlock (int pageNum, SM_FileHandle *fileHandle, SM_PageHandle pageHandle)
{
	int i,j;
	int retCode = RC_OK;

	//Condition to check pagenumber value
	if (pageNum < 0 || pageNum > fileHandle->totalNumPages)
	{
		retCode = RC_READ_NON_EXISTING_PAGE;
	}
	if(retCode == RC_OK)
	{
		//opening file in read mode - opens empty file
		filePointer = fopen(fileHandle->fileName, "r");

		//condition to check fileopen statement
		if(filePointer==NULL)
		{
			retCode = RC_FILE_NOT_FOUND;
		}

		if(retCode == RC_OK)
		{
			//Adjust the position of the pointer filestream, position is evaluated by pagenumber * PAGE_SIZE
			//seek is complete when it is equal to 0
			int seek_s = fseek(filePointer,(pageNum * PAGE_SIZE),SEEK_SET);

			if (seek_s != 0)
				retCode = RC_READ_NON_EXISTING_PAGE;
			else
			{
				//Reading the content, storing it to the location set out by memPage
				fread(pageHandle, sizeof(char), PAGE_SIZE, filePointer);
				//retCode = RC_OK;
			}
			if(retCode == RC_OK)
			{
				//Setting the current page position to the pointer
				//After reading 1 block/page what is the current posotion of the pointer in file. It returns the current location in file stream
				fileHandle->curPagePos = ftell(filePointer);
			}
		}
		if(filePointer != NULL)
			fclose(filePointer);
	}
	return retCode;
}

extern int getBlockPos (SM_FileHandle *fileHandle)
{  
	int k;
	while(k>0){
    //Returning current position of page, retrieved from the fhandle. It will be same as file position
    return fileHandle->curPagePos;
    break;
	}
}

extern RC readFirstBlock (SM_FileHandle *fileHandle, SM_PageHandle pageHandle)
{
	//Passing to the readblock function setting page number to 0
	return readBlock(0, fileHandle, pageHandle);
}

extern RC readPreviousBlock (SM_FileHandle *fileHandle, SM_PageHandle pageHandle)
{
	int l;
    //Calculating current page number by dividing page size to current page position
    int prevailingPageNum = fileHandle->curPagePos / PAGE_SIZE;
    while(l>0)
    {
        //Passing to the readblock function setting page number to previous block = current - 1
        return readBlock(prevailingPageNum - 1, fileHandle, pageHandle);
        break;
    }
}

extern RC readCurrentBlock(SM_FileHandle *fileHandle , SM_PageHandle pageHandle)
{
	//Finding the current page number by dividing current page position by size of the page. It will have position in bytes
	int prevailingPageNum = fileHandle -> curPagePos / PAGE_SIZE ;
	
	//good to read, so recursively call readBlock subroutine
	return readBlock(prevailingPageNum , fileHandle , pageHandle);
}

extern RC readNextBlock(SM_FileHandle *fileHandle , SM_PageHandle pageHandle)
{
	//Find the current page number the same was as reading the current block
	int prevailingPageNum = fileHandle -> curPagePos / PAGE_SIZE ;

	//to read next block, add "1" in the prevalingPageNum for recursive reading 
	return readBlock(prevailingPageNum+1 , fileHandle , pageHandle);
}

extern RC readLastBlock(SM_FileHandle *fileHandle, SM_PageHandle pageHandle)
{
	// get the total number of pages
	int totalPages = fileHandle -> totalNumPages - 1;

	// read the last block by recursively calling readBlock with totalPages
	return readBlock(totalPages , fileHandle, pageHandle);
}

//Logic to write block char by char
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int retCode = RC_OK;
	//First check if the no of pages is proper or not
	if(pageNum < 0 || pageNum > (fHandle->totalNumPages + 1))
		retCode = RC_PAGE_NOT_FOUND;

	if(retCode == RC_OK)
	{
		//Open the file in write mode
		filePointer = fopen(fHandle->fileName, "r+");
		if(filePointer == NULL)
			retCode = RC_FILE_NOT_FOUND;

		if(retCode == RC_OK)
		{
			//Now move the current pointer location to appropriate location in the file
			unsigned int offset = PAGE_SIZE * pageNum;
			if(fseek(filePointer, offset, SEEK_SET) == 0)
			{
				unsigned int bytesWrittenToFile = fwrite(memPage, sizeof(char), PAGE_SIZE, filePointer);
				if(bytesWrittenToFile == PAGE_SIZE)
				{
					//It will set the pointer to current position in the file (in bytes)
					fHandle->curPagePos = ftell(filePointer);
					retCode = RC_OK;
				}
				else
					retCode = RC_COMPLETE_BLOCK_NOT_WRITTEN;
			}
			else
				retCode = RC_SET_FILE_OFFSET_FAILED;
		}
		//Close the file irrespective of the outcome
		if(filePointer != NULL)
			fclose(filePointer);
	}
	return retCode;
}

//Write the block/page to the file on disk
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int pageNumToWrite;
	//Calculate current page no: floor value (already written pages) + 1 (new/curr page to write)
	pageNumToWrite = (fHandle->curPagePos/PAGE_SIZE) + 1;

	int returnCode = writeBlock(pageNumToWrite, fHandle, memPage);
	if(returnCode == RC_OK)
	{
		//file write successful, so now update the info about file in the handler
		fHandle->totalNumPages++;
		returnCode = RC_OK;
	}
	return returnCode;
}

//Add empty block to file
extern RC appendEmptyBlock (SM_FileHandle *fHandle)
{
	int retCode = RC_OK;
	SM_PageHandle startOf_Empty_Block = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));

	//Moving the pointer to the end of the file
	if(fseek(filePointer, 0, SEEK_END) == 0)
	{
		if(fwrite(startOf_Empty_Block, sizeof(char), PAGE_SIZE, filePointer) == PAGE_SIZE)
		{
			//Set the new value to total no of pages
			fHandle->totalNumPages++;
			retCode = RC_OK;
		}
	}
	else
		retCode = RC_WRITE_FAILED;
	
	//Irrespective of result of write, free the allocated space to ensure memory management
	free(startOf_Empty_Block);
	return retCode;
}

//Add empty blocks so that it equals to the total no of pages in file
extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
	int retCode = RC_OK;
	//Open the file in append mode
	filePointer = fopen(fHandle->fileName, "a");
	if(filePointer != NULL)
	{
		//Append empty pages until the numberOfPages is equal to the pages in file
		while(fHandle->totalNumPages < numberOfPages)
			appendEmptyBlock(fHandle);
	}
	else
		retCode = RC_FILE_NOT_FOUND;
	if(filePointer != NULL)
		fclose(filePointer);
	return retCode;
}
