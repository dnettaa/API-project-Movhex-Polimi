#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_COMM 16

const int col_disp[] = {-1, 0, 0, 1, 1, 1};
const int rig_disp[] = {0, -1, 1, -1, 0, 1};
const int col_pari[] = {-1, -1, -1, 0, 0, 1};
const int rig_pari[] = {-1, 0, 1, -1, 1, 0};

//colore per visita
typedef enum{
	WHITE, BLACK, GRAY
}color_t;

//lista di rotte
typedef struct routeListNode_{
	int c_d, r_d;
	int route_cost;
	struct routeListNode_ *next;
}routeListNode_t;

//tipo esagono
typedef struct{
	int hex_cost;
	routeListNode_t *route_list;
	color_t color;
	int trip_cost, c, r, version;
	int index;
}hex_t; 

//lista/coda di esagoni 
typedef struct hexListNode_{
	hex_t *hex;
	int dist;
	struct hexListNode_ *next;
}hexListNode_t;

typedef struct{
	hexListNode_t *head, *tail;
}hexQueue_t;

typedef struct {
	hex_t **arr;
	int dim;
}hexHeap_t;

void searchHex(routeListNode_t *h, int r_2, int c_2, int *r_count, int *present); //cerca la rotta aerea
routeListNode_t* removeRoute(routeListNode_t *h, int r_2, int c_2); //rimuove rotta
routeListNode_t* addRoute(routeListNode_t *h, int r_2, int c_2, int cost); //aggiunge rotta se non supera le 5
hexListNode_t* addHex(hexListNode_t *head, hex_t *hex, int dist);
void enqueue(hexQueue_t *q, hex_t *hex, int dist);
hexListNode_t* dequeue(hexQueue_t *q);
int parent(int n);
int left(int n);
int right(int n);
void minHeapify(hexHeap_t *heap, int i);
hex_t* extractMin(hexHeap_t *heap);
void decreaseKey(hexHeap_t *heap, hex_t *hex, int trip_cost);
void minHeapInsert(hexHeap_t *heap, hex_t *hex, int n);

hex_t** initMap(int col, int rig);
void toggle(hex_t *map[], int col, int rig);
void travel(hex_t *map[], int col, int rig, int version);
void changeCost(hex_t *map[], int col, int rig);

int calculateCost(hex_t *hex);
void changeHexCosts(hex_t *hex, int v, int raggio, int dist); //per calcolare costo esagoni con change

//pensiamo di averlo capiuto dijkstra

