#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
// lexing: translating char stream to token stream

typedef enum TokenKind {
	TOKEN_INT = 128,
	TOKEN_IDENT,
	// ...
} TokenKind;

typedef struct Token {
	TokenKind kind;
	const char *start;
	size_t len;
	uint64_t val;
} Token;

Token token;
const char *stream;

void next_token() {
	switch (*stream) {
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
		uint64_t val = 0;
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
		const char *start = stream++;
		while (isalnum(*stream) || *stream == '_') {
			stream++;
		}
		token.kind = TOKEN_IDENT;
		token.start = start;
		token.len = stream - start;
		break;
	}
	default:
		token.kind = *stream++;
		break;
	}
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

void lex_test() {
	char *source = "+()_HELLO1,234+FOO!994";
	stream = source;
	next_token();
	while (token.kind) {
		print_token(token);
		next_token();
	}
}

int main() {
	lex_test();
	return 0;
}
