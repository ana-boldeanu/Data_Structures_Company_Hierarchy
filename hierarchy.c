// Boldeanu Ana-Maria
// 311 CD
// SD Tema 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxiliare.h"

#define MAX_STRING 15 // Nr maxim de bytes pe care il poate ocupa un nume

/* Adauga un angajat nou in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 * manager_name: numele sefului noului angajat
 *
 * return: intoarce ierarhia modificata. Daca tree si manager_name sunt NULL, 
           atunci employee_name e primul om din ierarhie.
 */
Tree hire(Tree tree, char *employee_name, char *manager_name) {
    // Creez noul nod cu proprietatile angajatului
    Tree new_employee = (Tree) malloc(sizeof(TreeNode));

    new_employee->name = (char *) malloc(MAX_STRING);
    strcpy(new_employee->name, employee_name);

    new_employee->direct_employees_no = 0;
    new_employee->team = NULL;
    new_employee->manager = NULL;

    // Caut nodul (manager) in echipa caruia trebuie sa il adaug
    if (manager_name == NULL) {
        tree = new_employee;    // E primul om

    } else {
        Tree manager = find_manager(tree, manager_name);
        new_employee->manager = manager;

        // Actualizez vectorul team
        manager->direct_employees_no++;

        if (manager->direct_employees_no == 1) { // Daca e primul lui angajat
            manager->team = (Tree *) malloc(sizeof(Tree));
            manager->team[0] = new_employee;
            
        } else {
            manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));

            // Iterez vectorul team de la sfarsit la inceput pentru a gasi pozitia unde trebuie adaugat
            int i;
            for (i = manager->direct_employees_no - 2; i >= 0; i--) {
                if (strcmp(manager->team[i]->name, employee_name) < 0) {
                    manager->team[i + 1] = new_employee;
                    break;
                } else {
                    manager->team[i + 1] = manager->team[i];
                }
            }
            if (i == -1) {  // Daca am terminat vectorul si nu am adaugat inca angajatul, inseamna ca trebuie pus la inceput
                manager->team[0] = new_employee;
            }
        }
    }
    return tree;
}

/* Sterge un angajat din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului concediat
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire(Tree tree, char *employee_name) {
    if (strcmp(tree->name, employee_name) == 0) {
        return tree;
    }

    Tree employee = find_manager(tree, employee_name); // Determin nodul corespunzator angajatului ce trebuie concediat

    if (employee != NULL) {
        Tree manager = employee->manager; // Managerul celui concediat

        // Il elimin intai din echipa managerului sau
        for (int i = 0; i < manager->direct_employees_no; i++) {
            if (strcmp(manager->team[i]->name, employee_name) == 0) {
                for (int k = i + 1; k < manager->direct_employees_no; k++) {
                    manager->team[k - 1] = manager->team[k];
                }
                manager->direct_employees_no--;
                manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));
                break;
            }
        }

        // Ii "re-angajez" echipa in echipa managerului
        if (employee->team != NULL) {
            for (int i = 0; i < employee->direct_employees_no; i++) {
                manager = rehire_employee(manager, &(employee->team[i]), manager->name);
            }
        }

        free(employee->name);
        free(employee->team);
        free(employee);
    }
    
    return tree;
}

/* Promoveaza un angajat in ierarhie. Verifica faptul ca angajatul e cel putin 
 * pe nivelul 2 pentru a putea efectua operatia.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 *
 * return: intoarce ierarhia modificata.
 */
Tree promote(Tree tree, char *employee_name) {
    if (tree == NULL) {
        return tree;    // Daca tree e gol
    }

    Tree employee = find_manager(tree, employee_name); // Determin nodul corespunzator angajatului ce trebuie promovat

    if (employee == NULL || employee->manager == NULL || employee->manager->manager == NULL) {
        return tree;    // Daca employee nu se afla pe cel putin nivelul 2 (sau nu exista)
    }

    char *new_manager_name = malloc(MAX_STRING * sizeof(char)); // Managerul managerului sau
    strcpy(new_manager_name, employee->manager->manager->name);

    tree = fire(tree, employee_name); // Daca are echipa, ea va fi automat mutata cu un nivel mai sus
    tree = hire(tree, employee_name, new_manager_name); // Il "re-angajez" cu un nivel mai sus
    
    free(new_manager_name);

    return tree;
}

