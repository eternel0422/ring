/*
**  Copyright (c) 2013-2024 Mahmoud Fayed <msfclipper@yahoo.com> 
**  Include Files 
*/
#include "ring.h"
/* Define Functions */

void ring_objfile_writefile ( RingState *pRingState )
{
	FILE *fObj;
	char cFileName[400]  ;
	/* Create File */
	sprintf( cFileName , "%so" , ring_list_getstring(pRingState->pRingFilesList,1) ) ;
	fObj = fopen(cFileName , "w+b" );
	fprintf( fObj , "# Ring Object File\n"  ) ;
	fprintf( fObj , RING_OBJFILE_VERSION  ) ;
	fprintf( fObj , "\n"  ) ;
	/* Write Functions Lists */
	fprintf( fObj , "# Functions List\n"  ) ;
	ring_objfile_writelist(pRingState->pRingFunctionsMap,fObj);
	/* Write Classes List */
	fprintf( fObj , "# Classes List\n"  ) ;
	ring_objfile_writelist(pRingState->pRingClassesMap,fObj);
	/* Write Packages */
	fprintf( fObj , "# Packages List\n"  ) ;
	ring_objfile_writelist(pRingState->pRingPackagesMap,fObj);
	/* Write Code */
	fprintf( fObj , "# Program Code\n"  ) ;
	ring_objfile_writelist(pRingState->pRingGenCode,fObj);
	/* Write Files List */
	fprintf( fObj , "# Files List\n"  ) ;
	ring_objfile_writelist(pRingState->pRingFilesList,fObj);
	/* Close File */
	fprintf( fObj , "# End of File\n"  ) ;
	fclose( fObj ) ;
}

void ring_objfile_writelist ( List *pList,FILE *fObj )
{
	List *pList2  ;
	int x,x2,lCont  ;
	char *cString  ;
	char cKey[11]  ;
	strcpy(cKey,"ringstring");
	fprintf( fObj , "{\n"  ) ;
	/* Write List Items */
	lCont = 1 ;
	for ( x = 1 ; x <= ring_list_getsize(pList) ; x++ ) {
		if ( ring_list_islist(pList,x) ) {
			pList2 = ring_list_getlist(pList,x);
		}
		else {
			/* For the Files List */
			pList2 = pList ;
			lCont = 0 ;
		}
		fprintf( fObj , "[T]\n"  ) ;
		for ( x2 = 1 ; x2 <= ring_list_getsize(pList2) ; x2++ ) {
			if ( ring_list_isstring(pList2,x2) ) {
				fprintf( fObj , "[S][%d]" , ring_list_getstringsize(pList2,x2) ) ;
				/* Encrypt String */
				cString = ring_list_getstring(pList2,x2) ;
				ring_objfile_xorstring(cString,ring_list_getstringsize(pList2,x2),cKey,10);
				fwrite( ring_list_getstring(pList2,x2) , 1 , ring_list_getstringsize(pList2,x2) , fObj );
				/* Decrypt String */
				ring_objfile_xorstring(cString,ring_list_getstringsize(pList2,x2),cKey,10);
				fprintf( fObj , "\n"  ) ;
			}
			else if ( ring_list_isint(pList2,x2) ) {
				fprintf( fObj , "[I]%d\n" , ring_list_getint(pList2,x2) ) ;
			}
			else if ( ring_list_isdouble(pList2,x2) ) {
				fprintf( fObj , "[D]%f\n" , ring_list_getdouble(pList2,x2) ) ;
			}
			else if ( ring_list_ispointer(pList2,x2) ) {
				fprintf( fObj , "[P]%p\n" , (void *) ring_list_getpointer(pList2,x2) ) ;
			}
			else if ( ring_list_islist(pList2,x2) ) {
				fprintf( fObj , "[L]\n"  ) ;
				ring_objfile_writelist(ring_list_getlist(pList2,x2) ,fObj);
			}
		}
		fprintf( fObj , "[E]\n"  ) ;
		if ( lCont == 0 ) {
			break ;
		}
	}
	fprintf( fObj , "}\n"  ) ;
}

