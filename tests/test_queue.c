#include "storage.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
int main(void){char d[]="/tmp/spds-queue-XXXXXX";assert(mkdtemp(d));assert(spds_storage_init(d)==0);unsigned q=0,n=0;for(unsigned i=1;i<=10;i++){SpdsRecord r={1000+i,1700000000+i,i,"Guest"};assert(spds_storage_receive(d,&r,9999,1700000000+i)==1);assert(spds_storage_queue_count(d,&q)==0&&q==i);}SpdsRecord extra={2000,1700000100,1,"Overflow"};assert(spds_storage_receive(d,&extra,9999,1700000100)==SPDS_STORAGE_QUEUE_FULL);assert(spds_storage_total(d,&n)==0&&n==10);assert(spds_storage_queue_clear(d)==0);assert(spds_storage_queue_count(d,&q)==0&&q==0);assert(spds_storage_total(d,&n)==0&&n==10);for(unsigned i=11;i<=20;i++){SpdsRecord r={1000+i,1700000200+i,i,"Guest"};assert(spds_storage_receive(d,&r,9999,1700000200+i)==1);}assert(spds_storage_queue_count(d,&q)==0&&q==10);SpdsRecord dup={1001,1700000300,1,"Guest"};assert(spds_storage_receive(d,&dup,9999,1700000300)==0);/* Simulate application restart: close all operations, initialize the same
 * persistent directory again, and confirm queued visitors and cooldown data
 * survive without relying on process-global state. */
assert(spds_storage_init(d)==0);
assert(spds_storage_queue_count(d,&q)==0&&q==10);
assert(spds_storage_total(d,&n)==0&&n==20);
SpdsRecord persisted_dup={1001,1700000400,99,"Renamed"};
assert(spds_storage_receive(d,&persisted_dup,9999,1700000400)==0);
assert(spds_storage_queue_count(d,&q)==0&&q==10);
SpdsRecord after_restart={3000,1700000401,3,"AfterRestart"};
assert(spds_storage_receive(d,&after_restart,9999,1700000401)==SPDS_STORAGE_QUEUE_FULL);
puts("PASS ten visitor queue, restart persistence and cooldown");return 0;}