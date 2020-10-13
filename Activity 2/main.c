/*  Actividad individual 2: Apuntadores a funciones
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autor: Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 27-8-2020
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *title;
    int pages;
} book;

typedef void (*t_print)(void *);
typedef void (*t_free)(void *);
typedef void (*t_reverseSort)(void *, void *, size_t, int);
typedef void (*t_bidirectionalSort)(void *, void *, size_t, int);
typedef void (*t_sort)(void *, void *, size_t, int);
typedef void (*t_iterator)(void *, size_t, int, t_print, t_free, t_sort);

void printInt(void *curr) {
    printf("\t%d", *(int *)curr);
}

void printBook(void *curr) {
    printf("\t%s (%d pp.)", ((book *)curr)->title, ((book *)curr)->pages);
}

void printItem(void *curr, t_print tp) {
    (*tp)(curr);
}

void freeInt(void *curr) {
    *(int *)curr = 0;
}

void freeBook(void *curr) {
    ((book *)curr)->title = NULL;
    ((book *)curr)->pages = 0;
    free(((book *)curr)->title);
}

void freeItem(void *curr, t_free tf) {
    (*tf)(curr);
}

void printCollection(void *col, size_t size, int n, t_print tp) {
    void *curr, *last = col + size * n;
    for (curr = col; curr < last; curr += size) {
        printItem(curr, tp);
    }
}

void reverseSortInt(void *col, void *reverse, size_t size, int n) {
    void *curr = col + size * n - size;
    void *last = col;
    void *currR = reverse;
    for (; curr >= last; curr -= size) {
        *(int *)currR = *(int *)curr;
        currR += size;
    }
}

void reverseSortBook(void *col, void *reverse, size_t size, int n) {
    void *curr = col + size * n - size;
    void *last = col;
    void *currR = reverse;
    for (; curr >= last; curr -= size) {
        *(book *)currR = *(book *)curr;
        currR += size;
    }
}

void reverseSort(void *col, void *reverse, size_t size, int n, t_reverseSort trs) {
    (*trs)(col, reverse, size, n);
}

void bidirectionalSortInt(void *col, void *bidirectional, size_t size, int n) {
    void *curr = col;
    void *last = col + size * n - size;
    void *currB = bidirectional;
    while (curr <= last) {
        if (curr == last) {
            *(int *)currB = *(int *)curr;
            break;
        } else {
            *(int *)currB = *(int *)curr;
            currB += size;
            *(int *)currB = *(int *)last;
            currB += size;
            curr += size;
            last -= size;
        }
    }
}

void bidirectionalSortBook(void *col, void *bidirectional, size_t size, int n) {
    void *curr = col;
    void *last = col + size * n - size;
    void *currB = bidirectional;
    while (curr <= last) {
        if (curr == last) {
            *(book *)currB = *(book *)curr;
            break;
        } else {
            *(book *)currB = *(book *)curr;
            currB += size;
            *(book *)currB = *(book *)last;
            currB += size;
            curr += size;
            last -= size;
        }
    }
}

void bidirectionalSort(void *col, void *bidirectional, size_t size, int n, t_bidirectionalSort tbs) {
    (*tbs)(col, bidirectional, size, n);
}

void sort(void *col1, void *col2, size_t size, int n, t_sort ts) {
    (*ts)(col1, col2, size, n);
}

int controller(void *col, size_t size, int n, void **curr, void *last, t_print tp) {
    printf("\nCollection:\n\n");
    printCollection(col, size, n, tp);
    printf("\n\nCurrent item:");
    printItem(*curr, tp);
    printf("\n\n0. Back to iterators\n1. Start\n2. End\n3. Previous\n4. Next\n\nChoose an option: ");
    int option;
    scanf("%d", &option);
    switch (option) {
        case 0:
            return 0;
        case 1:
            *curr = col;
            break;
        case 2:
            *curr = col + size * n - size;
            break;
        case 3:
            if (*curr != col) {
                *curr -= size;
            } else {
                printf("\nNULL\n");
            }
            break;
        case 4:
            if (*curr != last) {
                *curr += size;
            } else {
                printf("\nNULL\n");
            }
            break;
        default:
            printf("\nPlease enter a valid option number.\n");
            break;
    }
    return 1;
}

void forwardIterator(void *col, size_t size, int n, t_print tp, t_free tf, t_sort ts) {
    void *curr = col;
    void *last = col + size * n - size;
    int option = 1;
    while (option != 0) {
        printf("\nForward Iterator\n----------------\n");
        option = controller(col, size, n, &curr, last, tp);
    }
}

void reverseIterator(void *col, size_t size, int n, t_print tp, t_free tf, t_sort ts) {
    void *reverse = (void *) malloc(sizeof(col));
    sort(col, reverse, size, n, ts);
    void *curr = reverse;
    void *last = reverse + size * n - size;
    int option = 1;
    while (option != 0) {
        printf("\nReverse Iterator\n----------------\n");
        option = controller(reverse, size, n, &curr, last, tp);
    }
    curr = reverse;
    for (; curr <= last; curr += size) {
        freeItem(curr, tf);
    }
    free(reverse);
}

void bidirectionalIterator(void *col, size_t size, int n, t_print tp, t_free tf, t_sort ts) {
    void *bidirectional = (void *) malloc(sizeof(col));
    sort(col, bidirectional, size, n, ts);
    void *curr = bidirectional;
    void *last = bidirectional + size * n - size;
    int option = 1;
    while (option != 0) {
        printf("\nBidirectional Iterator\n----------------------\n");
        option = controller(bidirectional, size, n, &curr, last, tp);
    }
    curr = bidirectional;
    for (; curr <= last; curr += size) {
        freeItem(curr, tf);
    }
    free(bidirectional);
}

// Función "recorre"
void iterate(void *col, size_t size, int n, t_print tp, t_free tf, t_sort ts, t_iterator ti) {
    (*ti)(col, size, n, tp, tf, ts);
}

void iterators(void *col, size_t size, int n, t_print tp, t_free tf, t_reverseSort trs, t_bidirectionalSort tbs) {
    while (1) {
        printf("\nIterators\n---------\n\nCollection:\n\n");
        printCollection(col, size, n, tp);
        printf("\n\n0. Back to menu\n1. Forward iterator\n2. Reverse iterator\n3. Bidirectional iterator\n\nChoose an option: ");
        void *curr = col;
        void *last = col + n;
        int option;
        scanf("%d", &option);
        switch (option) {
            case 0:
                return;
            case 1:
                iterate(col, size, n, tp, tf, trs, &forwardIterator);
                break;
            case 2:
                iterate(col, size, n, tp, tf, trs, &reverseIterator);
                break;
            case 3:
                iterate(col, size, n, tp, tf, tbs, &bidirectionalIterator);
                break;
            default:
                printf("\nPlease enter a valid option number.\n");
                break;
        }
    }
}

void numbers(int n) {
    size_t size = sizeof(int);
    int *col = (int *) malloc(size * n);
    int *curr = col;
    int *last = col + n;
    for (; curr < last; curr++) {
        *curr = rand() % 100;
    }
    iterators(col, size, n, &printInt, &freeInt, &reverseSortInt, &bidirectionalSortInt);
    curr = col;
    for (; curr < last; curr++) {
        freeItem(curr, &freeInt);
    }
    free(col);
}

void books(int n) {
    size_t size = sizeof(book);
    book *col = (book *) malloc(size * n + size);
    book *curr = col;
    book *last = col + n;
    for (; curr < last; curr++) {
        curr->title = (char *) malloc(sizeof(char) * 30);
        printf("\nTitle: ");
        scanf(" %[^\n]", curr->title);
        printf("Number of pages: ");
        scanf("%d", &curr->pages);
    }
    iterators(col, size, n, &printBook, &freeBook, &reverseSortBook, &bidirectionalSortBook);
    curr = col;
    for (; curr < last; curr++) {
        freeItem(curr, &freeBook);
    }
    free(col);
}

void menu() {
    while (1) {
        printf("\nMenu\n----\n\n0. Exit program\n1. Numbers\n2. Books\n\nChoose an option: ");
        int option;
        scanf("%d", &option);
        switch (option) {
            case 0:
                exit(0);
            case 1:
                numbers(5);
                break; 
            case 2:
                books(3);
                break;
            default:
                printf("\nPlease enter a valid option number.\n");
                break;
        }
    }
}

int main(int argc, char const *argv[]) {
    menu();
    return 0;
}
