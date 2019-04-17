# redis-modules
redis modules

## move module to redis source code directory
```
  cp modulename.c $RedisSource/src/modules/
```

## compile module
```
gcc -fPIC -std=gnu99 -c -o modulename.o modulename.c
ld -o modulename.so modulename.o -shared -Bsymbolic -lc
```

## load module
```
127.0.0.1:6379> module load $RedisSource/src/modules/modulename.so
OK
```

## module list
### pushpersistenceinfo.c
* pushpersistenceinfo.timer: open a 5s timer to publish persistence info to **pinfo** channel
* pushpersistenceinfo.stop: stop the timer
