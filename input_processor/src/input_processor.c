#include "input_processor.h"

void copy_option_argument(char** destination, enum OPTS_WITH_ARGS_INDEX_ORDER index, char* source)
{
    destination[index] = (char*) malloc((strlen(source) + 1) * sizeof(char));
    if (NULL == destination[index])
    {
        exit(ENOMEM);
    }
    strcpy(destination[index], source);
}

FC parse_input(int argc, char* argv[], char* optsargs[], FC flags)
{
    const char* const OPTS_STRING = ":au:n:f:";

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTS_STRING)) != -1)
    {
        switch (opt)
        {
        case 'a':
            flags.opt_flags |= OPT_a_FLAG;
            break;
        case 'u':
            copy_option_argument(optsargs, OPT_u_INDEX, optarg);
            flags.opt_flags |= OPT_u_FLAG;
            break;
        case 'n':
            copy_option_argument(optsargs, OPT_n_INDEX, optarg);
            flags.opt_flags |= OPT_n_FLAG;
            break;
        case 'f':
            copy_option_argument(optsargs, OPT_f_INDEX, optarg);
            flags.opt_flags |= OPT_f_FLAG;
            break;
        case '?':
            flags.errors_flags |= UNKNOWN_OPTION_ERROR;
            fprintf(stderr, "error: Unknown option: %c.\n", optopt);
            break;
        case ':':
            flags.errors_flags |= ARG_REQUIRED_ERROR;
            fprintf(stderr, "error: Option %c requires an argument.\n", optopt);

            switch (optopt)
            {
            case 'u':
                flags.opt_flags |= OPT_u_FLAG;
                break;
            case 'n':
                flags.opt_flags |= OPT_n_FLAG;
                break;
            }
            
            break;
        }
    }
    return flags;
}

FC check_for_mandatory_option(FC flags)
{
    if (!(flags.opt_flags & (OPT_a_FLAG | OPT_u_FLAG | OPT_n_FLAG)))
    {
        fprintf(stderr, "error: One of these options needs to be chosen: -a, -u, -n.\n");
        flags.errors_flags |= MANDATORY_OPT_ERROR;
    }
    return flags;
}

FC check_for_options_exclusivity(FC flags)
{
    const int MAX_EXCLUSIVE_OPTS_COUNT = 1;

    if ((((flags.opt_flags & OPT_a_FLAG) > 0) + ((flags.opt_flags & OPT_u_FLAG) > 0) + 
            ((flags.opt_flags & OPT_n_FLAG) > 0)) > MAX_EXCLUSIVE_OPTS_COUNT)
    {
        fprintf(stderr, "error: Only one of these options can be chosen: -a, -u, -n.\n");
        flags.errors_flags |= EXCLUSIVE_OPTS_ERROR;
    }
    return flags;
}

FC check_for_non_optional_arguments(int argc, char* argv[], FC flags)
{
    if (optind < argc)
    {
        fprintf(stderr, "error: Program does not accept non-optional arguments:");
    
        while (optind < argc)
        {
            printf(" \"%s\"", argv[optind++]);
        }
        printf(".\n");
        flags.errors_flags |= NON_OPT_ARG_ERROR;
    }

    return flags;
}

bool error_occurred(FC flags)
{
    return flags.errors_flags > 0;
}

char** allocate_memory_for_option_arguments(unsigned args_count)
{
    char** optargs = (char**) calloc(args_count, sizeof(char*));
    if (NULL == optargs)
    {
        exit(ENOMEM);
    }

    return optargs;
}

void deallocate_option_arguments_memory(char** optargs, unsigned args_count)
{
    for (unsigned i = 0; i < args_count; i++)
    {
        free(optargs[i]);
    }
    
    free(optargs);
}