int ring_objfile_readfile ( RingState *pRingState,char *cFileName )
{
	return ring_objfile_readfromsource(pRingState,cFileName,RING_OBJFILE_READFROMFILE) ;
}

int ring_objfile_readstring ( RingState *pRingState,char *cString )
{
	return ring_objfile_readfromsource(pRingState,cString,RING_OBJFILE_READFROMSTRING) ;
}

int ring_objfile_readfromsource ( RingState *pRingState,char *cSource,int nSource )
{
	List *pListFunctions, *pListClasses, *pListPackages, *pListCode, *pListFiles, *pListStack  ;
	/* Create Lists */
	pListFunctions = ring_list_new_gc(pRingState,0);
	pListClasses = ring_list_new_gc(pRingState,0);
	pListPackages = ring_list_new_gc(pRingState,0);
	pListCode = ring_list_new_gc(pRingState,0);
	pListFiles = ring_list_new_gc(pRingState,0);
	pListStack = ring_list_new_gc(pRingState,0);
	/* Process Content (File or String) */
	if ( nSource == RING_OBJFILE_READFROMFILE ) {
		if ( ! ring_objfile_processfile(pRingState,cSource,pListFunctions, pListClasses, pListPackages, pListCode, pListFiles, pListStack) ) {
			return 0 ;
		}
	}
	else if ( nSource ==RING_OBJFILE_READFROMSTRING ) {
		if ( ! ring_objfile_processstring(pRingState,cSource,pListFunctions, pListClasses, pListPackages, pListCode, pListFiles, pListStack) ) {
			return 0 ;
		}
	}
	ring_list_delete_gc(pRingState,pListStack);
	/* Update Ring State */
	#ifdef DEBUG_OBJFILE
		puts("Old Code List ");
		ring_list_print(pRingState->pRingGenCode);
	#endif
	/* Update Lists */
	pRingState->pRingFunctionsMap = pListFunctions ;
	pRingState->pRingClassesMap = pListClasses ;
	pRingState->pRingPackagesMap = pListPackages ;
	pRingState->pRingGenCode = pListCode ;
	/*
	**  Update the Files List 
	**  Delete the old list (Contains only one file - the *.ringo file name) 
	*/
	ring_list_deleteallitems_gc(pRingState,pRingState->pRingFilesList);
	/*
	**  Add all source code files (*.ring files) in the project 
	**  The List contains sub list - i.e. looks like  [  [ files ] ] - but we need [ files ] only 
	**  So we get the first item using ring_list_getlist() function 
	*/
	ring_list_copy_gc(pRingState,pRingState->pRingFilesList,ring_list_getlist(pListFiles,1));
	#ifdef DEBUG_OBJFILE
		puts("Update Done! ");
		puts("New Code List ");
		ring_list_print(pRingState->pRingGenCode);
	#endif
	/* Update Classes Pointers */
	ring_objfile_updateclassespointers(pRingState);
	return 1 ;
}