/* Muta un angajat in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_employee(Tree tree, char *employee_name, char *new_manager_name) {
    if (tree == NULL || strcmp(tree->name, employee_name) == 0) { // Daca tree e gol sau se incearca mutarea primului om
        return tree;
    }

    Tree employee = find_manager(tree, employee_name);
    Tree new_manager = find_manager(tree, new_manager_name);

    if (employee == NULL || new_manager == NULL) {  // Daca nu exista angajatul sau managerul
        return tree;
    }

    if (strcmp(employee->manager->name, new_manager_name) == 0) { // Daca se incearca mutarea unui angajat in aceeasi echipa
        return tree;
    }

    tree = fire(tree, employee_name); // Daca are echipa, ea va fi automat mutata cu un nivel mai sus
    tree = hire(tree, employee_name, new_manager_name); // Il "re-angajez" cu un nivel mai sus

    return tree;   
}

/* Muta o echipa in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei mutate
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_team(Tree tree, char *employee_name, char *new_manager_name) {
    if (tree == NULL || strcmp(tree->name, employee_name) == 0) { // Daca tree e gol sau se incearca mutarea primului om
        return tree;
    }

    Tree employee = find_manager(tree, employee_name);
    Tree new_manager = find_manager(tree, new_manager_name);

    if (employee == NULL || new_manager == NULL) {  // Daca nu exista angajatul sau managerul
        return tree;
    }

    Tree manager = employee->manager; // Managerul celui mutat

    // Il elimin intai din echipa managerului sau
    for (int i = 0; i < manager->direct_employees_no; i++) {
        if (strcmp(manager->team[i]->name, employee_name) == 0) {
            for (int k = i + 1; k < manager->direct_employees_no; k++) {
                manager->team[k - 1] = manager->team[k];
            }
            manager->direct_employees_no--;
            manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));
            break;
        }
    }

    // Il "re-angajez" in echipa managerului nou
    new_manager = rehire_employee(new_manager, &employee, new_manager_name);

    return tree;
}

/* Concediaza o echipa din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei concediate
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire_team(Tree tree, char *employee_name) {
    if (tree == NULL || strcmp(tree->name, employee_name) == 0) { // Daca tree e gol sau se incearca concedierea primului om
        return tree;
    }

    Tree employee = find_manager(tree, employee_name); // Determin angajatul ce trebuie concediat

    if (employee == NULL) {  // Daca nu exista angajatul
        return tree;
    }

    Tree manager = employee->manager; // Managerul celui mutat

    // Il elimin din echipa managerului sau
    if (manager->direct_employees_no == 1) { // Cazul in care era singurul angajat
        manager->direct_employees_no = 0;
        free(manager->team);
        manager->team = NULL;

    } else {    // Caz general
        for (int i = 0; i < manager->direct_employees_no; i++) {
            if (strcmp(manager->team[i]->name, employee_name) == 0) {
                for (int k = i + 1; k < manager->direct_employees_no; k++) {
                    manager->team[k - 1] = manager->team[k];
                }
                manager->direct_employees_no--;
                manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));
                break;
            }
        }
    }

    // Free-uri pentru echipa concediata
    if (employee->team != NULL) {
        for (int i = 0; i < employee->direct_employees_no; i++) {
            destroy_tree(employee->team[i]);
        }
    }
    free(employee->name);
    free(employee->team);
    free(employee);

    return tree;
}

/* Afiseaza toti angajatii sub conducerea unui angajat.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei
 */
void get_employees_by_manager(FILE *f, Tree tree, char *employee_name) {
    Tree manager = find_manager(tree, employee_name); // Determin managerul echipei de afisat

    if (manager == NULL) {  // Daca nu exista managerul
        fprintf(f, "\n");
        return;
    }

    TList employees_list = NULL; // Lista folosita pentru pastrarea angajatilor in ordine alfabetica
    employees_list = build_TList(&employees_list, manager);

    TList aux = employees_list;
    while (aux != NULL) {
        fprintf(f, "%s ", aux->name);
        aux = aux->next;
    }
    fprintf(f, "\n");

    free_TList(employees_list);

    return;
}

