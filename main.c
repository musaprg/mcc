#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // symbol
    TK_NUM,      // integer
    TK_EOF,      // end of input
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // token type
    Token *next;    // next token
    int val;        // value (if its kind is TK_NUM)
    char *str;      // token string
};

Token *token;

// input source
char *user_input;

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "not '%c'", op);
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "not number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip brank space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = term();

    for (;;) {
        if (consume('*'))
            node = new_node(ND_MUL, node, term());
        else if (consume('/'))
            node = new_node(ND_DIV, node, term());
        else
            return node;
    }
}

Node *term() {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_node_num(expect_number());
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: mcc [options] file...\n");
        return 1;
    }

    user_input = argv[1];

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
    printf("_main:\n");
    printf("  mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf("  ret\n");

    return 0;
}
