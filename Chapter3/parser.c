#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdarg.h>

#define MAX(x, y) ((x) >= (y) ? (x) : (y))

void* xrealloc(const void *ptr, const size_t num_bytes) {
	ptr = realloc(ptr, num_bytes);
	if (!ptr) {
		perror("xrealloc failed");
		exit(1);
	}
	return ptr;
}

void* xmalloc(const size_t num_bytes) {
	void* ptr = malloc(num_bytes);
	if (!ptr) {
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

void fatal(const char* fmt, ...) {
	va_list(args);
	va_start(args, fmt);
	printf("FATAL: ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
	system("pause");
	exit(1);
}

// Stretchy buffers
typedef struct BufHdr {
	size_t len;
	size_t cap;
	char buf[0]; // [0] new in C99
} BufHdr;

#define buf__hdr(b) ((BufHdr*)((char*)(b) - offsetof(BufHdr, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n)  (buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit((b), 1), (b)[buf__hdr(b)->len++] = (x))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

const void* buf__grow(const void* buf, const size_t new_len, const size_t elem_size)
{
	auto new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
	assert(new_len <= new_cap);
	auto new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
	BufHdr* new_hdr;
	if (buf)
		new_hdr = xrealloc(buf__hdr(buf), new_size);
	else
	{
		new_hdr = xmalloc(new_size);
		new_hdr->len = 0;
	}
	new_hdr->cap = new_cap;
	return new_hdr->buf;
}



void list_test() {
	uint64_t *list = NULL;
	assert(list == NULL);
	assert(buf_len(list) == 0);
	const int N = 1024;
	for (int i = 0; i < N; i++)
		buf_push(list, i); //std::vector::push_back equivalent

	assert(buf_len(list) == N);
	for (uint64_t i = 0; i < buf_len(list); i++) {
		assert(list[i] == i);
	}
	buf_free(list);
	assert(list == NULL);
	assert(buf_len(list) == 0);
}

typedef struct InternStr {
	size_t len;
	const char* str;
} InternStr;

static InternStr *interns;

const char* str_intern_range(const char *start, size_t len) {
	for (size_t i = 0; i < buf_len(interns); i++) {
		if (interns[i].len == len && strncmp(interns[i].str, start, len) == 0)
			return  interns[i].str;
	}

	char *str = xmalloc(len + 1);
	memcpy(str, start, len);
	str[len] = 0;
	buf_push(interns, ((InternStr){len, str}));
	return str;
}

const char* str_intern(const char* str) {
	return str_intern_range(str, strlen(str));
}

void str_intern_test() {
	char x[] = "Hello";
	char y[] = "Hello";
	assert(x != y);
	const char *px = str_intern(x);
	const char *py = str_intern(y);
	assert(px == py);
	char z[] = "Hello!";
	const char *pz = str_intern(z);
	assert(pz != px);
}

// lexing: translating char stream to token stream

typedef enum TokenKind {
	TOKEN_SPACE = ' ',
	TOKEN_LAST_CHAR = 127,
	TOKEN_INT,
	TOKEN_IDENT,
	// ...
} TokenKind;

// Warning: This returns a pointer to a static internal buffer, so it'll be overwritten next call.
const char* token_kind_name(TokenKind kind) {
	static char buf[256];
	switch(kind) {
	case TOKEN_INT:
		sprintf(buf, "integer");
		break;
	case TOKEN_IDENT:
		sprintf(buf, "identifier");
		break;
	default:
		if (kind > TOKEN_SPACE && kind <= TOKEN_LAST_CHAR)
			sprintf(buf, "'%c'", kind);
		else
			sprintf(buf, "<ASCII %d>", kind);
		break;
	}
	return buf;
}






typedef struct Token {
	TokenKind kind;
	const char *start;
	size_t len;
	union {
		int val;
		const char* ident;
	};
} Token;

Token token;
const char *stream, *keyword_if, *keyword_for, *keyword_while;

void init_keywords() {
	keyword_if = str_intern("if");
	keyword_for = str_intern("for");
	keyword_while = str_intern("while");
	// ...
}


void next_token() {
	token.start = stream;
	switch (*stream) {
	case TOKEN_SPACE:
		do {
			stream++;
		}while(*stream == TOKEN_SPACE);
		next_token();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		int val = 0;
		while (isdigit(*stream)) {
			val *= 10;
			val += *stream++ - '0';
		}
		token.kind = TOKEN_INT;
		token.val = val;
		break;
	}
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_': {
		while (isalnum(*stream) || *stream == '_') {
			stream++;
		}
		token.kind = TOKEN_IDENT;
		token.ident = str_intern_range(token.start, stream - token.start);
		break;
	}
	default:
		token.kind = *stream++;
		break;
	}
	token.len = stream - token.start;
}

void init_stream(const char* str) {
	stream  = str;
	next_token();
}

void print_token(Token token) {
	switch (token.kind) {
	case TOKEN_INT:
		printf("TOKEN INT: %llu\n", token.val);
		break;
	case TOKEN_IDENT:
		printf("TOKEN NAME: %.*s\n", (int)token.len, token.start);
		break;
	default:
		printf("TOKEN '%c'\n", token.kind);
		break;
	}
}

inline bool is_token(TokenKind kind) {
	return token.kind == kind;
}

inline bool is_token_ident(const char* ident) {
	return token.kind == TOKEN_IDENT && token.ident == ident;
}

inline bool match_token(TokenKind kind) {
	if(is_token(kind)) {
		next_token();
		return true;
	}

	return false;
}

inline bool expect_token(TokenKind kind) {
	if (is_token(kind)) {
		next_token();
		return true;
	}
	fatal("expected token %s, got %s", token_kind_name(kind), token_kind_name(token.kind));
}


void lex_test() {
	printf("\nLexer Tests\n\n");
	char *source = "+()_HELLO1,234+FOO!994";
	stream = source;
	next_token();
	while (token.kind) {
		print_token(token);
		next_token();
	}
}

/*
 *		EXPRESSION GRAMMER
 *
 *		expr3 = INT | '(' expr ')'
 *		expr2 = [+-]? expr2 | expr3
 *		expr1 = expr2 ([/*] expr2)*
 *		expr0 = expr1 ([+-] expr1)*
 *		expr  = expr0
 *
 */

int parse_expr();

int parse_expr3() {
	if(is_token(TOKEN_INT)) {
		const int val = token.val;
		next_token();
		return val;
	}
	else if(match_token('(')) {
		const int val = parse_expr();
		expect_token(')');
		return val;
	}
	else {
		fatal("expecteed integer of (, got %s", token_kind_name(token.kind));
		return 0;
	}
}

int parse_expr2() {
	if (match_token('-'))
		return -parse_expr2();
	else if (match_token('+'))
		return parse_expr2();
	else
		return parse_expr3();
}

int parse_expr1() {
	int val = parse_expr2();
	while(is_token('*') || is_token('/')) {
		const char op = token.kind;
		next_token();
		const int rval = parse_expr2();
		if (op == '*')
			val *= rval;
		else {
			assert(op == '/');
			assert(rval != 0);
			val /= rval;
		}
	}
	return val;
}

int parse_expr0() {
	int val = parse_expr1();
	while(is_token('+') || is_token('-')) {
		const char op = token.kind;
		next_token();
		const int rval = parse_expr1();
		if (op == '+')
			val += rval;
		else {
			assert(op == '-');
			val -= rval;
		}
	}
	return val;
}

int parse_expr() {
	return parse_expr0();
}

int parse_expr_str(const char* str) {
	init_stream(str);
	return parse_expr();
}

#define TEXT_EXPR(x) \
	printf("%s == %d\n", #x, (x)); \
	assert(parse_expr_str(#x) == (x)) 

void parse_test() {
	printf("\nParse Tests\n\n");
	TEXT_EXPR(1);
	TEXT_EXPR((1));
	TEXT_EXPR(-+1);
	TEXT_EXPR(1 - 2 - 3);
	TEXT_EXPR(2 * 3 + 4 * 5);
	TEXT_EXPR(2 * (3 + 4) * 5);
	TEXT_EXPR(2 + -3);
}

#undef TEXT_EXPR
int main() {
	list_test();
	lex_test();
	str_intern_test();
	parse_test();
	system("pause");
	return 0;
}
