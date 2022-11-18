## File Structure

### Booting
| File | Content |
| - | - |
| `mp3.img`<br>`bootimg`<br>`filesys_img` | Boot images |
| `boot.S` | First instructions of OS |
| `kernel.c` | Contains `entry` called from ^ |
| `x86_desc.S` | Descriptor organization |
| `multiboot.h` | GRUB compliance |

### Checkpoint 1
| File | Content |
| - | - |
| `idt.c`<br>`idt_lnk.S` | IDT initialization<br>with assembly linkage |
| `i8259.c` | PIC initialization |
| `keyboard.c` | Keyboard initialization |
| `rtc.c` | RTC initialization |
| `page.c` | Paging initialization |

### Checkpoint 2
| File | Content |
| - | - |
| `terminal.c` | Terminal driver |
| `filesys.c` | Read-only file system |

### Checkpoint 3 & 4
| File | Content |
| - | - |
| `syscall.c` | System call handler |

### Helpers
| File | Content |
| - | - |
| `tests.c` | Test points |
| `lib.c` | Basic library functions |
| `types.h` | Type declarations |
| `debug.h`<br>`debug.sh` | Useful debugging<br>macros and script |

<br>

---

## Testing Guidelines

### **IMPORTANT NOTES**
- ***Backup** `mp3.img` in a safe place and **restore** it each time the OS crashes.*
- *An exception immediately freezes the system and should appear as the **last** testpoint.*

### Checkpoint 1
| Type | Tested Functionality | Testing Method |
| - | - | - |
| IDT | Values contained in IDT array | Run `idt_test` |
| IDT | Handling exceptions | Run `div0_test`<br>***(This should come last)*** |
| Keyboard | Interpreting scancodes in<br>the keyboard handler | Type & echo characters<br>on the screen |
| RTC | Receiving an RTC interrupt | Add `test_interrupt();`<br>to `rtc.c:76` |
| Paging | Values contained in<br>paging structures | In QEMU, Ctrl+Alt+2<br>and type `info mem` |
| Paging | Dereferencing address ranges | Run `page_test` and<br>`..._deref_not_exist` |
| Paging | Dereferencing NULL to<br>produce a page fault | Run `..._deref_null` |

### Checkpoint 2
| Type | Tested Functionality | Testing Method |
| - | - | - |
| RTC | Change all possible frequencies<br>and receive interrupts | Run `rtc_driver_test` |
| Terminal | Read user input from keyboard | Run `terminal_kbd_test_echo`,<br>type characters, press Enter,<br>and examine the echoed string |
| Terminal | Write different sized strings<br>to the terminal | Run `terminal_kbd_test_newline`<br>and examine the echoed string<br>(Also change `nbytes` in argument) |
| Filesys | Check whether a file exists | Run `read_file_name_test` |
| Filesys | Print out the contents of<br>different sized files | Run `read_data_test`<br>and examine the echoed string |
| Filesys | Print out a list of all files<br>in the file system | Run `read_directory_test` |
| Filesys | Filesys driver functions | Run `*_file_test` (For now,<br>they should all PASS except for<br>nonexistant filenames in `read`) |

### Checkpoint 3
| Type | Tested Functionality | Testing Method |
| - | - | - |
| Syscall | Sanity check | Run `syserr` (#6 will fail since `vidmap`<br>is not implemented) |
| Syscall | Execute a program | Run `shell` |
| Syscall | Halt a program | Run `exit` |
| Syscall | Read from the terminal | Run `hello` |
| Syscall | Write to the terminal | Run `testprint`, `counter`, `pingpong` |

### Checkpoint 4
| Type | Tested Functionality | Testing Method |
| - | - | - |
| Syscall | Multiple shells | Run `shell` on top of another |
| Syscall | Get arguments | Run `cat <filename>` |
| Syscall | Map video memory | Run `fish` |

<br>

---

## Work Distribution

<table>
    <tr> <td rowspan="2"> <b>Checkpoint</b> </td> <td rowspan="2"> <b>Functionality</b> </td> <td rowspan="2"> <b>Branch</b> </td> <td colspan="4" align="center"> <b>Group Members</b> </td> </tr>
    <tr> <td> <b>Ziyuan Chen</b> </td> <td> <b>Zhirong Chen</b> </td> <td> <b>Zicheng Ma</b> </td> <td> <b>Shihua Zeng</b> </td> </tr>
    <tr> <td rowspan="3"> <b>Ckpt. 1</b> </td> <td> GDT, IDT </td> <td> <code>gdt_idt</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> </tr>
    <tr> <td> Init Devices </td> <td> <code>device</code> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> ○ </td> </tr>
    <tr> <td> Init Paging </td> <td> <code>paging</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>
    <tr> <td rowspan="3"> <b>Ckpt. 2</b> </td> <td> RTC Driver </td> <td> <code>rtc_driver</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> </tr>
    <tr> <td> Terminal </td> <td> <code>terminal_kbd</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>
    <tr> <td> File System </td> <td> <code>filesystem</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> ○ </td> <td align="center"> </td> </tr>
    <tr> <td rowspan="2"> <b>Ckpt. 3</b> </td> <td> Syscall (EH) </td> <td> <code>syscall_eh</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> </tr>
    <tr> <td> Syscall (OCRW) </td> <td> <code>syscall_eh</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>
    <tr> <td rowspan="1"> <b>Ckpt. 4</b> </td> <td> Syscall (GV) </td> <td> <code>master</code> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>align="center"> </td> </tr>
    <tr> <td rowspan="2"> <b>Ckpt. 5</b> </td> <td> Multi Terminals </td> <td> <code>multi_terminal</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> </tr>
    <tr> <td> Scheduling </td> <td> <code>multi_terminal</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> ○ </td> <td align="center"> ○ </td> </tr>
    <tr> <td rowspan="1"> <b>Extra</b> </td> <td> Syscall (SS) </td> <td> <code></code> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> <td 
