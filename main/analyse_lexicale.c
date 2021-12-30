#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define bool int
#define true 1
#define false 0
#define MAX_CHAR_NUM 32
#define MAX_MOT_NUM 144

//Related to lexeme

typedef enum lexeme_t {
    DIGIT,  // [0-9]+
    OPERATOR,  // =, →
    CHARACTER,  // `.+`
    KEYWORD,  // Automate, etats, initial, final, transition
    DELIMITER, // ()[]{}
    STRING  // ".*"
} lexeme_t;

typedef struct lexeme {
    lexeme_t type;
    char *value;
} lexeme;

//Related to heap
typedef struct HeapEle {
    lexeme *ele;
    int head_ptr;

    bool (*lt)(struct HeapEle *, struct HeapEle *);

    bool (*gt)(struct HeapEle *, struct HeapEle *);

    bool (*eq)(struct HeapEle *, struct HeapEle *);
} HeapEle;

static inline bool lt(HeapEle *a, HeapEle *b) { return a->head_ptr < b->head_ptr; }

static inline bool gt(HeapEle *a, HeapEle *b) { return a->head_ptr > b->head_ptr; }

static inline bool eq(HeapEle *a, HeapEle *b) { return a->head_ptr == b->head_ptr; }

HeapEle *NewHeapEle(lexeme *ele, int head_ptr) {
    HeapEle *this = malloc(sizeof(HeapEle));
    this->head_ptr = head_ptr;
    this->ele = ele;
    this->lt = lt;
    this->gt = gt;
    this->eq = eq;
    return this;
}

typedef struct Heap {
    HeapEle *container;
    int size;
    int n; //current size of heap

    void (*sift_down)(struct Heap *, int);

    int (*left_child)(int);

    int (*right_child)(int);

    int (*parent)(int);

    HeapEle *(*get_ele)(struct Heap *, int);

    void (*set_ele)(struct Heap *, int, HeapEle *);

    bool (*is_leaf)(struct Heap *, int);

    HeapEle *(*remove_min)(struct Heap *);

    bool (*insert)(struct Heap *, HeapEle *);

    void (*swap)(struct Heap *, int, int);
} Heap;

static inline int left_child(int pos) { return 2 * pos + 1; }

static inline int right_child(int pos) { return 2 * pos + 2; }

static inline int parent(int pos) { return (pos - 1) / 2; }

static inline HeapEle *get_ele(Heap *heap, int pos) { return &heap->container[pos]; }

static inline void set_ele(Heap *heap, int pos, HeapEle *ele) {
    heap->container[pos] = *ele;
}

static void swap(Heap *heap, int a, int b) {
    HeapEle temp = *heap->get_ele(heap, a);
    heap->set_ele(heap, a, heap->get_ele(heap, b));
    heap->set_ele(heap, b, &temp);
}

static inline bool is_leaf(Heap *heap, int pos) { return pos >= heap->n / 2; }

static void sift_down(Heap *heap, int pos) {
    HeapEle *temp = NewHeapEle(NULL, 0);
    while (!heap->is_leaf(heap, pos)) {
        int j = heap->left_child(pos);
        int rc = heap->right_child(pos);
        if ((rc < heap->n) && temp->gt(heap->get_ele(heap, j), heap->get_ele(heap, rc))) j = rc;
        if (!temp->gt(heap->get_ele(heap, pos), heap->get_ele(heap, j))) return;
        heap->swap(heap, pos, j);
        pos = j;
    }
    free(temp);
}

static HeapEle *remove_min(Heap *heap) {
    if (heap->n == 0) return false;
    heap->swap(heap, 0, --heap->n);
    if (heap->n != 0) heap->sift_down(heap, 0);
    return heap->get_ele(heap, heap->n);
}

static bool insert(Heap *heap, HeapEle *ele) {
    if (heap->n >= heap->size) return false;
    int curr = heap->n++;
    heap->set_ele(heap, curr, ele);
    while ((curr != 0) && ele->lt(heap->get_ele(heap, curr), heap->get_ele(heap, heap->parent(curr)))) {
        heap->swap(heap, curr, heap->parent(curr));
        curr = heap->parent(curr);
    }
    return true;
}

Heap *NewHeap(int size) {
    Heap *this = malloc(sizeof(Heap));
    this->n = 0;
    this->container = malloc(sizeof(HeapEle) * size);
    this->size = size;
    this->sift_down = sift_down;
    this->left_child = left_child;
    this->right_child = right_child;
    this->parent = parent;
    this->get_ele = get_ele;
    this->set_ele = set_ele;
    this->is_leaf = is_leaf;
    this->remove_min = remove_min;
    this->insert = insert;
    this->swap = swap;
    return this;
}

typedef struct {
    int length;
    char *content;
} String;