/* Afiseaza toti angajatii de pe un nivel din ierarhie.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * level: nivelul din ierarhie
 */
void get_employees_by_level(FILE *f, Tree tree, int level) {
    int current_lvl = 0;
    TList X_level_list = NULL;

    print_X_level(tree, &X_level_list, current_lvl, level);

    TList aux = X_level_list;
    while (aux != NULL) {
        fprintf(f, "%s ", aux->name);
        aux = aux->next;
    }
    fprintf(f, "\n");

    free_TList(X_level_list);

    return;
}

/* Afiseaza angajatul cu cei mai multi oameni din echipa.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void get_best_manager(FILE *f, Tree tree) {
    TList best_managers_list = NULL; // lista cu managerii (daca sunt mai multi)
    int biggest_team = 0; // acumulator

    find_biggest_team(tree, &best_managers_list, &biggest_team);

    TList aux = best_managers_list;
    while (aux != NULL) {
        fprintf(f, "%s ", aux->name);
        aux = aux->next;
    }
    fprintf(f, "\n");
    
    free_TList(best_managers_list);

    return;
}


/* Reorganizarea ierarhiei cu un alt angajat in varful ierarhiei.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului care trece in varful ierarhiei
 */
Tree reorganize(Tree tree, char *employee_name) {
    if (tree == NULL) {
        return tree;
    }

    Tree employee = find_manager(tree, employee_name); // Angajatul ce va deveni root

    if (employee == tree || employee == NULL) { // Daca este deja root sau nu exista
        return tree;
    }

    Tree new_hierarchy = employee;

    if (employee != NULL) {
        Tree manager = employee->manager;   // Managerul sau
        
        // Sparg arborele. Rup legaturile de pe branchul cu cel care trebuie promovat
        while (manager != NULL) {
            Tree new_manager = manager->manager;

            // Elimin employee din echipa managerului
            if (manager->direct_employees_no == 1) { // Cazul in care era singurul angajat
                manager->direct_employees_no = 0;
                free(manager->team);
                manager->team = NULL;

            } else {    // Cazul general
                for (int i = 0; i < manager->direct_employees_no; i++) {
                    if (strcmp(manager->team[i]->name, employee->name) == 0) {
                        for (int k = i + 1; k < manager->direct_employees_no; k++) {
                            manager->team[k - 1] = manager->team[k];
                        }
                        manager->direct_employees_no--;
                        manager->team = (Tree *) realloc(manager->team, (manager->direct_employees_no) * sizeof(Tree));
                        break;
                    }
                }
            }

            new_hierarchy = rehire_employee(new_hierarchy, &manager, employee->name);
            
            employee = manager;
            manager = new_manager;
        }
    }
    
    new_hierarchy->manager = NULL;

    return new_hierarchy;
}

/* Parcurge ierarhia conform parcurgerii preordine.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void preorder_traversal(FILE *f, Tree tree) {
    if (tree->manager == NULL) {
        fprintf(f, "%s ", tree->name);
    }
    if (tree->team != NULL) {
        for (int i = 0; i < tree->direct_employees_no; i++) {
            fprintf(f, "%s-%s ", tree->team[i]->name, tree->team[i]->manager->name);
            preorder_traversal(f, tree->team[i]);
        }
    }
    if (tree->manager == NULL) {
        fprintf(f, "\n");
    }
    return;
}

/* Elibereaza memoria alocata nodurilor din arbore
 *
 * tree: ierarhia existenta
 */
void destroy_tree(Tree tree) {
    if (tree == NULL) {
        return;
    }
    free(tree->name);
    if (tree->team != NULL) {
        for (int i = 0; i < tree->direct_employees_no; i++) {
            destroy_tree(tree->team[i]);
        }
    }
    free (tree->team);
    free(tree);
    return;
}

