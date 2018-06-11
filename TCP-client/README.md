TCP-Client
===========

This is a simple TCP-Client making an outgoing connection to `aaaa::1` on port `1234`.

The following settings have been applied to `platform/minimal-net/contiki_conf.h`:

```
#undef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL 0
#define RPL_BORDER_ROUTER 0
#define HARD_CODED_ADDRESS   "aaaa::10"
```


The program is built and started on the `minimal-net` platform like this:

```
make TARGET=minimal-net
sudo ./tcp-client.minimal-net

ifconfig tap0 up
*******Contiki-3.x-3070-g616b65c online*******
IPV6 Addresss: [aaaa::ff:fe00:10]
IPV6 Addresss: [fe80::ff:fe00:10]
# # # # # # #
TCP client process started. 
  Configure TAP-interface and press key to continue... 

```
  
The program then stops to allow the user to configure the local TAP interface:

    sudo ip -6 addr add aaaa::1/64 dev tap0

A local `netcat` session can be started to emulate a server that the client connects to and see output of the program:

    nc -6 -l -p 1234
