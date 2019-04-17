#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>



RedisModuleString *infoStr;
int off=0 ;
void  getPersistenceStatus(RedisModuleCtx *ctx,RedisModuleString **infoStr);
void  publishPersistenceStatus(RedisModuleCtx *ctx,RedisModuleString **infoStr);

/* Timer callback. */
void timerHandler(RedisModuleCtx *ctx,void *data) {
    if(off == 1) return;
    REDISMODULE_NOT_USED(ctx);
    getPersistenceStatus(ctx,&infoStr);
    publishPersistenceStatus(ctx,&infoStr);
    RedisModule_CreateTimer(ctx,5000,timerHandler,NULL);

}

int TimerCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    off = 0;
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);
    RedisModule_AutoMemory(ctx);
    getPersistenceStatus(ctx,&infoStr);
    publishPersistenceStatus(ctx,&infoStr);

    RedisModule_CreateTimer(ctx,5000,timerHandler,NULL);
    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
int TimerStopCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    off = 1;
    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}

void  getPersistenceStatus(RedisModuleCtx *ctx,RedisModuleString **infoStr){
    RedisModuleCallReply *reply;

    reply = RedisModule_Call(ctx,"info","c","Persistence");

    *infoStr = RedisModule_CreateStringFromCallReply(reply);
}

void  publishPersistenceStatus(RedisModuleCtx *ctx,RedisModuleString **infoStr){
    RedisModule_Call(ctx,"publish","cs","pinfo",*infoStr);
}

/* This function must be present on each Redis module. It is used in order to
 * register the commands into the Redis server. */
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

    if (RedisModule_Init(ctx,"pushpersistenceinfo",1,REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"pushpersistenceinfo.timer",
        TimerCommand_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"pushpersistenceinfo.stop",
        TimerStopCommand_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}