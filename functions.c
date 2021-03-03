/* POSTOLACHE Alexandru-Gabriel */

// functions used for implementing Queue and Stack

void Free_Page(void *a)
{
    Web_Page b = (Web_Page)(a);
    free(b->resources);
    free(b);
}
void* InitS(size_t d)
{
    ASt s;
    s = (ASt)malloc(sizeof(TStiva));
    if(!s) return NULL;
    s->dime = d;
    s->vf = NULL;
    return (void*)s;
}

int PushS(void *a, void *ae)
{
    ACel aux = (ACel)malloc(sizeof(TCel));
    if(!aux)return 0;
    aux->info = (Web_Page)malloc(((TStiva *)a)->dime);
    if(!aux->info)return 0;
    memcpy(aux->info, ae, ((TStiva *)a)->dime);
    aux->urm = ((TStiva*)a)->vf;
    ((TStiva *)a)->vf = aux;
    return 1;
}

int Pop(void* s, void* ae)
{
    if(((TStiva*)s)->vf == NULL) return 0;
    memcpy(ae, ((TStiva*)s)->vf->info, ((TStiva*)s)->dime);
    ACel aux = ((TStiva*)s)->vf;
    ((TStiva*)s)->vf = aux->urm;
    free(aux->info);
    free(aux);
    return 1;
}

void ResetS(void *a)
{
    ACel l = ((TStiva*)a)->vf;
    //destroy l
    ACel aux;
    while(l)
    {
        aux = l;
        l = l->urm;
        free(aux->info);
        free(aux);
    }
    ((TStiva*)a)->vf = NULL;
}

void DistrS(void *a, TF Free_EL)
{
    ACel l = ((TStiva*)a)->vf;
    //destroy l
    ACel aux;
    while(l)
    {
        aux = l;
        l = l->urm;
        Free_EL(aux->info);
        free(aux);
    }
    ((TStiva*)a)->vf = NULL;
    free(a);
}
void* InitQ(size_t d)
{
    AQ c;
    c = (AQ)malloc(sizeof(TCoada));
    if(!c) return NULL;
    c->dime = d;
    c->ic = NULL;
    c->sc = NULL;
    return (void*)c;
}
//add at the end of the queue
int PushC(void *a, void*ae)//ae e alocat deja
{
    ACel aux = malloc(sizeof(TCel));
    if(!aux)return 0;
    aux->info = malloc(((AQ)a)->dime);
    if(!aux->info)return 0;
    memcpy(aux->info, ae, ((AQ)a)->dime);
    aux->urm = NULL;
    if(((AQ)a)->ic == NULL && ((AQ)a)->sc == NULL) //coada vida
    {
        ((AQ)a)->ic = aux;
        ((AQ)a)->sc = aux;
    }
    else
    {
        ((AQ)a)->sc->urm = aux;
        ((AQ)a)->sc = aux;
    }
    return 1;
}
int Dequeue(void *a, void *ae)
{
    ACel aux = ((AQ)a)->ic;
    if(aux == NULL)
        return 0;
    ((AQ)a)->ic = aux->urm;
    memcpy(ae, aux->info, ((AQ)a)->dime);
    free(aux->info);
    free(aux);
    if(((AQ)a)->ic == NULL)
        ((AQ)a)->sc = NULL;
    return 1;
}
int ConcatQ(void *d, void *s)
{
    if(((AQ)d)->ic == NULL && ((AQ)d)->sc == NULL)
    {
        //if d is empty
        ((AQ)d)->ic = ((AQ)s)->ic;
        ((AQ)d)->sc = ((AQ)s)->sc;
        ((AQ)s)->ic = NULL;
        ((AQ)s)->sc = NULL;
        return 1;
    }
    else if(((AQ)s)->ic == NULL && ((AQ)s)->sc == NULL)
    {
        //if s is empty
        return 0;
    }
    else
    {
        //if s and d aren't empty
        ((AQ)d)->sc->urm = ((AQ)s)->ic;
        ((AQ)d)->sc = ((AQ)s)->sc;
        ((AQ)s)->ic = NULL;
        ((AQ)s)->sc = NULL;
        return 1;
    }
}
void ResetQ(void *a)
{
    ACel p = ((AQ)a)->ic;
    ((AQ)a)->sc = NULL;
    ((AQ)a)->ic = NULL;
    while(p != NULL)
    {
        ACel aux = p;
        p = p->urm;
        free(aux->info);
        free(aux);
    }
}
void DistrQ(void *a)
{
    ResetQ(a);
    free(a);
}