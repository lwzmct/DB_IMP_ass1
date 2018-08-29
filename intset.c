/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct intSet *List;
List newIntSet(void);
void bubble_sort(int list[], int n);
int N_inarray(int Array[]);

PG_MODULE_MAGIC;

//typedef struct Complex
//{
//	double		x;
//	double		y;
//}			Complex;

typedef struct intSet{
  int intArray[128];
  int nb_of_node;
} intSet;
int check_dup(intSet *set,int num);
/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/


//my input output
PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);

	char *temp_nb=palloc(6*sizeof(char));
	int flag=0; //meet a number become 1 
	int nb_of_comma = 0;
	int len_of_int = 0;
	int i=0;
	//int cur_node = 0;
	int index = 0;
	intSet *result = palloc(sizeof(intSet));
	//result->intArray=palloc(128*sizeof(int));
	for(i=0;i<6;i++){
	    temp_nb[i]='\0';
	}
	//check position 0
	if(str[0] != '{' || str[strlen(str)-1]!='}')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input 1 syntax for intSet: \"%s\"",
						str)));

	for(i=1;i<strlen(str)-1;i++){
		//input error
		if(!isdigit(str[i]) && str[i]!=' ' && str[i]!=',')
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input 2 syntax for intSet: \"%s\"",
						str)));
		//Discontinuous number
		if(flag == 1 && str[i] == ' ')
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input 3 syntax for intSet: \"%s\"",
						str)));
		//check full or not
		//if(len_of_int == ((int)(sizeof(result->intArray) / 4))){
			//result->intArray = (int *)repalloc(result->intArray,
				//sizeof(result->intArray) + 8);
		//}
		//record number of comma and storage
		if (str[i] == ',' && temp_nb[0] != '\0'){
			nb_of_comma ++;
			//store here
			//First storage
			if(result->nb_of_node == 0){
				result->intArray[0] = atoi(temp_nb);
				result->nb_of_node ++ ;
				index++;
				//clean temp_nb
				temp_nb[0] = '\0';
				len_of_int = 0;
			}else if(check_dup(result, atoi(temp_nb)) == 0){
				temp_nb[0] = '\0';
                len_of_int = 0;
			}else{
				result->intArray[index] = atoi(temp_nb);
				index ++;
				result->nb_of_node ++;
				temp_nb[0] = '\0';
				len_of_int = 0;
			}			
		}else if (str[i] == ',' && temp_nb[0] == '\0'){
			ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input 4 syntax for intSet: \"%s\"", str)));
		}
		//store the integers to temp char*
		if(isdigit(str[i])){
			if(len_of_int == strlen(temp_nb)){
				temp_nb = (char *)repalloc(temp_nb, strlen(temp_nb) + 2);
			}
			//append integer to temp
			sprintf(temp_nb,"%s%c",temp_nb,str[i]);
			//temp_nb[len_of_int] = str[i];
			len_of_int ++;
			flag = 1;
		}
	}//for loop end
	//final storage
	if(str[strlen(str)-1]=='}' && temp_nb[0] != '\0'){
		result->intArray[index] = atoi(temp_nb);
		result->nb_of_node ++;
		temp_nb[0] = '\0';
	}
	//check comma
	//if(N_inarray(result->intArray)!= nb_of_comma+1)
		//ereport(ERROR,
				//(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 //errmsg("5.invalid  input syntax for intSet: \"%s\"", str)));
	PG_RETURN_POINTER(result);
	//has error when first insert is 4 comma the next insert can not be 
}


int check_dup(intSet *set,int num){
	int i;
	for(i=0;i<set->nb_of_node;i++){
		if(num==set->intArray[i])
			return 0;
	}
	return 1;
}




//out function
PG_FUNCTION_INFO_V1(intset_out);

Datum
intset_out(PG_FUNCTION_ARGS)
{
    intSet    *set = (intSet *) PG_GETARG_POINTER(0);
	char	   *result;
	int i=0;
	int j=0;
    i = N_inarray(set->intArray);
	result=psprintf("{%d,",set->intArray[0]);
    for(j=1;j<i-1;j++){
        result=psprintf("%s%d,",result,set->intArray[j]);
    }
    result=psprintf("%s%d}",result,set->intArray[i-1]);
	PG_RETURN_CSTRING(result);
	
}
void bubble_sort(int list[], int n)
{
  int c, d, t;
 
  for (c = 0 ; c < n - 1; c++)
  {
    for (d = 0 ; d < n - c - 1; d++)
    {
      if (list[d] > list[d+1])
      {
        /* Swapping */
 
        t         = list[d];
        list[d]   = list[d+1];
        list[d+1] = t;
      }
    }
  }
}
int N_inarray(int Array[]){
    int i=0;
    while(Array[i]!='\0'){
        i++;
    }
    return i;
}
