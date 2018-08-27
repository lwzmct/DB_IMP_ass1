#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
#include <string.h>

PG_MODULE_MAGIC;

// typedef struct intSet *List;
// List newIntSet();

// typedef struct node {
//   int data;
//   struct node *next;
// }NodeT;

// List newIntSet() {
//   intSet *newSet = NULL;
//   newSet = malloc(sizeof(intSet));
//   newSet -> head = malloc(sizeof(NodeT));
//   //malloc fail
//   if (newSet -> head == NULL){
//     return NULL;
//   }
//   //empty list
//   newSet -> nb_of_node = 0;
//   newSet -> head -> data = 0;
//   newSet -> head -> next = NULL;
  
//   return newSet;
// };

typedef struct intSet{
  int intArray[128];
  int nb_of_node = 0;
} intSet;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/


//my input output
PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	intSet *result = palloc(sizeof(intSet));
	char[6] temp_nb;
	int flag=0; //meet a number become 1 
	int nb_of_comma = 0;
	int len_of_int = 0;
	int i=0;
	int cur_node = 0;
	int index = 0;

	//check position 0
	if(str[0] != '{' || str[strlen(str)-1]!='}')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intSet: \"%s\"",
						str)));

	for(i=1;i<strlen(str)-1;i++){
		//input error
		if(!isdigit[str[i]] && str[i]!=' ' && sti[i]!=',')
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intSet: \"%s\"",
						str)));
		//Discontinuous number
		if(flag == 1 && str[i] == ' ')
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intSet: \"%s\"",
						str)));
		//check full or not
		if(len_of_int == ( (int)(sizeof(result->intArray) / sizeof(int)) ){
			result->intArray = (int *)repalloc(result->intArray,
				sizeof(result->intArray) + 8);
		}
		//record number of comma and storage
		if (str[i] == ',' && temp_nb[0] != '\0'){
			nb_of_comma += 1;
			//store here
			//First storage
			if(result->nb_of_node == 0){
				result->intArray[0] = atoi(temp_nb);
				result->nb_of_node += 1;
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
				result->nb_of_node += 1;
				temp_nb[0] = '\0';
				len_of_int = 0;
			}			
		}else if (str[i] == ',' && temp_nb[0] == '\0'){
			ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intSet: \"%s\"", str)));
		}
		//store the integers to temp char*
		if(isdigit[str[i]]){
			if(len_of_int == strlen(temp_nb)){
				temp_nb = (char *)realloc(temp_nb, strlen(temp_nb) + 2);
			}
			//append integer to temp
			temp_nb = strcat(temp_nb, str[i]);
			//temp_nb[len_of_int] = str[i];
			len_of_int += 1;
			flag = 1;
		}
	}//for loop end
	//final storage
	if(str[strlen(str)-1]=='}' && temp_nb[0] != '\0'){
		result->intArray[index] = atoi(temp_nb);
		result->nb_of_node += 1;
		temp_nb[0] = '\0';
	}
	//check comma
	if(result->nb_of_node != nb_of_comma + 1)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intSet: \"%s\"", str)));
	PG_RETURN_POINTER(result);
}


int check_dup(intSet *set,int num){
	int i;
	for(i=0;i<set->nb_of_node;i++){
		if(num==set->array[i])
			return 0;
	}
	return 1;
}

//out function
PG_FUNCTION_INFO_V1(intSet_out);

Datum
intSet_out(PG_FUNCTION_ARGS)
{
	char	   *result;
	int 		i=0;
	intSet *set=(intSet *) PG_GETARG_POINTER(0);
	Node *n=set->head;
	result=(char *) palloc(sizeof(intSet));
	while(n!=NULL)
	{
		if(n==set->Head){
			i+=sprintf(result+i,"{%d",n->data);
			n=n->next;
			continue;
			
		}
		else if(n->next==NULL){
			sprintf(result+i,"%d}",n->data);
			break;
		}
		i+=sprintf(result+i,"%d,",n->data);
		n=n->next;
	}//end while loop	
	PG_RETURN_CSTRING(result);
}

