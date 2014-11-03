// a config info from CMAKE

#define PROJECT_NAME   ""
#define PROJECT_VESION "1.1.0"
#define SVN_VESION     "Path: /home/guofutan/workspace/MTRpc_proj \nWorking Copy Root Path: /home/guofutan/workspace/MTRpc_proj \nURL: http://tc-svn.tencent.com/pub/pub_MTRpc_rep/MTRpc_proj/trunk \nRelative URL: ^/MTRpc_proj/trunk \nRepository Root: http://tc-svn.tencent.com/pub/pub_MTRpc_rep \nRepository UUID: d1354fd6-07e0-11e4-a2a2-158d611bd191 \nRevision: 212 \nNode Kind: directory \nSchedule: normal \nLast Changed Author: guofutan \nLast Changed Rev: 212 \nLast Changed Date: 2014-10-27 15:58:59 +0800 (Mon, 27 Oct 2014)"
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
