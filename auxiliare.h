#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hierarchy.h"

#define MAX_STRING 15

// Returneaza nodul cu numele primit ca argument
Tree find_manager(Tree tree, char *manager_name) {
    if (tree == NULL) {
        return NULL;
    }

    Tree manager = NULL;

    if (strcmp(tree->name, manager_name) == 0) {
        manager = tree;
        return manager;
    }

    if (tree->team != NULL) {
        for (int i = 0; i < tree->direct_employees_no; i++) {
            manager = find_manager(tree->team[i], manager_name);
            if (manager != NULL) {
                return manager;
            }
        }
    }
    return manager;
}

// Schimba pozitia unui angajat in ierarhie (il muta in echipa managerului cu numele primit ca argument)
Tree rehire_employee(Tree tree, Tree *employee, char *manager_name) {

    Tree manager = find_manager(tree, manager_name);
    (*employee)->manager = manager;

    // Actualizez vectorul team
    manager->direct_employees_no++;

    if (manager->direct_employees_no == 1) { // Daca e primul lui angajat
        manager->team = (Tree *) malloc(sizeof(Tree));
        manager->team[0] = *employee;
        
    } else {
        manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));

        // Iterez vectorul team de la sfarsit la inceput pentru a gasi pozitia unde trebuie adaugat
        int i;
        for (i = manager->direct_employees_no - 2; i >= 0; i--) {
            if (strcmp(manager->team[i]->name, (*employee)->name) < 0) {
                manager->team[i + 1] = *employee;
                break;
            } else {
                manager->team[i + 1] = manager->team[i];
            }
        }
        if (i == -1) {  // Daca am terminat vectorul si nu am adaugat inca angajatul, inseamna ca trebuie pus la inceput
            manager->team[0] = *employee;
        }
    }
    return tree;
}


// Lista simplu inlantuita, sortata
typedef struct Node {
    char *name;
    struct Node *next;
} TNode, *TList;

TList add_toList(TList list, char *name) {
    // Creez noul nod
    TList new_node = malloc(sizeof(TNode));
    new_node->name = malloc(MAX_STRING);
    strcpy(new_node->name, name);
    new_node->next = NULL;

    if (list == NULL) {
        return new_node; // Este primul nod

    } else {
        // Trebuie inserat la inceput
        if (strcmp(name, list->name) < 0) {
            new_node->next = list;
            return new_node;
        }

        // Trebuie adaugat in interior
        TList prev = list;
        TList curr = list->next;
        while (curr != NULL) {
            if (strcmp(name, curr->name) < 0) {
                new_node->next = curr;
                prev->next = new_node;
                return list;
            }
            prev = curr;
            curr = curr->next;
        }

        // Daca s-a ajuns la sfarsit
        prev->next = new_node;
        return list;
    }

    return list;
}

void free_TList (TList list) {
    while (list != NULL) {
        free(list->name);
        TList aux = list;
        list = list->next;
        free(aux);
    }
    return;
}


// -----------------------------------------
// ---- Pentru get_employees_by_manager ----
// -----------------------------------------
TList build_TList(TList *employees_list, Tree manager) {
    *employees_list = add_toList(*employees_list, manager->name);

    if (manager->team != NULL) {
        for (int i = 0; i < manager->direct_employees_no; i++) {
            *employees_list = build_TList(employees_list, manager->team[i]);
        }
    }

    return *employees_list;
}

// -----------------------------------------
// ----- Pentru get_employees_by_level -----
// -----------------------------------------
// Construieste lista cu angajatii de pe un anumit level (X_lvl)
void print_X_level(Tree tree, TList *list, int current_lvl, int X_lvl) {
    if (tree == NULL) {
        return;
    }
    // Daca nivelul curent coincide cu nivelul cerut, retin numele angajatului in lista ordonata alfabetic
    if (current_lvl == X_lvl) {
        *list = add_toList(*list, tree->name);  
        return;
    }
    // Avansez pe urmatorul nivel (se face recursiv, retin mereu pe ce nivel ma aflu in argumentul functiei)
    if (tree->team != NULL) {
        for (int i = 0; i < tree->direct_employees_no; i++) {
            print_X_level(tree->team[i], list, current_lvl + 1, X_lvl);
        }
    }
    return;
}

// -----------------------------------------
// ------- Pentru find_best_manager --------
// -----------------------------------------
// Construieste lista cu managerii cu cea mai mare echipa (daca sunt mai multi), recursiv (folosesc acumulatorul biggest_team)
void find_biggest_team(Tree tree, TList *best_managers_list, int *biggest_team) {
    if (tree == NULL) {
        return;
    }
    // Daca gasesc un manager cu echipa mai mare, golesc lista, construiesc lista noua si actualizez acumulatorul
    if (tree->direct_employees_no > *biggest_team) {
        free_TList(*best_managers_list);
        *best_managers_list = NULL;

        *best_managers_list = add_toList(*best_managers_list, tree->name);

        *biggest_team = tree->direct_employees_no;

    // Daca gasesc un manager cu echipa la fel de mare, il adaug in lista curenta
    } else if (tree->direct_employees_no == *biggest_team) {
        *best_managers_list = add_toList(*best_managers_list, tree->name);
    }

    // Chem functia pentru fiecare membru din echipa managerului curent
    if (tree->team != NULL) {
        for (int i = 0; i < tree->direct_employees_no; i++) {
            find_biggest_team(tree->team[i], best_managers_list, biggest_team);
        }
    }
    return;
}