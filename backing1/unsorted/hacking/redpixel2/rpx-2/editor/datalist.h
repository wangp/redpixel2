#ifndef __included_datalist_h
#define __included_datalist_h

void *rpx_load_data(char *filename, char *branch);
void rpx_unload_data(void *ptr);

void rpx_data_init();
void rpx_data_cleanup();

#endif /* __included_datalist_h */
