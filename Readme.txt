// Boldeanu Ana-Maria
// 311 CD
// SD Tema 2


=====================================================================
============================= Cerinta 1 =============================
=====================================================================

==================== hire() ====================

   - Am setat pointerii pentru manager si team la NULL si marimea echipei la 0 ca
sa aiba o valoare de referinta nula.

   - Am folosit o functie auxiliara find_manager()

================ find_manager() ================

   - Aceasta primeste ca argument ierarhia tree si numele angajatului care 
trebuie gasit si returneaza pointer la  nodul care corespunde angajatului cautat.
Daca nu exista, intoarce NULL.
   Cautarea se face recursiv. Intai verific nodul curent si apoi fiecare copil
(daca team != NULL)
   
   - Daca managerul gasit nu mai are alti angajati, fac intai alocarea memoriei
pentru vectorul team. Daca are, fac o realocare (team va ocupa intotdeauna exact
nr_de_copii * sizeof(pointer) bytes) si apoi iterez vectorul ca sa gasesc
pozitia unde trebuie adaugat angajatul - in acelasi timp, mut si succesorii la
dreapta cu o pozitie ca sa fac loc pentru inserare.

==================== fire() ====================

   - Intai gasesc nodul corespunzator angajatului ce trebuie concediat cu ajutorul
functiei find_manager(), apoi ii determin managerul.

   - Elimin angajatul din echipa managerului sau prin iterarea vectorului team
pentru a gasi angajatul de concediat, apoi mutarea succesorilor cu o pozitie la 
stanga. Realoc memoria pentru team ca sa nu ocupe mai mult decat e necesar.

(Nu am facut separat cazul pentru cand era singurul angajat pentru ca nu a fost
nevoie - abia mai tarziu la reorganize() m-am gandit ca trebuie sa tratez 
separat cazul acela ca sa setez pointerul team la NULL, dar am facut modificarea
doar pentru functia fire_team() pe care am folosit-o acolo.)

   - Dupa eliminarea angajatului din echipa managerului, tree-ul lui inca exista
dar "in aer" (in caz ca avea subordonati)
   - Pentru asta, am facut o functie auxiliara rehire_employee()

=============== rehire_employee() ===============

   - E la fel ca hire(), dar in loc de nume primeste ca argument adresa angajatului
deja existent ce trebuie doar mutat in alta echipa ("re-angajat").

(Aici cumva am avut noroc ca am anticipat nevoia unei functii de genul si am putut
sa o refolosesc de multe ori mai tarziu. Functia are avantajul ca primeste adresa
nodului care exista deja si nu doar un nume, deci face mutarea angajatului cu tot
cu echipa lui si ce mai avea)

================== promote() ====================

   - Dupa ce gasesc nodul corespunzator angajatului ce trebuie promovat, determin
numele noului sau manager (daca exista) si folosesc functiile fire() si hire()
pentru ca fire() ii muta automat echipa cu un nivel mai sus si dezaloca memoria
pentru angajatul respectiv, apoi il "re-angajez" pur si simplu la noul manager.

================ move_employee() ================

   - Am procedat la fel ca la promote, cu singura diferenta ca aici aveam deja
numele noului manager dat ca argument.

================== move_team() ==================

   - Am mers din nou pe principiul fire() si hire(). Am copiat bucatica de cod
de la fire() care elimina angajatul din echipa managerului sau. Apoi am folosit
functia rehire_employee() cu adresa sa (care e practic nodul din varful ierarhiei
ce trebuie mutata) ca sa il adaug in echipa noului manager cu tot cu echipa sa.

================== fire_team() ==================

   - Din nou am copiat bucatica de cod de la fire() care elimina angajatul din
echipa managerului sau, iar apoi nu a ramas decat sa dau free la memoria ocupata
de echipa eliminata.

   - Aici am facut caz separat pentru cand era singurul angajat ca sa fiu sigura
ca se actualizeaza pointerul manager->team la NULL.


=====================================================================
============================= Cerinta 2 =============================
=====================================================================

   - Pentru ca era nevoie de retinerea numelor unor angajati in ordine alfabetica,
am folosit o structura TList de tip lista simplu inlantuita, sortata.
   - TList e pointer la TNode, care contine un nume si un pointer la urmatorul nod.
   
   - add_toList() este o functie de inserarea unui nod in lista sortata. Primeste
ca argument lista sortata si valoarea ce trebuie adaugata. Returneaza noua lista.
   - free_TList() este o functia ce dezaloca memoria folosita pentru o lista.

============ get_employees_by_manager() ===========

   - Determin intai managerul pentru care se cauta angajatii.
   - Lista de angajati, sortata alfabetic, o construiesc recursiv cu ajutorul 
functiei build_TList() care primeste ca argument adresa listei (initial goala) 
si ierarhia ceruta.

   - Pentru fiecare nod din ierarhia ceruta, adaug numele in lista si apoi, daca
are echipa, chem functia build_TList() pentru fiecare copil. Lista data ca argument
e practic un acumulator, iar la sfarsit va contine toate numele din ierarhie.

============= get_employees_by_level() ============

   - Folosesc functia auxiliara print_X_level() care primeste ca argument ierarhia,
adresa listei de angajati de la nivelul cerut (initial goala; este tot un acumulator),
current_lvl este nivelul curent din ierarhie (initial 0) si level este nivelul cerut.

   - Parcurge in mod recursiv intregul arbore in ordinea nod - angajati si retine
mereu care e nivelul curent. Daca acesta coincide cu nivelul cerut, adauga in lista
primita ca argument numele angajatilor de la acest nivel.

================ get_best_manager() ===============

   - best_managers_list este o lista cu toti managerii care au cea mai mare echipa
(daca sunt mai multi care au acelasi nr maxim de angajati)

   - Folosesc functia auxiliara find_biggest_team() care primeste ca argument
ierarhia, adresa listei pe care o va construi si adresa unui acumulator 
(int *biggest_team) care retine cel mai mare numar de angajati din ierarhie.

   - Parcurgerea e tot recursiva. Exista doua cazuri. Daca gasesc un manager cu
echipa mai mare, golesc lista precedenta si construiesc una noua in care il adaug.
Daca gasesc un manager cu echipa la fel de mare ca cel precedent, il adaug in lista
curenta.


=====================================================================
============================= Cerinta 3 =============================
=====================================================================

=================== reorganize() ==================

   - Tree employee este angajatul ce va deveni root
   - Tree manager este managerul sau
   - Tree new_hierarchy este initial employee. Aceasta va fi noua ierarhie dupa
reorganizare.

   - In bucla while(manager != NULL), o sa trec prin fiecare pereche employee -
manager (de jos in sus) pana cand, la ultimul pas, manager este root. Ideea este
sa rup legaturile si sa adaug fiecare manager in echipa employee-ului curent

   - Tree new_manager este manager->manager ca sa pot face trecerea cu un nod mai
sus la sfarsitul buclei.
   
   - Am copiat bucatica de cod de la fire_team() care elimina un angajat din echipa
managerului sau. Apoi, folosesc rehire_employee() ca sa il adaug in noua ierarhie,
sub numele angajatului curent. Actualizarea legaturii pentru noul manager se face
la nivelul functiei rehire_employee(), apoi employee devine manager si manager 
devine new_manager, adica trec cu un nivel mai sus in ierarhie ca sa fac modificarile
recursiv.

==================== Victory!! ===================