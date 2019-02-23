#ifndef SORT
#define SORT
#include<stdio.h>
#include<stdlib.h>
#include "Data.h"
#include "graph.h"
  
/* Link list node */
/*struct Node
{
    int data;
    struct Node* next;
};*/
 
/* function prototypes */
struct Node* SortedMerge(struct Node* a, struct Node* b);
struct Node* SortedMerge_2(struct Node* a, struct Node* b);
struct Node* SortedMerge_Dataset(struct Node* a, struct Node* b);
alert_list* SortedMerge_Alert(alert_list* a, alert_list* b);
void FrontBackSplit(struct Node* source,
          struct Node** frontRef, struct Node** backRef);
void FrontBackSplit_Alert(alert_list* source,
          alert_list** frontRef, alert_list** backRef);
 
/* sorts the linked list by changing next pointers (not data) */
void sort_fitness(struct Node** headRef) //MergeSort
{
  struct Node* head = *headRef;
  struct Node* a;
  struct Node* b;
 
  /* Base case -- length 0 or 1 */
  if ((head == NULL) || (head->next == NULL))
  {
    return;
  }
 
  /* Split head into 'a' and 'b' sublists */
  FrontBackSplit(head, &a, &b); 
 
  /* Recursively sort the sublists */
  sort_fitness(&a);
  sort_fitness(&b);
 
  /* answer = merge the two sorted lists together */
  *headRef = SortedMerge(a, b);
}

/* sorts the linked list by changing next pointers (not data) */
void sort_confirm(struct Node** headRef) //MergeSort
{
  struct Node* head = *headRef;
  struct Node* a;
  struct Node* b;
 
  /* Base case -- length 0 or 1 */
  if ((head == NULL) || (head->next == NULL))
  {
    return;
  }
 
  /* Split head into 'a' and 'b' sublists */
  FrontBackSplit(head, &a, &b); 
 
  /* Recursively sort the sublists */
  sort_confirm(&a);
  sort_confirm(&b);
 
  /* answer = merge the two sorted lists together */
  *headRef = SortedMerge_2(a, b);
}

void sort_dataset(struct Node** headRef) //MergeSort
{
  struct Node* head = *headRef;
  struct Node* a;
  struct Node* b;
 
  /* Base case -- length 0 or 1 */
  if ((head == NULL) || (head->next == NULL))
  {
    return;
  }
 
  /* Split head into 'a' and 'b' sublists */
  FrontBackSplit(head, &a, &b); 
 
  /* Recursively sort the sublists */
  sort_dataset(&a);
  sort_dataset(&b);
 
  /* answer = merge the two sorted lists together */
  *headRef = SortedMerge_Dataset(a, b);
}


/* sorts the linked list by changing next pointers (not data) */
void sort_alert(alert_list** headRef) //MergeSort
{
  alert_list* head = *headRef;
  alert_list* a;
  alert_list* b;
 
  /* Base case -- length 0 or 1 */
  if ((head == NULL) || (head->next == NULL))
  {
    return;
  }
 
  /* Split head into 'a' and 'b' sublists */
  FrontBackSplit_Alert(head, &a, &b); 
 
  /* Recursively sort the sublists */
  sort_alert(&a);
  sort_alert(&b);
 
  /* answer = merge the two sorted lists together */
  *headRef = SortedMerge_Alert(a, b);
}

 
/* See http://www.geeksforgeeks.org/?p=3622 for details of this 
   function */
struct Node* SortedMerge(struct Node* a, struct Node* b)
{
  struct Node* result = NULL;
 
  /* Base cases */
  if (a == NULL)
     return(b);
  else if (b==NULL)
     return(a);
 
  /* Pick either a or b, and recur */
  if (a->fitness <= b->fitness)
  {
     result = a;
     result->next = SortedMerge(a->next, b);
  }
  else
  {
     result = b;
     result->next = SortedMerge(a, b->next);
  }
  return(result);
}

struct Node* SortedMerge_2(struct Node* a, struct Node* b)
{
  struct Node* result = NULL;
 
  /* Base cases */
  if (a == NULL)
     return(b);
  else if (b==NULL)
     return(a);
 
  /* Pick either a or b, and recur */
  if (a->confirm <= b->confirm)
  {
     result = a;
     result->next = SortedMerge_2(a->next, b);
  }
  else
  {
     result = b;
     result->next = SortedMerge_2(a, b->next);
  }
  return(result);
}

struct Node* SortedMerge_Dataset(struct Node* a, struct Node* b)
{
  struct Node* result = NULL;
 
  /* Base cases */
  if (a == NULL)
     return(b);
  else if (b==NULL)
     return(a);
 
  /* Pick either a or b, and recur */
  if (a->seconds_elapsed <= b->seconds_elapsed)
  {
     result = a;
     result->next = SortedMerge_Dataset(a->next, b);
  }
  else
  {
     result = b;
     result->next = SortedMerge_Dataset(a, b->next);
  }
  return(result);
}

alert_list* SortedMerge_Alert(alert_list* a, alert_list* b)
{
  alert_list* result = NULL;
 
  /* Base cases */
  if (a == NULL)
     return(b);
  else if (b==NULL)
     return(a);
 
  /* Pick either a or b, and recur */
  if (a->a.seconds_elapsed <= b->a.seconds_elapsed)
  {
     result = a;
     result->next = SortedMerge_Alert(a->next, b);
  }
  else
  {
     result = b;
     result->next = SortedMerge_Alert(a, b->next);
  }
  return(result);
}
 
/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves,
     and return the two lists using the reference parameters.
     If the length is odd, the extra node should go in the front list.
     Uses the fast/slow pointer strategy.  */
void FrontBackSplit(struct Node* source,
          struct Node** frontRef, struct Node** backRef)
{
  struct Node* fast;
  struct Node* slow;
  if (source==NULL || source->next==NULL)
  {
    /* length < 2 cases */
    *frontRef = source;
    *backRef = NULL;
  }
  else
  {
    slow = source;
    fast = source->next;
 
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
      fast = fast->next;
      if (fast != NULL)
      {
        slow = slow->next;
        fast = fast->next;
      }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two
      at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
  }
}

void FrontBackSplit_Alert(alert_list* source,
          alert_list** frontRef, alert_list** backRef)
{
  alert_list* fast;
  alert_list* slow;
  if (source==NULL || source->next==NULL)
  {
    /* length < 2 cases */
    *frontRef = source;
    *backRef = NULL;
  }
  else
  {
    slow = source;
    fast = source->next;
 
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
      fast = fast->next;
      if (fast != NULL)
      {
        slow = slow->next;
        fast = fast->next;
      }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two
      at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
  }
}
 
#endif
