// a config info from CMAKE

#define PROJECT_NAME   ""
#define PROJECT_VESION "1.1.0"
#define SVN_VESION     ""
#define BUILD_TIME     __DATE__ " " __TIME__
#define BUILD_FLAGS    "/usr/bin/c++  -ggdb -g3 -fPIC -rdynamic  -fpermissive -Wall -Wextra -Wl,--eh-frame-hdr -msse -msse2 -msse3 -msse4.1  -std=c++0x  -g "

static void print_version()
{
    printf("PROJECT_NAME : %s\n",PROJECT_NAME);
    printf("PROJECT_VESION : %s\n",PROJECT_VESION);
    printf("SVN_VESION : %s\n",SVN_VESION);
    printf("BUILD_TIME : %s\n",BUILD_TIME);
    printf("BUILD_FLAGS: %s\n",BUILD_FLAGS);
}



static void print_help(char const * execname)
{
    printf("Usage:%s [option]\n", execname);
    printf("Options:\n");
    printf("  -h        print this message\n");
    printf("  -v        print the version\n");
    printf("  -f cfgfilepath\n");
}


static void parse_argv(int argc,char * argv[]){
    if(argc == 2 && 0 == strcmp(argv[1],"-v")){
        print_version();
        exit(0);
    }
}
