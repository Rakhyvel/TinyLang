struct listElem {struct listElem* next; struct listElem* prev; void* data;};

struct list {struct listElem head;struct listElem tail;};

// List create/destroy
struct list* list_create();

// List traversal
struct listElem* list_begin(struct list*);
struct listElem* list_end(struct list*);

// List operations
void list_insert(struct list*, struct listElem*, void*);
void* list_get(struct list*, int);

void* list_pop(struct list*);
void* list_peek(struct list*);

// Queue operations
void queue_push(struct list*, void*);

// Stack operations
void stack_push(struct list*, void*);

// Simple operations
int list_isEmpty(struct list*);