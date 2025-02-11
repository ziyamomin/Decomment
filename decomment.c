#include <stdio.h>
#include <stdlib.h>

/* Enum representing the possible states of the DFA. */
enum Statetype {
    START, /* Initial state before processing characters */
    FORWARD_SLASH, /* State is entered when a forward slash ('/') is encountered */
    IN_COMMENT, /* state is entered when the program is inside a comment */
    ASTERISK, /* state is entered when an asterisk ('*') is encountered inside a comment */
    STRING_LITERAL, /* state is entered when the program is inside a string literal (starting with '"') */
    CHAR_LITERAL, /* state is entered when the program is inside a character literal (starting with '\'') */
    ESCAPE_STRING, /* state is entered when an escape sequence is detected within a string literal */
    ESCAPE_CHAR, /* state is entered when an escape sequence is detected within a character literal */
    REJECT /* State represents an error or unhandled state */
};

/* This function prints a character to the standard output (stdout) stream. 
 * It is used to display each character processed by the DFA as it works through the input stream. 
 * The parameter `c` is the character that will be printed.
 * The function does not return any value.
 */
void print(char c) {
    putchar(c);
}

/* This function reports an error to the standard error (stderr) stream when an unterminated comment is found. 
 * If the program reaches the end of input while still inside a comment, it triggers this error message.
 * The parameter `line_number` is the line where the unterminated comment began.
 * The function does not return any value.
 */
void report_error(int line_number) {
    fprintf(stderr, "Error: line %d: unterminated comment\n", line_number);
}

/* This function handles the START state where the program processes the initial characters.
 * In this state, the program checks for the start of comments, string literals, and character literals, 
 * as well as handling normal characters and line breaks. 
 * It also updates the line_number pointer variable when a newline character is encountered.
 * The function takes in takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
enum Statetype handleStartState(int c, int *line_number, int *comment_start_line) {
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

/* This function handles the FORWARD_SLASH state, which is entered when a '/' is encountered.
 * In this state, the program checks if the '/' is part of a comment
 * or if it's simply a part of a line comment
 * If another '/' is encountered, the state repeats to handle line comments ('//'). 
 * If a newline is encountered, the program moves back to the START state.
 * If the program detects the start of a comment, it updates the comment_start_line parameter to record the line number where the comment begins.
 * The function takes in takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
enum Statetype handleForwardSlashState(int c, int *line_number, int *comment_start_line) {
    if (c == '*') {
        print(' ');
        *comment_start_line = *line_number;
        return IN_COMMENT;
    }
    if (c == '/') {
        print('/');
        return FORWARD_SLASH; 
    }
    if (c == '\n') {
        print('/');
        print(c);
        (*line_number)++;
        return START;
    }
    if (c == '"') {
        print('/');
        print(c);
        return STRING_LITERAL;
    }
    if (c == '\'') {
        print('/');
        print(c);
        return CHAR_LITERAL;
    } else {
        print('/');
        print(c);
    }
    return START;
}

/* This function handles the IN_COMMENT state, which is entered when the program is inside a comment.
 * In this state, the program processes the characters within the comment and waits for a closing '*' 
 * or continues to handle other characters within the comment.
 * If a newline is encountered, the line number is incremented, but the program stays in the IN_COMMENT state.
 * The function takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
enum Statetype handleInCommentState(int c, int *line_number) {
    if (c == '*') {
        return ASTERISK;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
        return IN_COMMENT;
    }
    return IN_COMMENT;
}

/* This function handles the ASTERISK state, which is entered when a '*' is encountered inside a comment.
 * In this state, the program looks for the closing '*' to end the comment, or it continues to stay inside the comment.
 * If a newline is encountered, the program stays inside the comment and increments the line number.
 * The function takes in takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
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
        return IN_COMMENT;
    }
    return IN_COMMENT;
}

/* This function handles the STRING_LITERAL state, which is entered when the program is inside a string literal.
 * In this state, the program processes characters inside the string, including escape sequences.
 * If an escape sequence is encountered (i.e., a backslash), the program transitions to the ESCAPE_STRING state.
 * If a closing '"' is encountered, the string literal ends, and the program returns to the START state.
 * The function takes in takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
enum Statetype handleStringLiteral(int c, int *line_number) {
    if (c == '\\') {
        print(c);
        return ESCAPE_STRING;
    }
    if (c == '"') {
        print(c);
        return START;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
        return STRING_LITERAL;
    } else {
        print(c);
    }
    return STRING_LITERAL;
}

/* This function handles the CHAR_LITERAL state, which is entered when the program is inside a character literal.
 * In this state, the program processes characters inside the character literal, including escape sequences.
 * If an escape sequence is encountered (i.e., a backslash), the program transitions to the ESCAPE_CHAR state.
 * If a closing '\'' is encountered, the character literal ends, and the program returns to the START state.
 * The function takes in takes in the parameter 'c' (the current character) and the int parameter line_number (the current line number).
 * It returns the new state after processing the character.
 */
enum Statetype handleCharLiteral(int c, int *line_number) {
    if (c == '\\') {
        print(c);
        return ESCAPE_CHAR;
    }
    if (c == '\'') {
        print(c);
        return START;
    }
    if (c == '\n') {
        print(c);
        (*line_number)++;
        return CHAR_LITERAL;
    } else {
        print(c);
    }
    return CHAR_LITERAL;
}

/* This function handles escape sequences inside both string literals and character literals.
 * It processes the escape character (backslash) and returns to the previous state after handling it.
 * The parameter `c` is the character after the escape sequence, and `previous_state` is the state before encountering the escape character.
 * The function returns the previous_state parameter (the previous state) after processing the escape character.
 */
enum Statetype handleEscapeState(int c, enum Statetype previous_state) {
    print(c);
    return previous_state;
}

/* The main function processes the input character by character and manages the DFA transitions.
 * It reads characters from the standard input and handles state transitions based on the current character and state.
 * If an unterminated comment is found, it reports an error and exits with failure.
 * The program returns EXIT_SUCCESS if no errors are detected and EXIT_FAILURE if an error occurs (e.g., unterminated comment).
 * The function does not take in any parameters.
 */
int main(void) {
    int c;
    enum Statetype state = START; /* Start in the START state */
    int line_number = 1; /* Initialize the line number to 1 */
    int comment_start_line = 0; /* Variable to keep track the line where the comment starts */

    while ((c = getchar()) != EOF) {
        switch (state) {
            case START:
                state = handleStartState(c, &line_number, &comment_start_line);
                break;
            case FORWARD_SLASH:
                state = handleForwardSlashState(c, &line_number, &comment_start_line);
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

    if (state == FORWARD_SLASH) {
        print('/');
    }
    /* If the program ends while still inside a comment, or if an asterisk state is left unresolved,
    the program reports an error for an unterminated comment and returns failure. */
    if (state == IN_COMMENT || state == ASTERISK) {
        report_error(comment_start_line);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}