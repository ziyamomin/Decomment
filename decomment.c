#include <stdio.h>
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
    } 
    else if (c == '"') {
        print(c);
        state = STRING_LITERAL;
    }
    else if (c == '\'') {
        print(c);
        state = CHAR_LITERAL;
    }
    else if (c == '\n') {
        print(c);
        (*line_number)++;
        state = START;
    } else {
        print(c);
        state = START;
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
        state = START;
    }
    return state;
}

enum Statetype handleInCommentState(int c, int *line_number) {
    enum Statetype state = IN_COMMENT;
    if (c == '*') {
        state = ASTERISK;
    } 
    else if (c == '\n') {
        print(c);
        (*line_number)++;
    } 
    else if (c == EOF) {
        state = REJECT;
    }
    return state;
}

enum Statetype handleAsteriskState(int c) {
    enum Statetype state = IN_COMMENT;
    if (c == '/') {
        state = START;
    }
    else if (c == '*') {
        state = ASTERISK;
    }
    return state;
}

enum Statetype handleStringLiteral(int c, int *line_number) {
    enum Statetype state = STRING_LITERAL;
    if (c == '\\') {
        print(c);
        state = ESCAPE;
    } 
    else if (c == '"') {
        print(c);
        state = START;
    } 
    else if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }
    return state;
}

enum Statetype handleCharLiteral(int c, int *line_number) {
    enum Statetype state = CHAR_LITERAL;
    if (c == '\\') {
        print(c);
        state = ESCAPE;
    } 
    else if (c == '\'') {
        print(c);
        state = START;
    } 
    else if (c == '\n') {
        print(c);
        (*line_number)++;
    } else {
        print(c);
    }
    return state;
}

enum Statetype handleEscapeState(int c, enum Statetype previous_state) {
    print(c);
    return previous_state;
}

int main(void) {
    int c;
    enum Statetype state = START;
    enum Statetype previous_state;
    int line_number = 1;
    int comment_start_line = 0;

    while ((c = getchar()) != EOF) {
        enum Statetype prev_state = state;
        
        switch (state) {
            case START: 
                state = handleStartState(c, &line_number);
                break;
            case FORWARD_SLASH: 
                state = handleForwardSlashState(c);
                break;
            case IN_COMMENT: 
                state = handleInCommentState(c, &line_number);
                break;
            case ASTERISK: 
                state = handleAsteriskState(c);
                break;
            case STRING_LITERAL: 
                state = handleStringLiteral(c, &line_number);
                previous_state = STRING_LITERAL;
                break;
            case CHAR_LITERAL: 
                state = handleCharLiteral(c, &line_number);
                previous_state = CHAR_LITERAL;
                break;
            case ESCAPE: 
                state = handleEscapeState(c, previous_state);
                break;
            case REJECT: 
                return EXIT_FAILURE;
        }

        if (state == IN_COMMENT && prev_state != IN_COMMENT) {
            comment_start_line = line_number;
        }
    }

    if (state == IN_COMMENT || state == ASTERISK) {
        report_error(comment_start_line);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
