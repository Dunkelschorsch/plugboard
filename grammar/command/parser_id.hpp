#ifndef GRAMMAR_COMMAND_PARSER__HPP
#define GRAMMAR_COMMAND_PARSER__HPP


namespace parserID
{
    // literals
    const int integer = 1;
    const int real = 2;
    const int complex = 3;
    const int string = 4;

    const int primitive = 5;

    // arrays
    const int range = 6;
    const int array = 7;
    const int rowvec = 8;

    const int variable = 9;

    const int identifier = 10;

    // arithmetic
    const int scalarfactor = 11;
    const int scalarterm = 12;
    const int scalarexpression = 13;
    const int scalarpot = 14;

    const int assignment = 50;

    // commands
    const int command = 1000;
    const int addblock = 1001;
    const int connect = 1002;
    const int run = 1003;

    // start rule
    const int first = 10000;

    const int openparen = 2001;
    const int closeparen = 2002;
}

#endif // GRAMMAR_COMMAND_PARSER__HPP
