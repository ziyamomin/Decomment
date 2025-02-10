#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
enum Statetype {START, IN_COMMENT, ASTERISK, STRING_LITERAL, CHAR_LITERAL, ESCAPE, FORWARD_SLASH, REJECT};

int line_number = 1;

void print(char c) {
    putchar(c);
}

void report_error() {
    fprintf(stderr, "Error: line %d: unterminated comment\n", line_number);
}

enum Statetype
handleStartState(int c)
{
    enum Statetype state;
    if(c == '/') {
        state = FORWARD_SLASH;
    } 
    if(c == '"') {
        print('"');
        state = STRING_LITERAL;
    }
    if(c == '\'') {
        print('\'');
        state = CHAR_LITERAL;
    }
    if(c == '\n') {
        print('\n');
        state = START;
        line_number++;
    } else {
        print(c);
        state = START;
    }
    return state;
}

enum Statetype
handleForwardSlashState(int c)
{
    enum Statetype state;
    if(c == '*') {
        print(' ');
        state = IN_COMMENT;
    } else {
        print('/');
        print(c);
        state = START;
    }
    return state;
}

enum Statetype
handleInCommentState(int c) {
    enum Statetype state;
    if(c == '*') {
        state = ASTERISK;
    }
    if(c == '\n') {
        line_number++;
        state = IN_COMMENT;
    }
    if(c == EOF) {
        report_error();
        state = REJECT;
    } else {
        state = IN_COMMENT;
    }
    return state;
}

enum Statetype
handleAsterickCase(int c) {
    enum Statetype state;
    if(c == '/') {
        state = START;
    }
    if(c == '*') {
        state = ASTERISK;
    } else {
        state = IN_COMMENT;
    }
    return state;
}

enum Statetype
handleStringLiteralCase(int c) {
    enum Statetype state;
    if(c == '\\') {
        print('\\');
        state = ESCAPE;
    }
    if(c == '"') {
        print('"');
        state = START;
    }
    if(c == '\n') {
        print('\n');
        line_number++;
        state = STRING_LITERAL;
    } else {
        print(c);
        state = STRING_LITERAL;
    }
    return state;
}

enum Statetype
handleCharLiteralState(int c) {
    enum Statetype state;
    if(c == '\\') {
        print('\\');
        state = ESCAPE;
    }
    if(c == '\'') {
        print('\'');
        state = START;
    }
    if(c == '\n') {
        print('\n');
        line_number++;
        state = CHAR_LITERAL;
    } else {
        print(c);
        state = CHAR_LITERAL;
    }
    return state;
}

enum Statetype
handleEscapeCaseState(int c, enum Statetype prev_state) {
    print(c);
    return prev_state;
}

int main(void)
{
    int c;
    enum Statetype state = START;
    enum Statetype prev_state = START;

    while ((c = getchar()) != EOF) {
        switch (state) {
            case START:
                state = handleStartState(c);
                break;
            case FORWARD_SLASH:
                state = handleForwardSlashState(c);
                break;
            case IN_COMMENT:
                state = handleInCommentState(c);
                break;
            case ASTERISK:
                state = handleAsterickCase(c);
                break;
            case STRING_LITERAL:
                prev_state = STRING_LITERAL;
                state = handleStringLiteralCase(c);
                break;
            case CHAR_LITERAL:
                prev_state = CHAR_LITERAL;
                state = handleCharLiteralState(c);
                break;
            case ESCAPE:
                state = handleEscapeCaseState(c, prev_state);
                break;
            case REJECT:
                return EXIT_FAILURE;
        }
    }
    if(state == IN_COMMENT || state == ASTERISK) {
        report_error();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}