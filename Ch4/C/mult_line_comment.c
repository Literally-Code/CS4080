else if (peekNext() == '*')
{
    // Consume the opening /*
    advance(); // /
    advance(); // *

    int nest_depth = 1;

    while (nest_depth > 0 && !isAtEnd())
    {
        if (peek() == '/' && peekNext() == '*')
        {
            // Consume /*
            advance();
            advance();
            nest_depth++;
        }
        else if (peek() == '*' && peekNext() == '/')
        {
            // Consume */
            advance();
            advance();
            nest_depth--;
        }
        else
        {
            if (peek() == '\n')
            {
                scanner.line++;
            }

            advance();
        }
    }

    break;
}
