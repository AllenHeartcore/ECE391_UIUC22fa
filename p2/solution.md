### P2 Solution

1. Message direction: PC -> Tux Controller
    | Command | Send Time | Effect | Response |
    | - | - | - | - |
    | `MTCP_BIOC_ON` | Determined by user | Enables Button Interrupt-on-change<br> and generates a `MTCP_BIOC_EVENT` | `MTCP_ACK` |
    | `MTCP_LED_SET` | Determined by user | Sets the values to be displayed<br>by LED when it is in USR mode | `MTCP_ACK` |

2. Message direction: Tux Controller -> PC
    | Response | Send Time | Information |
    | - | - | - |
    | `MTCP_ACK` | After `MTCP_BIOC_ON`, `MTCP_BIOC_OFF`,<br>`MTCP_DBG_OFF`, and `MTCP_LED_SET` | The MTC successfully completes a command |
    | `MTCP_BIOC_EVENT` | As Byte 0 of the response packet<br>of `MTCP_BIOC_EVT` | The BIOC mode is enabled and a<br>button is either pressed or released |
    | `MTCP_RESET` | When (1) the device re-inits after a power-up,<br>(2) the `RESET` button is pressed, or<br>(3) Tux receives a `MTCP_RESET_DEV` command | The device has finished initializing itself |

3. **In short:** Because the function's caller is called from an interrupt context.
    - For the caller / the PC side / `tuxctl_ldisc_data_callback` / the `ioctl`,
        - it should not spend any time waiting for responses from the Controller
    - For the callee / the Controller side / `tuxctl_handle_packet`,
        - it should not sleep, wait for a semaphore, or wait for space in the buffer
    - There will be **9 ms** between the "call" and "response."
        - If an INT handler takes up too much time, it will drop data packets!
        - So the `ioctl`s should return immediately if the parameters are valid
