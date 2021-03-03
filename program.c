/* POSTOLACHE Alexandru-Gabriel */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include "utils.c"
#include "functions.c"
TAB newtab()
{
    TAB aux;
    aux = malloc(sizeof(Tab));
    aux->current_page = NULL;
    aux->back_stack = NULL;
    aux->forward_stack = NULL;
    aux->next = NULL;
    aux->current = NULL;
    return aux;
}
void Add_TAB(TAB lista, TAB el)
{
    TAB p;
    p = lista;
    while(p->next != NULL)
        p = p->next;
    p->next = el;
    (lista)->current = el;
}
void deltab(TAB lista)
{
    TAB p = lista->next;
    TAB ant;
    //when it contains only one element
    if(p->next == NULL)
        lista->next = NULL;
    else
    {
        while(p->next != NULL)
        {
            ant = p;
            p = p->next;
        }
        ant->next = NULL;
        if(lista->current == p)
            lista->current = ant;
    }
    //free p
    if(p->back_stack != NULL)
        DistrS(p->back_stack, Free_Page);
    if(p->forward_stack != NULL)
        DistrS(p->forward_stack, Free_Page);
    if(p->current_page != NULL)
    {
        free(p->current_page->resources);
        free(p->current_page);
    }
    free(p);
}
void change_tab(TAB lista, int index)
{
    //find the tab with a certain index
    TAB p = lista->next;
    int nr = 0;
    if(nr != index)
        while(nr != index)
        {
            p = p->next;
            nr++;
        }
    lista->current = p;
}
void print_open_tabs(TAB lista, FILE *out)
{
    //first TAB from the list
    TAB p = lista->next;
//because the TAB "lista" is treated as a starting point, not as another TAB
    int nr = 0;
    while(p != NULL)
    {
        if(p->current_page != NULL)
            fprintf(out, "(%d: %s)\n", nr, p->current_page->url);
        else
            fprintf(out, "(%d: empty)\n", nr);
        nr++;
        p = p->next;
    }
}
Web_Page AlocWebPage(char *url)
{
    Web_Page pag = malloc(sizeof(W_P));
    strcpy(pag->url, url);
    int nr = 0;
    pag->resources = get_page_resources(pag->url, &nr);
    pag->numar_res = nr;
    return pag;
}
void Add_WebPage(TAB lista, Web_Page page)
{
    TAB p = lista->current;
    //verify if the web page "page" is open in the current TAB
    if(p->current_page == NULL)
        p->current_page = page;
    else
    {
        //add the page in the back_stack
        //if the back_stack doesn't exist create it
        if(p->back_stack == NULL)
            p->back_stack = InitS(sizeof(W_P));
        //add in the stack
        PushS(p->back_stack, p->current_page);
        free(p->current_page);//free the current page
        p->current_page = page; //change the current page
    }
    if(p->forward_stack != NULL)
        DistrS(p->forward_stack, Free_Page);
    p->forward_stack = NULL; //goto will empty the forward_stack
}
void BACK(TAB lista_tab, FILE *out)
{
    TAB current = lista_tab->current;
    if(current->back_stack->vf == NULL
            || current->back_stack == NULL)
        //the stack is empty or unallocated
        fprintf(out, "can't go back, no pages in stack\n");
    else
    {
        Web_Page page = malloc(sizeof(W_P));
        Pop(current->back_stack, page);
        //add the last current page in the forward stack
        //if forward_stack is empty initialize it
        if(current->forward_stack == NULL)
            current->forward_stack = InitS(sizeof(W_P));
        PushS(current->forward_stack, current->current_page);
        free(current->current_page);
        //set page as the current page
        current->current_page = page;
    }
}
void FORWARD(TAB lista_tab, FILE *out)
{
    TAB current = lista_tab->current;
    if(current->forward_stack->vf == NULL
            || current->forward_stack == NULL)
        fprintf(out, "can't go forward, no pages in stack\n");
    else
    {
        Web_Page page = malloc(sizeof(W_P));
        Pop(current->forward_stack, page);
        //the last page must be added to the back_stack
        PushS(current->back_stack, current->current_page);
        free(current->current_page);
        current->current_page = page;
    }
}
void Print_history(AQ Q, FILE *out)
{
    //we need an auxiliar queue
    AQ aux = InitQ(23 * sizeof(char));
    //the info of the element is of char type
    char* ae = malloc(23 * sizeof(TCel));
    while(Dequeue(Q, ae))
    {
        fprintf(out, "%s\n", ae);
        PushC(aux, ae);
    }
    ConcatQ(Q, aux);
    free(ae);
    DistrQ(aux);
}
void del_history(AQ Q, int n)
{
    char* ae = malloc(23 * sizeof(TCel));
    if(n == 0) //in this case the history will be deleted
    {
        while(Dequeue(Q, ae));
        Q->sc = NULL;
        Q->ic = NULL;
    }
    else
    {
        while(n != 0 && Q->ic != NULL)
        {
            Dequeue(Q, ae);
            n--;
        }
    }
    free(ae);
}
void list_dl(TAB lista, FILE *out)
{
    if(lista->current->current_page != NULL)
    {
        Web_Page page = lista->current->current_page;
        int nr = page->numar_res;
        int i = 0;
        while(i != nr)
        {
            fprintf(out, "[%d - \"%s-(%d)\" : %ld]\n",
                    i, page->url, i, page->resources[i].dimension);
            i++;
        }
    }
}
void print_downloads(AQ Q, Desc_Final L, FILE *out)
{
    //we need an auxiliar queue
    AQ aux = InitQ(sizeof(Resource));
    Resource* ae = malloc(sizeof(Resource));
    //firt i will print the incomplete downloads
    while(Dequeue(Q, ae))
    {
        fprintf(out, "[\"%s\" : %ld/%ld]\n", (*ae).name,
                (*ae).dimension, (*ae).currently_downloaded);
        PushC(aux, ae);
    }
    ConcatQ(Q, aux);
    free(ae);
    //print the finished downloads
    while(L != NULL)
    {
        if(L->info != NULL)
            fprintf(out, "[\"%s\" : completed]\n",
                    (*(L->info)).name);
        L = L->next;
    }
    DistrQ(aux);
}
void download(TAB lista, int n, AQ Q)
{
    Web_Page page = lista->current->current_page;
    if(page != NULL) //test 30
    {
        page->resources[n].currently_downloaded
            = page->resources[n].dimension;
        //auxiliar queue
        AQ aux = InitQ(sizeof(Resource));
        Resource* ae;
        //must find the exact postiton to insert the element and
        //we will use dequeue so that we can compare the elements
        int pushed = 0;
        while(pushed == 0)
        {
            if(Q->ic == NULL && Q->sc == NULL)
            {
                //care nu e * deci trimit cu &
                PushC(Q, &(page->resources[n]));
                pushed = 1;
                break;
            }
            //ae must be realloced
            ae = malloc(sizeof(Resource));
            Dequeue(Q, ae); //to insert at the beginning
            if(page->resources[n].dimension <= (*ae).dimension)
            {
                PushC(aux, &(page->resources[n]));
                PushC(aux, ae);
                ConcatQ(aux, Q);
                ConcatQ(Q, aux);
                pushed = 1;
            }
            if(page->resources[n].dimension > (*ae).dimension
                    && Q->ic == NULL && Q->sc == NULL)
            {
                PushC(aux, ae);
                //aux contains the elements before our inserted element
                ConcatQ(Q, aux);
                PushC(Q, &(page->resources[n]));
                pushed = 1;
            }
            //if no insertion is done then save the elements in aux
            PushC(aux, ae);
            free(ae);
        }
        DistrQ(aux);
    }
}
//add a resource in the list of completed downloads
void Add_lista(Desc_Final L, Resource* el)
{
    if(L->info == NULL)
    {
        //primul elem din lista deja alocata
        L->info = el;
    }
    else
    {
        Desc_Final p = L;
        while(p->next != NULL)
            p = p->next;
        Desc_Final aux = malloc(sizeof(D_F));
        aux->next = NULL;
        aux->info = el;
        p->next = aux;
    }
}
//move the completed downloads in the completed downloads list
void WAIT(AQ Q, Desc_Final D, int sec, long int speed)
{
    AQ aux = InitQ(sizeof(Resource));
    Resource* ae = malloc(sizeof(Resource));
    long int biti = speed * sec;
    while(biti != 0 && Dequeue(Q, ae))
    {
        long int dim = (*ae).dimension;
        if(dim <= biti)
        {
            biti = biti - dim;
            (*ae).dimension = 0;
            //the resource was downloaded and must be added to D
            Add_lista(D, ae);
        }
        else
        {
            (*ae).dimension = dim - biti;
            biti = 0;
            PushC(aux, ae);
            free(ae);
            ConcatQ(aux, Q);
            ConcatQ(Q, aux);
        }
        ae = malloc(sizeof(Resource));
    }
    DistrQ(aux);
    free(ae);
}
void free_all_Memory(TAB *lista, AQ Q1, AQ Q2, Desc_Final *desc)
{
    TAB p = (*lista)->next;
    TAB aux;
    while(p != NULL)
    {
        aux = p;
        p = p->next; //eliberam stivele
        if(aux->back_stack != NULL)
            DistrS(aux->back_stack, Free_Page);
        if(aux->forward_stack != NULL)
            DistrS(aux->forward_stack, Free_Page);
        if(aux->current_page != NULL)
        {
            free(aux->current_page->resources);
            free(aux->current_page);
        }
        free(aux);
    }
    free(*lista);
    DistrQ(Q1);
    DistrQ(Q2);
    Desc_Final q = (*desc);
    Desc_Final aux2;
    while(q != NULL)
    {
        aux2 = q;
        q = q->next;
        free(aux2->info);
        free(aux2);
    }
}
int main(int argc, char *argv[])
{
    FILE *in = fopen(argv[1], "rt");
    FILE *out = fopen(argv[2], "wt");
    char buff[100];
    char *token;
    char *desp = " ";
    long int bandwidth = 1024;
    TAB lista_taburi = newtab();
    //the current TAB must be initialized when we add the first page
    lista_taburi->current = newtab();
    Add_TAB(lista_taburi, lista_taburi->current);
    AQ history = InitQ(23 * sizeof(char)); //history queue
    AQ priorities = InitQ(sizeof(Resource)); //priority queue
    Desc_Final finnished = malloc(sizeof(D_F)); //completed downloads list
    finnished->info = NULL;
    finnished->next = NULL;
    while(fgets(buff, 100, in))
    {
        token = strtok(buff, desp);
        if(token != NULL)
        {
            if(strcmp(token, "set_band") == 0)
            {
                token = strtok(NULL, "\n");
                bandwidth = atoi(token);
                token = NULL;
            }
            if(strcmp(token, "newtab\n") == 0)
            {
                TAB nou = newtab();
                //add a new TAB in the list of TABS
                Add_TAB(lista_taburi, nou);
                token = NULL;
            }
            if(strcmp(token, "deltab\n") == 0)
            {
                deltab(lista_taburi);
                token = NULL;
            }
            if(strcmp(token, "change_tab") == 0)
            {
                token = strtok(NULL, "\n");
                change_tab(lista_taburi, atoi(token));
                token = NULL;
            }
            if(strcmp(token, "print_open_tabs\n") == 0)
            {
                print_open_tabs(lista_taburi, out);
            }
            if(strcmp(token, "goto") == 0)
            {
                WAIT(priorities, finnished, 1, bandwidth);
                token = strtok(NULL, "\n"); //url
                Web_Page pag = AlocWebPage(token);
                Add_WebPage(lista_taburi, pag);
                PushC(history, pag->url);
                token = NULL;
            }
            if(strcmp(token, "back\n") == 0)
            {
                BACK(lista_taburi, out);
                token = NULL;
            }
            if(strcmp(token, "forward\n") == 0)
            {
                FORWARD(lista_taburi, out);
            }
            if(strcmp(token, "history\n") == 0)
            {
                Print_history(history, out);
            }
            if(strcmp(token, "del_history") == 0)
            {
                token = strtok(NULL, "\n");
                del_history(history, atoi(token));
                token = NULL;
            }
            if(strcmp(token, "list_dl\n") == 0)
            {
                list_dl(lista_taburi, out);
                token = NULL;
            }
            if(strcmp(token, "downloads\n") == 0)
            {
                print_downloads(priorities, finnished, out);
                token = NULL;
            }
            if(strcmp(token, "download") == 0)
            {
                token = strtok(NULL, "\n");
                download(lista_taburi, atoi(token), priorities);
                token = NULL;
            }
            if(strcmp(token, "wait") == 0)
            {
                token = strtok(NULL, "\n");
                WAIT(priorities, finnished, atoi(token), bandwidth);
                token = NULL;
            }
        }
    }
    fclose(in);
    fclose(out);
    free_all_Memory(&lista_taburi, history, priorities, &finnished);
return 0;
}
