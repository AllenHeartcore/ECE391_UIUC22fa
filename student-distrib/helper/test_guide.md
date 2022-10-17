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

### Helpers
| File | Content |
| - | - |
| `tests.c` | Test points |
| `lib.c` | Basic library functions |
| `types.h` | Type declarations |
| `debug.h`<br>`debug.sh` | Useful debugging<br>macros and script |

## Testing Guidelines

### **IMPORTANT NOTES**
- ***Backup** `mp3.img` in a safe place and **restore** it each time the OS crashes.<br>Also, keep an eye on the git change log to identify if the image has been corrupted.*
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
| | Checking bad input<br>and return values | |
