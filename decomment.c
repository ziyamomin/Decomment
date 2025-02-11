#include <stdio.h>
#include <stdlib.h>

enum Statetype {
    START,
    FORWARD_SLASH,
    IN_COMMENT,
    ASTERISK,
    STRING_LITERAL,
    CHAR_LITERAL,
    ESCAPE_STRING,
    ESCAPE_CHAR,
    REJECT
};

void print(char c) {
    putchar(c);
}

void report_error(int line_number) {
    fprintf(stderr, "Error: line %d: unterminated comment\n", line_number);
}

enum Statetype handleStartState(int c, int *line_number) {
    if (c == '/') {
        return FORWARD_SLASH;
    }
    if (c == '"') {
        print(c);
        return STRING_LITERAL;
    }
    if (c == '\'') {
        print(c);
        return CHAR_LITERAL;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }
    return START;
}

enum Statetype handleForwardSlashState(int c, int *line_number) {
    if (c == '*') {
        print(' ');
        return IN_COMMENT;
    }
    print('/');
    if (c == '\n') {
        print(c);
        (*line_number)++;
    } 
    else {
        print(c);
    }
    return START;
}

enum Statetype handleInCommentState(int c, int *line_number) {
    if (c == '*') {
        return ASTERISK;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
    }
    return IN_COMMENT;
}

enum Statetype handleAsteriskState(int c, int *line_number) {
    if (c == '/') {
        return START;
    }
    if (c == '*') {
        return ASTERISK;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
    }
    return IN_COMMENT;
}

enum Statetype handleStringLiteral(int c, int *line_number) {
    print(c);
    if (c == '\\') {
        return ESCAPE_STRING;
    }
    if (c == '"') {
        return START;
    }
    if (c == '\n') {
        (*line_number)++;
    }
    return STRING_LITERAL;
}

enum Statetype handleCharLiteral(int c, int *line_number) {
    print(c);
    if (c == '\\') {
        return ESCAPE_CHAR;
    }
    if (c == '\'') {
        return START;
    }
    if (c == '\n') {
        (*line_number)++;
    }
    return CHAR_LITERAL;
}

enum Statetype handleEscapeState(int c, enum Statetype previous_state) {
    print(c);
    return previous_state;
}

int main(void) {
    int c;
    enum Statetype state = START;
    int line_number = 1;
    int comment_start_line = 0;

    while ((c = getchar()) != EOF) {
        switch (state) {
            case START:
                state = handleStartState(c, &line_number);
                break;
            case FORWARD_SLASH:
                state = handleForwardSlashState(c, &line_number);
                if (state == IN_COMMENT) {
                    comment_start_line = line_number;
                }
                break;
            case IN_COMMENT:
                state = handleInCommentState(c, &line_number);
                break;
            case ASTERISK:
                state = handleAsteriskState(c, &line_number);
                break;
            case STRING_LITERAL:
                state = handleStringLiteral(c, &line_number);
                break;
            case CHAR_LITERAL:
                state = handleCharLiteral(c, &line_number);
                break;
            case ESCAPE_STRING:
                state = handleEscapeState(c, STRING_LITERAL);
                break;
            case ESCAPE_CHAR:
                state = handleEscapeState(c, CHAR_LITERAL);
                break;
            case REJECT:
                break;
        }
    }

    if (state == IN_COMMENT || state == ASTERISK) {
        report_error(comment_start_line);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}