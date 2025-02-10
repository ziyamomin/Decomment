#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

enum Statetype {
    START,
    IN_COMMENT,
    ASTERISK,
    STRING_LITERAL,
    CHAR_LITERAL,
    ESCAPE,
    FORWARD_SLASH,
    REJECT
};

void print(char c) {
    putchar(c);
}

void report_error(int line_number) {
    fprintf(stderr, "Error: line %d: unterminated comment\n", line_number);
}

enum Statetype handleStartState(int c, int *line_number) {
    enum Statetype state = START;

    if (c == '/') {
        state = FORWARD_SLASH;
    } else if (c == '"') {
        print(c);
        state = STRING_LITERAL;
    } else if (c == '\'') {
        print(c);
        state = CHAR_LITERAL;
    } else if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }
    return state;
}

enum Statetype handleForwardSlashState(int c) {
    enum Statetype state = START;

    if (c == '*') {
        print(' ');
        state = IN_COMMENT;
    } else {
        print('/');
        print(c);
    }
    return state;
}

enum Statetype handleInCommentState(int c, int line_number) {
    enum Statetype state = IN_COMMENT;

    if (c == '*') {
        state = ASTERISK;
    } else if (c == '\n') {
        line_number++;
    } else if (c == EOF) {
        report_error(line_number);
        state = REJECT;
    }
    return state;
}

enum Statetype handleAsterickCase(int c) {
    enum Statetype state = IN_COMMENT;
    if (c == '/') {
        state = START;
        // Print space to replace comment
    } else if (c == '*') {
        state = ASTERISK;
    }

    return state;
}

enum Statetype handleStringLiteralCase(int c, int *line_number) {
    enum Statetype state = STRING_LITERAL;

    if (c == '\\') {
        print(c);
        state = ESCAPE;
    } else if (c == '"') {
        print(c);
        state = START;
    } else if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }

    return state;
}

enum Statetype handleCharLiteralState(int c, int *line_number) {
    enum Statetype state = CHAR_LITERAL;

    if (c == '\\') {
        print(c);
        state = ESCAPE;
    } else if (c == '\'') {
        print(c);
        state = START;
    } else if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }

    return state;
}

enum Statetype handleEscapeCaseState(int c, enum Statetype prev_state) {
    print(c);
    return prev_state;
}

int main(void) {
    int c;
    enum Statetype state = START;
    enum Statetype prev_state = START;
    int line_number = 1;

    while ((c = getchar()) != EOF) {
        switch (state) {
            case START:
                state = handleStartState(c, &line_number);
                break;
            case FORWARD_SLASH:
                state = handleForwardSlashState(c);
                break;
            case IN_COMMENT:
                state = handleInCommentState(c, line_number);
                break;
            case ASTERISK:
                state = handleAsterickCase(c);
                break;
            case STRING_LITERAL:
                prev_state = STRING_LITERAL;
                state = handleStringLiteralCase(c, &line_number);
                break;
            case CHAR_LITERAL:
                prev_state = CHAR_LITERAL;
                state = handleCharLiteralState(c, &line_number);
                break;
            case ESCAPE:
                state = handleEscapeCaseState(c, prev_state);
                break;
            case REJECT:
                return EXIT_FAILURE;
        }
    }

    if (state == IN_COMMENT || state == ASTERISK) {
        report_error(line_number);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}