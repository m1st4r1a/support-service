#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int num_requests = 0;

struct request{
  int id;
  char username[64];
  char priority;
  time_t timestamp;
  struct request* next;
};

struct cancel{
  struct request* cur;
  char where_is;
  struct cancel* next;
};

struct cancel* cancelled = NULL;

void add(struct request* queue){
  struct request* temp = calloc(1, sizeof(struct request));
  temp->id = num_requests++;

  printf("Введите имя\n");
  char buffer[64] = {0};
  scanf("%s", buffer);

  strcpy(temp->username, buffer);

  printf("Введите приоритет\n");
  char priority;
  scanf("%hhd", &priority);

  temp->priority = priority;

  temp->timestamp = time(NULL);

  while(queue->next != NULL)
    queue = queue->next;

  queue->next = temp;
  printf("Заявка добавлена\n");
}

void view(struct request* queue){
  if (queue == NULL){
    printf("Очередь пустая\n");
    return;
  }

  printf("id: %d\n", queue->id);
  printf("Имя: %s\n", queue->username);
  printf("Приоритет: %hhd\n", queue->priority);

  char time_str[64];
  struct tm *tm_info = localtime(&(queue->timestamp));
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
  printf("Время: %s\n", time_str);
}

void view_cancelled(){
    struct cancel* cur = cancelled;
    while(cur != NULL){
        printf("Очередь: %d\n", cur->where_is);
        view(cur->cur);
        cur = cur->next;
    }
}

void delete(struct request* queue){
  if (queue->next == NULL){
    printf("Очередь пустая\n");
    return;
  }

  struct request* temp = queue->next;

  queue->next = (queue->next)->next;

  free(temp);

  printf("Удалено");
}

void move(struct request* from, struct request* to){
  if(from->next == NULL){
      printf("Очередь пустая\n");
      return;
  }

  struct request* temp = from->next;
  from->next = (from->next)->next;

  while(to->next != NULL && (to->next)->priority >= temp->priority)
    to = to->next;

  temp->next = to->next;
  to->next = temp;
  printf("Перемещено\n");
}

void search(struct request* f, struct request* s, struct request* t){
  char type;
  printf("Искать по id(1), username(2)?\nВведите номер команды:\n");
  scanf(" %c", &type);

  int is_found = 0;

  if (type == '1'){
    int id;
    printf("Введите id: ");
    scanf("%d", &id);

    for(int q = 0; q < 3; q++){
      struct request* c = NULL;
      if (q == 0)
        c = f;
      else if (q == 1)
        c = s;
      else if (q == 2)
        c = t;

      while(c != NULL){
        if(c->id == id){
          printf("Очередь: %d, id: %d, имя: %s, приоритет: %d\n", q+1, c->id, c->username, (int)c->priority);
          is_found = 1;
        }
        c = c->next;
      }
    }
  }else if (type == '2'){
    char name[64];
    printf("Введите username: ");
    scanf("%s", name);

    for(int q = 0; q < 3; q++){
      struct request* c = NULL;
      if (q == 0)
        c = f;
      else if (q == 1)
        c = s;
      else if (q == 2)
        c = t;

      while(c != NULL){
        if(strcmp(c->username, name) == 0){
          view(c);
          is_found = 1;
        }
        c = c->next;
      }
    }
  }else{
    printf("Нет такой команды.\n");
  }

  if(!is_found) printf("Ничего не найдено\n");
}