void initTest(hex_t **map, int c_map, int r_map) {
	int i, j;

	for(i = r_map - 1; i >= 0; i--) {
		for(j = 0; j < c_map; j++) {
			//printf("%d ", map[j][i].trip_cost == INT_MAX ? -1 : map[j][i].trip_cost);
			printf("%d ", map[j][i].hex_cost);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]){
	char comm[MAX_COMM + 1];
	hex_t **map = NULL;
	int c_map, r_map, res;
	int version = 0;
	int i, j;
	routeListNode_t *curr;

	res = scanf("%s", comm);
	while(res > 0){
		if(comm[0] == 'i'){
			res = scanf("%d %d", &c_map, &r_map);
			map = initMap(c_map, r_map);

		}else if(comm[0] == 'c'){

			changeCost(map, c_map, r_map);

		}else if(comm[1] == 'o'){

			toggle(map, c_map, r_map);

		}else if(comm[1] == 'r'){

			version++;
			travel(map, c_map, r_map, version);

		}
		res = scanf("%s", comm);
	}

	//usa sottoprog per deallocare mappa
	for(i = 0; i < c_map; i++) {
		for(j = 0; j < r_map; j++) {
			while(map[i][j].route_list) {
				curr = map[i][j].route_list;
				map[i][j].route_list = map[i][j].route_list->next;
				free(curr);
			}
		}
		free(map[i]);
	}
	free(map);

	return 0;
}

void searchHex(routeListNode_t *h, int r_2, int c_2, int *r_count, int *present){
	*r_count = 0;
	*present = 0;
	while(h){
		(*r_count)++;
		if(h->c_d == c_2 && h->r_d == r_2){
			(*present) = 1;
		}
		h = h->next;
	}
}

routeListNode_t* removeRoute(routeListNode_t *h, int r_2, int c_2){
	routeListNode_t *curr, *prec;
	curr = h;
	prec = NULL;
	while(curr && (h->c_d != c_2 || h->r_d != r_2)){
		prec = curr;
		curr = curr->next;
	}
	if(curr){
		if(prec != NULL) {
			prec->next = curr->next;
		}else { 
			h = curr->next;
		}
		free(curr);
	}
	return h;
}


routeListNode_t* addRoute(routeListNode_t *h, int r_2, int c_2, int cost){
	routeListNode_t *tmp;

	tmp = malloc(sizeof(routeListNode_t));
	if(tmp){
		tmp->c_d = c_2;
		tmp->r_d = r_2;
		tmp->route_cost = cost;
		tmp->next = h;
		h = tmp;
	}
	return h;
}

hexListNode_t* addHex(hexListNode_t *head, hex_t *hex, int dist){
	hexListNode_t *tmp;

	tmp = malloc(sizeof(hexListNode_t));
	if(tmp){
		tmp->hex = hex;
		tmp->dist = dist;
		tmp->next = head;
		head = tmp;
	}
	return head;
}

void enqueue(hexQueue_t *q, hex_t *hex, int dist){
	hexListNode_t *tmp;

	tmp = malloc(sizeof(hexListNode_t));

	if(tmp){
		tmp->hex = hex;
		tmp->dist = dist;
		tmp->next = NULL;

		if(q->head == NULL){
			q->head = tmp;
		}else {
			q->tail->next = tmp;
		}
		q->tail = tmp;
	}
}

hexListNode_t* dequeue(hexQueue_t *q){
	hexListNode_t *tmp = NULL;

	if(q->head != NULL){
		tmp = q->head;
		q->head = q->head->next;
		if(q->head == NULL){
			q->tail = NULL;
		}

		tmp->next = NULL;
	}
	return tmp;
}

int parent(int n){
	if(n % 2 == 0){
		return (n / 2) - 1;
	}
	return n / 2;
}

int left(int n){
	return (2 * n) + 1;
}

int right(int n){
	return (2 * n) + 2;
}

void minHeapify(hexHeap_t *heap, int i){
	int l, r, smallest;
	hex_t *tmp;

	l = left(i);
	r = right(i);
	if(l < heap->dim && heap->arr[l]->trip_cost < heap->arr[i]->trip_cost){
		smallest = l;
	}else {
		smallest = i;
	}
	if(r < heap->dim && heap->arr[r]->trip_cost < heap->arr[smallest]->trip_cost){
		smallest = r;
	}
	if(smallest != i){
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[smallest];
		heap->arr[smallest] = tmp;
		minHeapify(heap, smallest);
	}
}

hex_t* extractMin(hexHeap_t *heap){
	hex_t *min;

	min = heap->arr[0];
	if(heap->dim > 0){
		heap->arr[0] = heap->arr[heap->dim - 1];
		heap->arr[0]->index = 0;
		(heap->dim)--;
		minHeapify(heap, 0);
	}else {
		(heap->dim)--;
	}

	return min;
}

void decreaseKey(hexHeap_t *heap, hex_t *hex, int trip_cost){
	int i;
	hex_t *tmp;

	hex->trip_cost = trip_cost;
	i = hex->index;
	while(i > 0 && heap->arr[parent(i)]->trip_cost > heap->arr[i]->trip_cost){
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[parent(i)];
		heap->arr[i]->index = i;
		heap->arr[parent(i)] = tmp;
		heap->arr[parent(i)]->index = parent(i);
		i = parent(i);
	}
}

void minHeapInsert(hexHeap_t *heap, hex_t *hex, int n){
	int k;

	if(heap->dim < n){
		(heap->dim)++;
		k = hex->trip_cost;
		hex->trip_cost = INT_MAX;
		heap->arr[heap->dim - 1] = hex;
		hex->index = heap->dim - 1;
		decreaseKey(heap, hex, k);
	}
}

hex_t** initMap(int col, int rig){
	hex_t **mappa = NULL;
	int i, j;

	if(col <= 0 || rig <= 0){
		printf("KO\n");
	}else {
		mappa = malloc(sizeof(hex_t*) * col);
		if(mappa){
			for(j = 0; j < col; j++){
				mappa[j] = malloc(sizeof(hex_t) * rig);
				if(mappa[j]){
					for(i = 0; i < rig; i++){
						mappa[j][i].hex_cost = 1;
						mappa[j][i].c = j;
						mappa[j][i].r = i;
						mappa[j][i].trip_cost = INT_MAX;
						mappa[j][i].color = WHITE;
						mappa[j][i].version = 0;
						mappa[j][i].route_list = NULL;
					}
				}
			}
		}
		printf("OK\n");
	}
	return mappa;
}

void toggle(hex_t *map[], int col, int rig){
	int r_1, c_1, r_2, c_2, res, r_count, present;

	res = scanf("%d %d %d %d", &c_1, &r_1, &c_2, &r_2);
	if(res){
		if(c_1 >= 0 && c_1 < col && r_1 >= 0 && r_1 < rig 
			&& c_2 >= 0 && c_2 < col && r_2 >= 0 && r_2 < rig 
			&& (c_1 != c_2 || r_1 !=r_2)){

			searchHex(map[c_1][r_1].route_list, r_2, c_2, &r_count, &present);

			if(present){
				map[c_1][r_1].route_list = removeRoute(map[c_1][r_1].route_list, r_2, c_2);
				printf("OK\n");
			}else if(r_count < 5){
				map[c_1][r_1].route_list = addRoute(map[c_1][r_1].route_list, r_2, c_2, calculateCost(&map[c_1][r_1]));
				printf("OK\n");
			}else {
				printf("KO\n");
			}
		}else {
			printf("KO\n");
		}
	}else {
		printf("KO\n");
	}
}

int calculateCost(hex_t *hex){
	int cost, count;
	routeListNode_t *tmp;

	cost = hex->hex_cost;
	count = 1;
	tmp = hex->route_list;
	while(tmp){
		count++;
		cost += tmp->route_cost;
		tmp = tmp->next;
	}
	return cost/count;
}

void travel(hex_t *map[], int col, int rig, int version){
	int r_1, c_1, r_2, c_2, res;
	hexHeap_t *heap;
	hex_t *currHex;
	int k, n;
	int c_ad, r_ad;
	routeListNode_t *currRoute;
	int new_cost;

	res = scanf("%d %d %d %d", &c_1, &r_1, &c_2, &r_2);
	if(res){
		if(c_1 >= 0 && c_1 < col && r_1 >= 0 && r_1 < rig 
			&& c_2 >= 0 && c_2 < col && r_2 >= 0 && r_2 < rig){
			if(c_1 == c_2 && r_1 == r_2){
				printf("0\n");
			}else {
				n = rig * col;
				heap = malloc(sizeof(hexHeap_t));
				heap->arr = malloc(sizeof(hex_t*) * n);
				heap->dim = 0;
				map[c_1][r_1].trip_cost = 0;
				minHeapInsert(heap, &map[c_1][r_1], n);

				currHex = NULL;
				while(heap->dim > 0 /*&& currHex != &map[c_2][r_2]*/){
					currHex = extractMin(heap);
					if(currHex->hex_cost > 0){
						new_cost = currHex->trip_cost + currHex->hex_cost;
						if(currHex->r % 2 == 0){
							for(k = 0; k < 6; k++){
								c_ad = currHex->c + col_pari[k];
								r_ad = currHex->r + rig_pari[k];
								if(c_ad >= 0 && c_ad < col && r_ad >= 0 && r_ad < rig){
									if(map[c_ad][r_ad].version < version){
										map[c_ad][r_ad].version = version;
										map[c_ad][r_ad].trip_cost = new_cost;
										minHeapInsert(heap, &map[c_ad][r_ad], n);
									}else if(map[c_ad][r_ad].trip_cost > new_cost){
										map[c_ad][r_ad].trip_cost = new_cost;
										decreaseKey(heap, &map[c_ad][r_ad], map[c_ad][r_ad].trip_cost);
									}
								}
							}
						}else {
							for(k = 0; k < 6; k++){
								c_ad = currHex->c + col_disp[k];
								r_ad = currHex->r + rig_disp[k];
								if(c_ad >= 0 && c_ad < col && r_ad >= 0 && r_ad < rig){
									if(map[c_ad][r_ad].version < version){
										map[c_ad][r_ad].version = version;
										map[c_ad][r_ad].trip_cost = new_cost;
										minHeapInsert(heap, &map[c_ad][r_ad], n);
									}else if(map[c_ad][r_ad].trip_cost > new_cost){
										map[c_ad][r_ad].trip_cost = new_cost;
										decreaseKey(heap, &map[c_ad][r_ad], map[c_ad][r_ad].trip_cost);
									}
								}
							}
						}
						currRoute = currHex->route_list;
						while(currRoute){
							if(map[currRoute->c_d][currRoute->r_d].version < version){
								map[currRoute->c_d][currRoute->r_d].version = version;
								map[currRoute->c_d][currRoute->r_d].trip_cost = new_cost;
								minHeapInsert(heap, &map[currRoute->c_d][currRoute->r_d], n);
							}else if(map[currRoute->c_d][currRoute->r_d].trip_cost > new_cost){
								map[currRoute->c_d][currRoute->r_d].trip_cost = new_cost;
								decreaseKey(heap, &map[currRoute->c_d][currRoute->r_d], map[currRoute->c_d][currRoute->r_d].trip_cost);
							}
							currRoute = currRoute->next;
						}
					}
				}
				if(map[c_2][r_2].version < version || map[c_2][r_2].trip_cost == 0){
					printf("-1\n");
				}else {
					printf("%d\n", map[c_2][r_2].trip_cost);
				}
				free(heap->arr);
				free(heap);
			}
		}else {
			printf("-1\n");		
		}
	}else {
		printf("-1\n");	
	}
}

void changeCost(hex_t *map[], int col, int rig){
	int r_1, c_1, v, raggio, res, k, c_ad, r_ad;
	hexListNode_t *visited = NULL, *curr;
	hexQueue_t *visiting = NULL;

	res = scanf("%d %d %d %d", &c_1, &r_1, &v, &raggio);
	if(res){
		if(c_1 >= 0 && c_1 < col && r_1 >= 0 && r_1 < rig
			&& v >= -10 && v <= 10
			&& raggio > 0){

			visiting = malloc(sizeof(hexQueue_t));
			visiting->head = NULL;
			visiting->tail = NULL;
			enqueue(visiting, &map[c_1][r_1], 0);
			map[c_1][r_1].color = GRAY;
			while(visiting->head){
				curr = dequeue(visiting);
				changeHexCosts(&map[curr->hex->c][curr->hex->r], v, raggio, curr->dist);
				if(curr->dist < raggio - 1){
					if(curr->hex->r % 2 == 0){
						for(k = 0; k < 6; k++){
							c_ad = curr->hex->c + col_pari[k];
							r_ad = curr->hex->r + rig_pari[k];
							if(c_ad >= 0 && c_ad < col && r_ad >= 0 && r_ad < rig){
								if(map[c_ad][r_ad].color == WHITE){
									enqueue(visiting, &map[c_ad][r_ad], curr->dist + 1);
									map[c_ad][r_ad].color = GRAY;
								}
							}
						}
					}else {
						for(k = 0; k < 6; k++){
							c_ad = curr->hex->c + col_disp[k];
							r_ad = curr->hex->r + rig_disp[k];
							if(c_ad >= 0 && c_ad < col && r_ad >= 0 && r_ad < rig){
								if(map[c_ad][r_ad].color == WHITE){
									enqueue(visiting, &map[c_ad][r_ad], curr->dist + 1);
									map[c_ad][r_ad].color = GRAY;
								}
							}
						}
					}
				}
				map[curr->hex->c][curr->hex->r].color = BLACK;
				if(visited != NULL){
					curr->next = visited;
				}
				visited = curr;
			}
			while(visited){
				curr = visited;
				visited = visited->next;
				map[curr->hex->c][curr->hex->r].color = WHITE;
				free(curr);
			}
			free(visiting);
			printf("OK\n");
		}else{
			printf("KO\n");
		}
	}else {
		printf("KO\n");
	}
}

void changeHexCosts(hex_t *hex, int v, int raggio, int dist){
	routeListNode_t *curr;
	float tmp;

	tmp = ((float)raggio - (float)dist)/(float)raggio;
	if(tmp < 0){
		tmp = 0;
	}
	tmp *= v;
	hex->hex_cost += tmp;
	if(hex->hex_cost < 0){
		hex->hex_cost = 0;
	}else if(hex->hex_cost > 100){
		hex->hex_cost = 100;
	}

	curr = hex->route_list;
	while(curr){
		tmp = ((float)raggio - (float)dist)/(float)raggio;
		if(tmp < 0){
			tmp = 0;
		}
		tmp *= v;
		curr->route_cost += tmp;
		if(curr->route_cost < 0){
			curr->route_cost = 0;
		}else if(curr->route_cost > 100){
			curr->route_cost = 100;
		}
		curr = curr->next;
	}
}









