int ring_objfile_processfile ( RingState *pRingState,char *cFileName,List *pListFunctions,List  *pListClasses,List  *pListPackages,List  *pListCode, List *pListFiles,List  *pListStack )
{
	FILE *fObj;
	signed char c  ;
	int nActiveList,nValue,nBraceEnd,nOutput  ;
	double dValue  ;
	char *cString  ;
	char cKey[11]  ;
	char cFileType[100]  ;
	List *pList  ;
	strcpy(cKey,"ringstring");
	/* Set Active List (1=functions 2=classes 3=packages 4=code) */
	nActiveList = 0 ;
	nBraceEnd = 0 ;
	pList = NULL ;
	/* Open File */
	fObj = fopen(cFileName , "rb" );
	if ( fObj==NULL ) {
		printf( "Can't open file %s \n  ",cFileName ) ;
		return 0 ;
	}
	fread( cFileType , 1 , 18 , fObj );
	cFileType[18] = '\0' ;
	if ( strcmp(cFileType,"# Ring Object File") != 0 ) {
		printf( RING_OBJFILEWRONGTYPE ) ;
		return 0 ;
	}
	c = getc(fObj);
	fread( cFileType , 1 , RING_OBJFILE_VERSIONSTRINGSIZE , fObj );
	cFileType[RING_OBJFILE_VERSIONSTRINGSIZE] = '\0' ;
	if ( strcmp(cFileType,RING_OBJFILE_VERSION) != 0 ) {
		printf( RING_OBJFILEWRONGVERSION ) ;
		return 0 ;
	}
	/* Process File */
	c = getc(fObj);
	while ( (c == '#') || (c == '{') || (c == '}') || (c == '[')  || (c == '\n') ) {
		/* Check Char */
		switch ( c ) {
			case '#' :
				/* Read Line */
				while ( c != '\n' ) {
					c = getc(fObj);
					#ifdef DEBUG_OBJFILE
						printf( "%c  ",c ) ;
					#endif
				}
				#ifdef DEBUG_OBJFILE
					puts("Read Comment ! ");
				#endif
				break ;
			case '{' :
				nActiveList++ ;
				switch ( nActiveList ) {
					case 1 :
						pList = pListFunctions ;
						break ;
					case 2 :
						pList = pListClasses ;
						break ;
					case 3 :
						pList = pListPackages ;
						break ;
					case 4 :
						pList = pListCode ;
						break ;
					case 5 :
						pList = pListFiles ;
						break ;
				}
				break ;
			case '[' :
				c = getc(fObj);
				switch ( c ) {
					case 'S' :
						c = getc(fObj);
						nOutput = fscanf( fObj , "[%d]" , &nValue ) ;
						if ( nOutput == 0 ) {
							printf( RING_FSCANFERROR ) ;
							return 0 ;
						}
						cString = (char *) ring_state_malloc(pRingState,nValue+1);
						fread( cString , 1 , nValue , fObj );
						cString[nValue] = '\0' ;
						/* Decrypt String */
						ring_objfile_xorstring(cString,nValue,cKey,10);
						ring_list_addstring2_gc(pRingState,pList,cString,nValue);
						ring_state_free(pRingState,cString);
						#ifdef DEBUG_OBJFILE
							printf( "Read String %s Size %d \n",cString,nValue ) ;
						#endif
						break ;
					case 'I' :
						c = getc(fObj);
						nOutput = fscanf( fObj , "%d" , &nValue ) ;
						if ( nOutput == 0 ) {
							printf( RING_FSCANFERROR ) ;
							return 0 ;
						}
						ring_list_addint_gc(pRingState,pList,nValue);
						#ifdef DEBUG_OBJFILE
							printf( "Read Number %d \n  ",nValue ) ;
						#endif
						break ;
					case 'D' :
						c = getc(fObj);
						nOutput = fscanf( fObj , "%lf" , &dValue ) ;
						if ( nOutput == 0 ) {
							printf( RING_FSCANFERROR ) ;
							return 0 ;
						}
						ring_list_adddouble_gc(pRingState,pList,dValue);
						#ifdef DEBUG_OBJFILE
							printf( "Read Double %d  \n",dValue ) ;
						#endif
						break ;
					case 'P' :
						ring_list_addpointer_gc(pRingState,pList,NULL);
						/* Read Line */
						while ( c != '\n' ) {
							c = getc(fObj);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read Pointer ");
						#endif
						break ;
					case 'T' :
						ring_list_addpointer_gc(pRingState,pListStack,pList);
						pList = ring_list_newlist_gc(pRingState,pList);
						/* Read Line */
						while ( c != '\n' ) {
							c = getc(fObj);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read T ");
						#endif
						break ;
					case 'E' :
						pList = (List *) ring_list_getpointer(pListStack,ring_list_getsize(pListStack)) ;
						ring_list_deletelastitem_gc(pRingState,pListStack);
						/* Read Line */
						while ( c != '\n' ) {
							c = getc(fObj);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read E ");
						#endif
						break ;
					case 'L' :
						/* Read Until { */
						while ( c != '{' ) {
							c = getc(fObj);
						}
						ring_list_addpointer_gc(pRingState,pListStack,pList);
						pList = ring_list_newlist_gc(pRingState,pList);
						nBraceEnd++ ;
						#ifdef DEBUG_OBJFILE
							puts("Read L ");
						#endif
						break ;
				}
				break ;
			case '}' :
				if ( nBraceEnd ) {
					pList = (List *) ring_list_getpointer(pListStack,ring_list_getsize(pListStack)) ;
					ring_list_deletelastitem_gc(pRingState,pListStack);
					nBraceEnd-- ;
					#ifdef DEBUG_OBJFILE
						puts("Read } ");
					#endif
				}
				break ;
		}
		c = getc(fObj);
	}
	/* Close File */
	fclose( fObj ) ;
	return 1 ;
}

int ring_objfile_processstring ( RingState *pRingState,char *cContent,List *pListFunctions,List  *pListClasses,List  *pListPackages,List  *pListCode, List *pListFiles,List  *pListStack )
{
	signed char c  ;
	int nActiveList,nValue,nBraceEnd,nOutput  ;
	double dValue  ;
	char *cString, *cData  ;
	char cKey[11]  ;
	char cFileType[100]  ;
	List *pList  ;
	strcpy(cKey,"ringstring");
	/* Set Active List (1=functions 2=classes 3=packages 4=code) */
	nActiveList = 0 ;
	nBraceEnd = 0 ;
	pList = NULL ;
	cData = cContent ;
	/* Check Type and Version */
	ring_objfile_readc(pRingState,&cData,cFileType,18);
	cFileType[18] = '\0' ;
	if ( strcmp(cFileType,"# Ring Object File") != 0 ) {
		printf( RING_OBJFILEWRONGTYPE ) ;
		return 0 ;
	}
	c = ring_objfile_getc(pRingState,&cData);
	ring_objfile_readc(pRingState,&cData,cFileType,RING_OBJFILE_VERSIONSTRINGSIZE);
	cFileType[RING_OBJFILE_VERSIONSTRINGSIZE] = '\0' ;
	if ( strcmp(cFileType,RING_OBJFILE_VERSION) != 0 ) {
		printf( RING_OBJFILEWRONGVERSION ) ;
		return 0 ;
	}
	/* Process Content */
	c = ring_objfile_getc(pRingState,&cData);
	while ( (c == '#') || (c == '{') || (c == '}') || (c == '[')  || (c == '\n') ) {
		/* Check Char */
		switch ( c ) {
			case '#' :
				/* Read Line */
				while ( c != '\n' ) {
					c = ring_objfile_getc(pRingState,&cData);
					#ifdef DEBUG_OBJFILE
						printf( "%c  ",c ) ;
					#endif
				}
				#ifdef DEBUG_OBJFILE
					puts("Read Comment ! ");
				#endif
				break ;
			case '{' :
				nActiveList++ ;
				switch ( nActiveList ) {
					case 1 :
						pList = pListFunctions ;
						break ;
					case 2 :
						pList = pListClasses ;
						break ;
					case 3 :
						pList = pListPackages ;
						break ;
					case 4 :
						pList = pListCode ;
						break ;
					case 5 :
						pList = pListFiles ;
						break ;
				}
				break ;
			case '[' :
				c = ring_objfile_getc(pRingState,&cData);
				switch ( c ) {
					case 'S' :
						c = ring_objfile_getc(pRingState,&cData);
						nOutput = sscanf(cData,"[%d]",&nValue);
						if ( nOutput == EOF ) {
							printf( RING_SSCANFERROR ) ;
							return 0 ;
						}
						/* Pass Letters */
						c = ' ' ;
						while ( c != ']' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						cString = (char *) ring_state_malloc(pRingState,nValue+1);
						ring_objfile_readc(pRingState,&cData,cString,nValue);
						cString[nValue] = '\0' ;
						/* Decrypt String */
						ring_objfile_xorstring(cString,nValue,cKey,10);
						ring_list_addstring2_gc(pRingState,pList,cString,nValue);
						#ifdef DEBUG_OBJFILE
							printf( "Read String %s Size %d \n",cString,nValue ) ;
						#endif
						ring_state_free(pRingState,cString);
						break ;
					case 'I' :
						c = ring_objfile_getc(pRingState,&cData);
						nOutput = sscanf(cData,"%d",&nValue);
						if ( nOutput == EOF ) {
							printf( RING_SSCANFERROR ) ;
							return 0 ;
						}
						/* Pass Letters */
						c = '0' ;
						while ( isdigit(c) || c=='.' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						cData-- ;
						ring_list_addint_gc(pRingState,pList,nValue);
						#ifdef DEBUG_OBJFILE
							printf( "Read Number %d \n  ",nValue ) ;
						#endif
						break ;
					case 'D' :
						c = ring_objfile_getc(pRingState,&cData);
						nOutput = sscanf(cData,"%lf",&dValue);
						if ( nOutput == EOF ) {
							printf( RING_SSCANFERROR ) ;
							return 0 ;
						}
						/* Pass Letters */
						c = '0' ;
						while ( isdigit(c) || c=='.' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						cData-- ;
						ring_list_adddouble_gc(pRingState,pList,dValue);
						#ifdef DEBUG_OBJFILE
							printf( "Read Double %d  \n",dValue ) ;
						#endif
						break ;
					case 'P' :
						ring_list_addpointer_gc(pRingState,pList,NULL);
						/* Read Line */
						while ( c != '\n' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read Pointer ");
						#endif
						break ;
					case 'T' :
						ring_list_addpointer_gc(pRingState,pListStack,pList);
						pList = ring_list_newlist_gc(pRingState,pList);
						/* Read Line */
						while ( c != '\n' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read T ");
						#endif
						break ;
					case 'E' :
						pList = (List *) ring_list_getpointer(pListStack,ring_list_getsize(pListStack)) ;
						ring_list_deletelastitem_gc(pRingState,pListStack);
						/* Read Line */
						while ( c != '\n' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						#ifdef DEBUG_OBJFILE
							puts("Read E ");
						#endif
						break ;
					case 'L' :
						/* Read Until { */
						while ( c != '{' ) {
							c = ring_objfile_getc(pRingState,&cData);
						}
						ring_list_addpointer_gc(pRingState,pListStack,pList);
						pList = ring_list_newlist_gc(pRingState,pList);
						nBraceEnd++ ;
						#ifdef DEBUG_OBJFILE
							puts("Read L ");
						#endif
						break ;
				}
				break ;
			case '}' :
				if ( nBraceEnd ) {
					pList = (List *) ring_list_getpointer(pListStack,ring_list_getsize(pListStack)) ;
					ring_list_deletelastitem_gc(pRingState,pListStack);
					nBraceEnd-- ;
					#ifdef DEBUG_OBJFILE
						puts("Read } ");
					#endif
				}
				break ;
		}
		c = ring_objfile_getc(pRingState,&cData);
	}
	return 1 ;
}

RING_API void ring_objfile_updateclassespointers ( RingState *pRingState )
{
	ring_vm_updateclassespointers(pRingState);
}

void ring_objfile_xorstring ( char *cString,int nStringSize,char *cKey,int nKeySize )
{
	int x  ;
	for ( x = 1 ; x <= nStringSize ; x++ ) {
		cString[x-1] = cString[x-1] ^ cKey[(x-1)%nKeySize] ;
	}
}

void ring_objfile_readc ( RingState *pRingState,char **cSource,char *cDest,int nCount )
{
	int x  ;
	char *cData  ;
	cData = *cSource ;
	for ( x = 0 ; x < nCount ; x++ ) {
		cDest[x] = cData[x] ;
	}
	*cSource += nCount ;
	cDest[nCount] = '\0' ;
}

char ring_objfile_getc ( RingState *pRingState,char **cSource )
{
	char c  ;
	char *cData  ;
	cData = *cSource ;
	c = cData[0] ;
	*cSource+=1 ;
	return c ;
}

void ring_objfile_writeCfile ( RingState *pRingState )
{
	FILE *fCode, *fCode2;
	char cCodeFileName[400]  ;
	int nSize,x,nFunction  ;
	/*
	**  Write C file 
	**  Set the file name 
	*/
	sprintf( cCodeFileName , "%s" , ring_list_getstring(pRingState->pRingFilesList,1) ) ;
	nSize = strlen( cCodeFileName ) ;
	cCodeFileName[nSize-4] = 'c' ;
	cCodeFileName[nSize-3] = '\0' ;
	fCode = fopen(cCodeFileName , "w+b" );
	/* write the main function */
	fprintf( fCode , "#include \"ring.h\" \n\n"  ) ;
	fprintf( fCode , "#include \"ringappcode.h\" \n\n"  ) ;
	fprintf( fCode , "int main( int argc, char *argv[])\n"  ) ;
	fprintf( fCode , "{\n"  ) ;
	/* main function code */
	fprintf( fCode , "\tRingState *pRingState;  \n"  ) ;
	fprintf( fCode , "\tpRingState = ring_state_new();  \n"  ) ;
	fprintf( fCode , "\tpRingState->argc = argc;  \n"  ) ;
	fprintf( fCode , "\tpRingState->argv = argv;  \n"  ) ;
	fprintf( fCode , "\tpRingState->pRingFilesList = ring_list_new_gc(pRingState,0);  \n"  ) ;
	fprintf( fCode , "\tpRingState->pRingFilesStack = ring_list_new_gc(pRingState,0);  \n"  ) ;
	fprintf( fCode , "\tring_list_addstring_gc(pRingState,pRingState->pRingFilesList,\"%so\");  \n",ring_list_getstring(pRingState->pRingFilesList,1)  ) ;
	fprintf( fCode , "\tring_list_addstring_gc(pRingState,pRingState->pRingFilesStack,\"%so\");  \n",ring_list_getstring(pRingState->pRingFilesList,1)  ) ;
	fprintf( fCode , "\tloadRingCode(pRingState);  \n"  ) ;
	fprintf( fCode , "\tring_objfile_updateclassespointers(pRingState);  \n"  ) ;
	fprintf( fCode , "\tring_state_runprogram(pRingState);  \n"  ) ;
	fprintf( fCode , "\tring_state_delete(pRingState);  \n"  ) ;
	fprintf( fCode , "\treturn 0;  \n"  ) ;
	fprintf( fCode , "}\n\n"  ) ;
	/* Create ringappcode.c */
	fclose( fCode ) ;
	fCode = fopen("ringappcode.c" , "w+b" );
	fprintf( fCode , "#include \"ringappcode.h\" \n\n"  ) ;
	fprintf( fCode , "void loadRingCode(RingState *pRingState) {\n"  ) ;
	fprintf( fCode , "\tList *pList1,*pList2,*pList3,*pList4,*pList5,*pList6 ;\n"  ) ;
	/* Write Data */
	nFunction = ring_objfile_writelistcode(pRingState->pRingFunctionsMap,fCode,1,1,0,RING_OBJFILE_ITEMSPERFUNCTION2);
	fprintf( fCode , "\tpRingState->pRingFunctionsMap = pList1;\n"  ) ;
	nFunction = ring_objfile_writelistcode(pRingState->pRingClassesMap,fCode,1,1,nFunction,RING_OBJFILE_ITEMSPERFUNCTION2);
	fprintf( fCode , "\tpRingState->pRingClassesMap = pList1;\n"  ) ;
	nFunction = ring_objfile_writelistcode(pRingState->pRingPackagesMap,fCode,1,1,nFunction,RING_OBJFILE_ITEMSPERFUNCTION2);
	fprintf( fCode , "\tpRingState->pRingPackagesMap = pList1;\n"  ) ;
	nFunction = ring_objfile_writelistcode(pRingState->pRingGenCode,fCode,1,1,nFunction,RING_OBJFILE_ITEMSPERFUNCTION);
	fprintf( fCode , "\tpRingState->pRingGenCode = pList1;\n"  ) ;
	fprintf( fCode , "}\n"  ) ;
	/* Close File */
	fclose( fCode ) ;
	/* Declare functions that load the Ring code */
	fCode2 = fopen("ringappcode.h" , "w+b" );
	fprintf( fCode2 , "#include \"ring.h\" \n\n"  ) ;
	fprintf( fCode2 , "void loadRingCode(RingState *pRingState) ;\n\n"  ) ;
	for ( x = 1 ; x <= nFunction ; x++ ) {
		fprintf( fCode2 , "void loadRingCode%d(RingState *pRingState,List *pList1) ;\n\n",x  ) ;
	}
	fclose( fCode2 ) ;
}

int ring_objfile_writelistcode ( List *pList,FILE *fCode,int nList,int lSeparate,int nFunction,int nItemsPerFunction )
{
	List *pList2  ;
	int x,x2,x3,nMax  ;
	char cList[20]  ;
	char *cString  ;
	char cFileName[400]  ;
	sprintf( cList , "pList%d" , nList+1 ) ;
	if ( nList == 1 ) {
		fprintf( fCode , "\tpList1 = ring_list_new_gc(pRingState,0) ; \n"  ) ;
	}
	/* Write List Items */
	for ( x = 1 ; x <= ring_list_getsize(pList) ; x++ ) {
		/* Separate Code to different functions */
		if ( (lSeparate == 1) && (nList == 1) ) {
			if ( x % nItemsPerFunction == 0 ) {
				nFunction++ ;
				/* Call the new function */
				fprintf( fCode , "\tloadRingCode%d(pRingState,pList1);  \n",nFunction  ) ;
				/* End the current function */
				fprintf( fCode , "}\n"  ) ;
				/*
				**  Start New source file 
				**  Close the current file 
				*/
				fclose( fCode ) ;
				/* Create another source file */
				sprintf( cFileName , "ringappcode%d.c" , nFunction ) ;
				fCode = fopen(cFileName , "w+b" );
				fprintf( fCode , "#include \"ring.h\" \n\n"  ) ;
				fprintf( fCode , "#include \"ringappcode.h\" \n\n"  ) ;
				/* Start New Functions */
				fprintf( fCode , "void loadRingCode%d(RingState *pRingState,List *pList1) {\n",nFunction  ) ;
				fprintf( fCode , "\tList *pList2,*pList3,*pList4,*pList5,*pList6 ;\n"  ) ;
			}
		}
		pList2 = ring_list_getlist(pList,x);
		fprintf( fCode , "\tpList%d = ring_list_newlist_gc(pRingState,pList%d);\n" , nList+1,nList ) ;
		for ( x2 = 1 ; x2 <= ring_list_getsize(pList2) ; x2++ ) {
			if ( ring_list_isstring(pList2,x2) ) {
				fprintf( fCode , "\tring_list_addstring_gc(pRingState,%s,\"" , cList ) ;
				/* Add the string */
				cString = ring_list_getstring(pList2,x2) ;
				nMax = ring_list_getstringsize(pList2,x2) ;
				for ( x3 = 0 ; x3 < nMax ; x3++ ) {
					fprintf( fCode , "\\x%02x" , (unsigned char) cString[x3] ) ;
				}
				fprintf( fCode , "\"); \n"  ) ;
			}
			else if ( ring_list_isint(pList2,x2) ) {
				fprintf( fCode , "\tring_list_addint_gc(pRingState,%s," , cList ) ;
				fprintf( fCode , "%d" , ring_list_getint(pList2,x2) ) ;
				fprintf( fCode , "); \n"  ) ;
			}
			else if ( ring_list_isdouble(pList2,x2) ) {
				fprintf( fCode , "\tring_list_adddouble_gc(pRingState,%s," , cList ) ;
				fprintf( fCode , "%f" , ring_list_getdouble(pList2,x2) ) ;
				fprintf( fCode , "); \n"  ) ;
			}
			else if ( ring_list_ispointer(pList2,x2) ) {
				fprintf( fCode , "\tring_list_addpointer_gc(pRingState,%s,NULL);\n" , cList ) ;
			}
			else if ( ring_list_islist(pList2,x2) ) {
				fprintf( fCode , "\tpList%d = ring_list_newlist_gc(pRingState,pList%d);\n" , nList+2,nList+1 ) ;
				nFunction = ring_objfile_writelistcode(ring_list_getlist(pList2,x2) ,fCode,nList+2,lSeparate,nFunction,nItemsPerFunction);
			}
		}
	}
	return nFunction ;
}
