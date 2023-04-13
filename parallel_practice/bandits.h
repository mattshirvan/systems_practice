#ifndef _BANDIT_H_
#define _BANDIT_H_

#define MAX 10000000

typedef struct list
{
    double *values;
    int length;
}
list;

int argmax(double *Q, int k);
int e_greedy(double *Q, double epsilon, int k);
double bandit(int A);
list simple_bandit(double epsilon, int steps, int k);
list simulate(int runs, int steps, int k, double epsilon);
double mean(double *values, int length);
double stddev(double *values, int length);
double *normal_distribution(int length);
void printHistogram(double* values, int n);
void init(list *list);
void append(list *list, double value);
void pop(list *list);
void printlist(list *list);
void init_q(double *q, int k);
void del(list *l);
void remove_val(list *list, double value);
double sum(list values);

// Represents a node
// typedef struct node
// {
//     double number;
//     struct node *next;
// }
// node;

// typedef struct Array
// {
//     int count;
//     int capacity;
//     double *values;
//     void (*add)(const struct Array*);
//     void (*resize)(const struct Array*);
//     void (*print)(const struct Array*);
//     void (*del)(const struct Array*);
//     void (*insert)(const struct Array*);
//     void (*add_first)(const struct Array*);
//     void (*add_last)(const struct Array*);
//     void (*del_last)(const struct Array*);
// } Array;



#endif