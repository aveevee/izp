/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <stdbool.h>
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;
    c->obj = NULL;
    if(cap > 0){
        c->obj = malloc(sizeof(struct obj_t)*cap);
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{   
    free(c->obj);
    init_cluster(c,0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->size >= c->capacity){
        resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    }
    c->obj[c->size].id = obj.id;
    c->obj[c->size].x = obj.x;
    c->obj[c->size].y = obj.y;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
    for (int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);
    }

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    for (int i = idx; i < narr; i++)
    {
        carr[i] = carr[i+1];
    }
    
    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrt(pow(o2->x-o1->x, 2)+pow(o2->y-o1->y, 2)*1.0);
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float min = obj_distance(&c1->obj[0], &c2->obj[0]);
    for (int i = 0; i < c1->size; i++)
    {
        for (int j = 0; j < c2->size; j++)
        {
            if(obj_distance(&c1->obj[i], &c2->obj[j])<min){
                min = obj_distance(&c1->obj[i], &c2->obj[j]);
            }
        }
        
    }
    return min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float dist = cluster_distance(&carr[0], &carr[1]);
    for (int i = 0; i < narr; i++)
    {
        for (int j = i+1; j < narr; j++)
        {   
                if(cluster_distance(&carr[i], &carr[j])<dist)
                    {
                        dist = cluster_distance(&carr[i], &carr[j]);
                        *c1 = i;
                        *c2 = j;
                    }
        }
    }
    
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    FILE *file;
    assert(arr != NULL);
    file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "Cannot open file\n");
        fclose(file);
        return -1;
    }
    int count;

    if(!fscanf(file, "count=%d", &count)){
        fprintf(stderr, "Wrong format of file\n");
        fclose(file);
        return -1;
    }
    
    *arr = malloc(sizeof(struct cluster_t)*count);
    
    for (int i = 0; i < count; i++)
    {   
        struct obj_t obj;
        fscanf(file, "%d %g %g", &obj.id, &obj.x, &obj.y);
        
        struct cluster_t cl;
        init_cluster(&cl, 1);
        append_cluster(&cl, obj);
        (*arr)[i]=cl;
    }
    
    fclose(file);
    return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*Function to control correct input to stdin, you can only enter numbers from 0 to 9.
 *The function returns true or false values. If you enter a number from 0 to 9,
 *the function returns true, otherwise false
 */
bool correct_input(char argv[]){
    int length = strlen(argv);
    for (int i = 0; i < length; i++){
        if (argv[i] < '0' || argv[i] > '9'){
            return true;
        }   
        i++;
    }
    return false;
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int atoi_argv = atoi(argv[2]);
    int count = load_clusters(argv[1], &clusters);
    if(argc != 3){
        fprintf(stderr, "Incorrect number of arguments\n");
        return -1;
    }else if(correct_input(argv[2])){
        fprintf(stderr, "Incorrect input\n");
        return -1;
    }else if (atoi_argv > count){
        fprintf(stderr, "wrong number of clusters\n");
        return -1;
    }
    
    while(count > atoi_argv)
	{
		int c1, c2;
		find_neighbours(clusters, count, &c1, &c2);
		merge_clusters(&clusters[c1], &clusters[c2]);
		count = remove_cluster(clusters, count, c2);
	}
	print_clusters(clusters, count);
    for (int i = 0; i < count; i++)
    {   
        clear_cluster(&clusters[i]);
    }
    free(clusters);
}