String *eliminate_comments(FILE *file) {
    String *buffer = malloc(sizeof(String));
    buffer->length = MAX_MOT_NUM * MAX_CHAR_NUM;
    buffer->content = malloc(sizeof(char) * MAX_CHAR_NUM * MAX_MOT_NUM);
    int c_previous = fgetc(file);
    int c;
    int c_num = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c_previous == '/') {
            switch (c) {
                case '/':
                    while ((c_previous = fgetc(file)) != EOF) {
                        if (c_previous == '\n') {
                            break;
                        }
                    }
                    if (c_previous == EOF) {
                        buffer->content[c_num++] = '\n';
                        break;
                    }
                    break;
                case '*':
                    c_previous = fgetc(file);
                    while ((c = fgetc(file)) != EOF) {
                        if (c_previous == '*' && c == '/') {
                            c_previous = '\n';
                            break;
                        } else
                            c_previous = c;
                    }
                    break;
                default:
                    buffer->content[c_num++] = (char) c_previous;
                    c_previous = c;
            }
        } else {
            buffer->content[c_num++] = (char) c_previous;
            c_previous = c;
        }
    }
    buffer->content[c_num++] = (char) c_previous;
    buffer->length = c_num;
    //for (int i = 0; i < buffer->length; i++) { putchar(buffer->content[i]); }
    return buffer;
}

void regular_expression_match(String *buffer, regex_t regexpr, lexeme_t type, Heap *heap) {
    int res, head, tail, offset = 0;
    regmatch_t pmatch[1];
    char *p = buffer->content;
    do {
        res = regexec(&regexpr, p, 1, pmatch, 0);
        if (res) break;
        head = pmatch[0].rm_so;
        tail = pmatch[0].rm_eo;
        if (type == DIGIT && head + offset != 0 &&
            (buffer->content[head + offset - 1] == '`' || buffer->content[head + offset - 1] == '"')) {
            p += tail;
            offset += tail;
            continue;
        }
        if (type == OPERATOR && buffer->content[head + offset] != '=')
            tail += 2;
        //length of string is tail - head, but needs a \0 and the end
        lexeme *temp = malloc(sizeof(lexeme));
        char *value = malloc(sizeof(char) * (tail - head + 1));
        for (int i = 0; i < tail - head; i++)
            value[i] = buffer->content[head + offset + i];
        value[tail - head] = '\0';
        temp->value = value;
        temp->type = type;
        HeapEle *ele = NewHeapEle(temp, head + offset);
        heap->insert(heap, ele);
        p += tail;
        offset += tail;
    } while (true);
}

void match_delimiter(String *buffer, Heap *heap) {
    lexeme *temp;
    char *value;
    HeapEle *ele;
    char c;
    for (int i = 0; i < buffer->length; i++) {
        c = buffer->content[i];
        if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
            temp = malloc(sizeof(lexeme));
            value = malloc(sizeof(char) * 2);
            value[0] = buffer->content[i];
            value[1] = '\0';
            temp->value = value;
            temp->type = DELIMITER;
            ele = NewHeapEle(temp, i);
            heap->insert(heap, ele);
        }
    }
}

lexeme *analyse_lexicale(char *in_file) {
    FILE *file = fopen(in_file, "r");

    // Eliminate the comments
    String *buffer = eliminate_comments(file);
    // Find the information by regular expressions
    regex_t reg_digit, reg_operator, reg_character, reg_keyword, reg_delimiter, reg_string;
    char *regexp_digit = "[0-9]+";
    char *regexp_operator = "[=→]";
    char *regexp_character = "`[^`.]+`";
    char *regexp_keyword = "(Automate|etats|initial|final|transition)";
    //char *regexp_delimiter = "[\\(\\)\\[\\]\\{\\}]";
    char *regexp_string = "\"[^\".]+\"";
    regcomp(&reg_digit, regexp_digit, REG_EXTENDED);
    regcomp(&reg_operator, regexp_operator, REG_EXTENDED);
    regcomp(&reg_character, regexp_character, REG_EXTENDED);
    regcomp(&reg_keyword, regexp_keyword, REG_EXTENDED);
    //regcomp(&reg_delimiter, regexp_delimiter, REG_EXTENDED);
    regcomp(&reg_string, regexp_string, REG_EXTENDED);
    Heap *result_heap = NewHeap(MAX_MOT_NUM);
    regular_expression_match(buffer, reg_keyword, KEYWORD, result_heap);
    regular_expression_match(buffer, reg_character, CHARACTER, result_heap);
    regular_expression_match(buffer, reg_string, STRING, result_heap);
    //regular_expression_match(buffer, reg_delimiter, DELIMITER, result_heap);
    regular_expression_match(buffer, reg_operator, OPERATOR, result_heap);
    regular_expression_match(buffer, reg_digit, DIGIT, result_heap);
    match_delimiter(buffer, result_heap);

    //generate analyse-lexicale result from the heap
    lexeme *res = malloc(sizeof(lexeme) * (result_heap->n + 1));
    HeapEle *temp;
    for (int i = 0; result_heap->n; i++) {
        temp = result_heap->remove_min(result_heap);
        res[i].type = temp->ele->type;
        res[i].value = temp->ele->value;
    }
    free(buffer->content);
    free(buffer);
    free(result_heap->container);
    free(result_heap);
    return res;
}
