/********************************************************************
CSL 316 - Assignment I - Semester 6th S24

Progammer: Anushka Yadav 
Enrollment No. : BT21CSE127
Section:   B 
Date Due:  24th Jan, 2024

Purpose:   When a program requests requests a block of memory dynamically,
           the system must allocate memory, and when a program terminates or frees 
           memory previously requested, the system must deallocate memory. 
*********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include<sstream>
#include <unordered_map>
#include<math.h>
using namespace std;
#define array_size 125



typedef struct node
{
    int size;
    struct node* next;
    int start_address;
    int reference_count;
}node;

void initialise_free(node* free_list)
{
    free_list->size=array_size*4;
    free_list-> reference_count=0;
    free_list->start_address = 0;
    free_list->next=NULL;
}

//function to display the the updated lists in the output file 

void display(node* list, const string& filename)
{
   ofstream outputFile(filename);

    if (!outputFile.is_open()) 
        {
            cerr << "Error opening output file." << endl;
            return;
        }

    node* ptr = list;
         

    while (ptr != NULL) 
        {
            outputFile << "Start address: " << ptr->start_address << endl;
            outputFile << "Size: " << ptr->size << endl;
            outputFile << "Reference count: " << ptr-> reference_count << endl;
            outputFile << endl;

            ptr = ptr->next;
        }

    // Close the output file stream
    outputFile.close();
     if (outputFile.fail())
        {
            cerr << "Error closing output file." << endl;
        }
}

// this function is to update the allocated list by introducing a newly allocated node 
void update_allocated(node* allocated_list , node* ptr1 )
{
    //insert the node;
        node* temp ;

        if(allocated_list->size==0)
            {
                allocated_list->size=ptr1->size;
                allocated_list->start_address=ptr1->start_address;
                allocated_list-> reference_count = ptr1-> reference_count;
                allocated_list->next = NULL;
            }
        else
            {
                temp = allocated_list;
                while(temp->next!=  NULL)
                    {
                        temp = temp->next;
                    }
                temp->next = ptr1;
            }
}

//this function performs compaction and coalaescing when fragmentation occurs 
void compaction (node** free_list , node** allocated_list)
{
    node* fptr;
    fptr= *free_list;
    int size =0;
    while(fptr!=NULL)
        {
            size+= fptr->size;
            fptr=fptr->next;
        }

    node* ptr ;
    ptr->size = size;
    ptr->start_address =0;
    ptr-> reference_count=0;
    ptr->next = NULL;
    *free_list = ptr;

    int next_add = ceil((double)size/4);
    node* aptr;
    aptr = *allocated_list;

    while(aptr!=NULL)
        {
            aptr->start_address = next_add;
            next_add = next_add + ceil((double)aptr->size/4);
            aptr=aptr->next;
        }
}

//this function reduces the siee of the free list and in turn also updates both the linked lists 
int allocate(int required_size, node** free_list, node** allocated_list)
{
    int flag =0;
    node* ptr ; 
    ptr= *free_list;

    node* ptr1 ;
    node* prev;
    ptr1 = new node; 
    while(ptr!=NULL && flag==0)
        {
            if(ptr->size >= required_size)
                {
                    ptr1->size=required_size;
                        ptr1-> reference_count=1;
                        ptr1->start_address=ptr->start_address;
                        ptr1->next=NULL;

                        update_allocated (*allocated_list, ptr1); //updating the allocated list

                        ptr->size=ptr->size-required_size;
                        ptr->start_address= ptr->start_address +ceil((double)required_size/4);
                        flag=1;

                }
            else
                {
                    prev = ptr;
                    ptr= ptr->next;
                }
        }

    if(flag==0)
        {
            compaction(free_list, allocated_list);
            return -1;
        }
    else 
        {
                return ptr1->start_address;
        }
    }

//this function is called when free is read, it deallocates the specified memory referece only if reference becomes 0
void deallocate(int address , node** allocated_list, node** free_list)
{
    node* temp, *prev;
    temp = *allocated_list;
    int flag=0;
    prev= NULL;
    while(flag==0)
        {
            if(temp->start_address==address && temp-> reference_count==1)
                {
                    if(prev==NULL)
                        {
                            *allocated_list=temp->next;
                        }
                    else
                        {
                            prev->next = temp->next;
                        }
                    flag=1;
                    node* freed;
                    freed= new node;
                    freed->next = *free_list;
                    *free_list = freed;
                    freed->start_address = temp ->start_address;
                    freed-> reference_count = 0;
                    freed->size = temp->size;
                }
            else if (temp->start_address==address && temp-> reference_count>1)
                {
                    temp-> reference_count--;
                    flag =1;
                }
            else
                {
                    prev = temp;
                    temp = temp->next;
                }
        }


}

//this function is to increase the reference of the specified node 
int increase_reference(  int a, node* allocated_list)
{
    node* ptr = allocated_list;
    while(ptr->start_address!=a)
    {ptr=ptr->next;}

    ptr-> reference_count++;
    return a;

}



int main()
{
    int arr[array_size];

    node* free_list;
    free_list = new node;
    node* allocated_list;
    allocated_list = new node;
    allocated_list->size =0;
    allocated_list->next=   NULL;

    initialise_free(free_list);

   ifstream inputFile("input.txt");
    
    if (!inputFile.is_open())
        {
        cerr << "Error opening input file." << endl;
            return 1;
        }

    // Process commands from the file
    string command;
    unordered_map< string, int> variable_map;
     string line;
    int  i=0 ;
    while ( getline(inputFile, line))
     {
         stringstream ss(line);
        
        while( ss >> command)
        {
            if(command == "free")
                {
                    ss>> command;
                    ss>> command ; 
                deallocate(variable_map[command],&allocated_list,&free_list);
                
                }
            else if (command == ")" || command == ";")
                {
                    continue;
                }
            else 
                {
                    string variable_1, variable_2;
                    int size ; 
                    variable_1 = command;

                    ss>>command;
                    ss>>command;
                    if(command == "allocate")
                        {
                            ss>>command;
                            ss>>command;
                            size = stoi(command) ;
                            variable_map[variable_1]= allocate(size, &free_list,&allocated_list);
                            if (variable_map[variable_1]==-1)
                                {
                                    variable_map[variable_1]= allocate(size, &free_list,&allocated_list);
                                }
                            if (variable_map[variable_1]==-1)
                                {
                                    cout<< "ERROR - Memory full "<<endl ;
                                }
                        }
                    else
                        {
                            variable_2 = command;
                            
                            variable_map[command]= increase_reference(variable_map[variable_1], allocated_list);
                        }
                }
        }

    }
 inputFile.close();

display (free_list , "free.txt" );
display(allocated_list, "allocated.txt");


 return 0;
    
}
