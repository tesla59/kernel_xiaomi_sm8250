#ifndef _NETLINK_H_
#define _NETLINK_H_

void sendnlmsg(char *message);
int netlink_init(void);
void netlink_exit(void);

#endif // _NETLINK_H_