void quicksort(struct request** arr, int left, int right){
    if(left >= right) return;

    struct request* base = arr[right];
    int i = left - 1;

    for(int j = left; j < right; j++){
        if(arr[j]->priority >= base->priority){
            i++;
            struct request* tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }

    struct request* tmp = arr[i+1];
    arr[i+1] = arr[right];
    arr[right] = tmp;

    quicksort(arr, left, i);
    quicksort(arr, i + 2, right);
}

void list(struct request* first, struct request* second, struct request* third){
    struct request** arr = NULL;
    int count = 0;
    
    struct request* cur = first->next;
    while(cur != NULL){
        arr = realloc(arr, (count + 1) * sizeof(struct request*));
        arr[count++] = cur;
        cur = cur->next;
    }
    cur = second->next;
    while(cur != NULL){
        arr = realloc(arr, (count + 1) * sizeof(struct request*));
        arr[count++] = cur;
        cur = cur->next;
    }
    cur = third->next;
    while(cur != NULL){
        arr = realloc(arr, (count + 1) * sizeof(struct request*));
        arr[count++] = cur;
        cur = cur->next;
    }
    
    if(count == 0){
        printf("Все очереди пусты\n");
        return;
    }

    quicksort(arr, 0, count - 1);
    for(int i = 0; i < count; i++)
        view(arr[i]);
    free(arr);
}

void cancel(struct request* queue, char where){
    if(queue->next == NULL){
        printf("Очередь пустая\n");
        return;
    }

    struct cancel* new = calloc(1, sizeof(struct cancel));
    new->cur = queue->next;
    new->where_is = where;
    new->next = cancelled;

    queue->next = queue->next->next;
    new->cur->next = NULL;

    cancelled = new;

    printf("Отменено\n");
}

void reditus(struct request* first, struct request* second, struct request* third){
    if(cancelled == NULL){
        printf("Нет отменённых заявок\n");
        return;
    }
    
    if (cancelled->where_is == 1){
      cancelled->cur->next = first->next;
      first->next = cancelled->cur;
      struct cancel* td = cancelled;
      cancelled = cancelled->next;
      free(td);
    }else if(cancelled->where_is == 2){
      cancelled->cur->next = second->next;
      second->next = cancelled->cur;
      struct cancel* td = cancelled;
      cancelled = cancelled->next;
      free(td);
    }else{
      cancelled->cur->next = third->next;
      third->next = cancelled->cur;
      struct cancel* td = cancelled;
      cancelled = cancelled->next;
      free(td);
    }

    printf("Восстановлено\n");
}

void export(struct request* queue, char type){
  FILE* f = fopen("save.txt", "a");

  while (queue->next != NULL){
    queue = queue->next;
    fprintf(f, "%d %d %s %d %ld\n", type, queue->id, queue->username, queue->priority, queue->timestamp);
  }

  fclose(f);
}

void export_cancel(){
  FILE* f = fopen("save.txt", "a");

  struct cancel *temp = cancelled;

  while (temp != NULL){
    fprintf(f, "4 %d %d %s %d %ld\n", temp->where_is, (temp->cur)->id, (temp->cur)->username, (temp->cur)->priority, (temp->cur)->timestamp);
    temp = temp->next;
  }

  fclose(f);
}

void import(struct request* first, struct request* second, struct request* third){
    FILE* f = fopen("save.txt", "r");
    if(f == NULL){
        printf("Файл не найден\n");
        return;
    }

    int where, id, priority;
    char username[64];
    long timestamp;

    while(fscanf(f, "%d", &where) == 1){
        if(where == 4){
            int where_is;
            fscanf(f, "%d %d %s %d %ld", &where_is, &id, username, &priority, &timestamp);
            struct request* req = calloc(1, sizeof(struct request));
            req->id = id;
            strcpy(req->username, username);
            req->priority = priority;
            req->timestamp = timestamp;
            struct cancel* node = calloc(1, sizeof(struct cancel));
            node->cur = req;
            node->where_is = where_is;
            node->next = cancelled;
            cancelled = node;
        }else{
            fscanf(f, "%d %s %d %ld", &id, username, &priority, &timestamp);
            struct request* req = calloc(1, sizeof(struct request));
            req->id = id;
            strcpy(req->username, username);
            req->priority = priority;
            req->timestamp = timestamp;
            struct request* dest = (where == 1) ? first : (where == 2) ? second : third;
            while(dest->next != NULL)
                dest = dest->next;
            dest->next = req;
        }
    }

    fclose(f);

    if(id >= num_requests) num_requests = id + 1;

    printf("Импортировано\n");
}



int main(){
  struct request* first = calloc(1, sizeof(struct request));
  struct request* second = calloc(1, sizeof(struct request));
  struct request* third = calloc(1, sizeof(struct request));

  printf("Введите команду\n");

  char buffer[64];
  scanf("%s", buffer);

  char num_queue;

  while(strcmp(buffer, "exit") != 0){
    if (strcmp(buffer, "add") == 0){
      printf("Введите номер очереди\n");
      scanf(" %c", &num_queue);

      if (num_queue == '1')
        add(first);
      else if (num_queue == '2')
        add(second);
      else if (num_queue == '3')
        add(third);
      else
        printf("Введён некорректный номер очереди\n");
    }else if (strcmp(buffer, "view") == 0){
      printf("Введите номер очереди\n");
      scanf(" %c", &num_queue);

      if (num_queue == '1')
        view(first->next);
      else if (num_queue == '2')
        view(second->next);
      else if (num_queue == '3')
        view(third->next);
      else
        printf("Введён некорректный номер очереди\n");
    }else if (strcmp(buffer, "delete") == 0){
      printf("Введите номер очереди\n");
      scanf(" %c", &num_queue);

      if (num_queue == '1')
        delete(first);
      else if (num_queue == '2')
        delete(second);
      else if (num_queue == '3')
        delete(third);
      else
        printf("Введён некорректный номер очереди\n");
    }else if(strcmp(buffer, "move") == 0){
      printf("Из какой очереди (1-3): ");
      scanf(" %c", &num_queue);
      printf("В какую очередь (1-3): ");
      char num_queue2;
      scanf(" %c", &num_queue2);

      struct request* from = (num_queue=='1') ? first : (num_queue=='2') ? second : third;
      struct request* to   = (num_queue2=='1') ? first : (num_queue2=='2') ? second : third;

      if(num_queue < '1' || num_queue > '3' || num_queue2 < '1' || num_queue2 > '3')
        printf("Введён некорректный номер очереди\n");
      else if(from == to)
        printf("Одинаковые очереди\n");
      else
        move(from, to);
    }else if(strcmp(buffer, "search") == 0){
      search(first, second, third);
    }else if(strcmp(buffer, "cancel") == 0){
      printf("Введите номер очереди: ");
      scanf(" %c", &num_queue);

      if (num_queue == '1')
        cancel(first, 1);
      else if (num_queue == '2')
        cancel(second, 2);
      else if (num_queue == '3')
        cancel(third, 3);
      else
        printf("Введён некорректный номер очереди\n");
    }else if(strcmp(buffer, "return") == 0){
      reditus(first, second, third);
    }else if(strcmp(buffer, "all") == 0){
      struct request* cur = first->next;

      printf("Очередь 1:\n");
      while(cur != NULL){ 
        view(cur); 
        cur = cur->next; 
      }

      printf("Очередь 2:\n");
      cur = second->next;
      while(cur != NULL){ 
        view(cur); 
        cur = cur->next; 
      }

      printf("Очередь 3:\n");
      cur = third->next;
      while(cur != NULL){ 
        view(cur); 
        cur = cur->next; 
      }

      printf("Отменённые:\n");
      view_cancelled();
    }else if(strcmp(buffer, "export") == 0){
      remove("save.txt");

      struct request* cur = first->next;

      export(first, 1); 
      export(second, 2); 
      export(third, 3); 

      export_cancel(); 
    }else if(strcmp(buffer, "import") == 0){
      import(first, second, third);
    }
    

    printf("Введите команду\n");
    scanf("%s", buffer);
  }

  remove("save.txt");
  export(first, 1);
  export(second, 2);
  export(third, 3);
  export_cancel();
}