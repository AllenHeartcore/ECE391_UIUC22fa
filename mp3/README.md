# ZinixOS: A Unix-like OS for x86
By the collaboration of [Ziyuan Chen](https://github.com/AllenHeartcore), [Zhirong Chen](https://github.com/rong-hash), [Zicheng Ma](https://github.com/ZichengMa), [Shihua Zeng](https://github.com/bekaboo) <br>
UIUC ECE391 FA22 | October 2022 ~ December 2022


File Structure
-----
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
| `i8259.c` | PIC chip driver |
| `keyboard.c` | Keyboard driver |
| `rtc.c` | RTC driver |
| `page.c` | Paging settings |

### Checkpoint 2
| File | Content |
| - | - |
| `terminal.c` | Terminal driver |
| `filesys.c` | Read-only file system |

### Checkpoint 3 & 4
| File | Content |
| - | - |
| `syscall.c` | System call handler |

### Checkpoint 5
| File | Content |
| - | - |
| `scheduler.c` | Round-robin scheduler |
| `pit.c` | PIT chip driver |

### Extra Credit
| File | Content |
| - | - |
| `memory.c` | Dynamic memory allocation |
| `speaker.c` | Speaker driver |
| `signal.c`<br>`signal_lnk.S` | Signal handler |
| `ata.c` | ATA hard disk driver |

### Helpers
| File | Content |
| - | - |
| `tests.c` | Test points |
| `lib.c` | Basic library functions |
| `types.h` | Type declarations |
| `debug.h`<br>`debug.sh` | Useful debugging<br>macros and script |


Testing Guidelines
-----
***IMPORTANT**: Backup `mp3.img` in a safe place and restore it each time the OS crashes.*

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


Work Distribution
-----
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
    <tr> <td rowspan="1"> <b>Ckpt. 4</b> </td> <td> Syscall (GV) </td> <td> <code>master</code> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>
    <tr> <td rowspan="2"> <b>Ckpt. 5</b> </td> <td> Multi Terminals </td> <td> <code>multi_terminal</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> </tr>
    <tr> <td> Scheduling </td> <td> <code>multi_terminal</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> ○ </td> <td align="center"> ○ </td> </tr>
    <tr> <td rowspan="4"> <b>Extra</b> </td> <td> Dynamic Malloc </td> <td> <code>memory</code> </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> </tr>
    <tr> <td> Speaker </td> <td> <code>multi_terminal</code> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> </td> <td align="center"> </td> </tr>
    <tr> <td> Signal </td> <td> <code>signal</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> </td> </tr>
    <tr> <td> File Write </td> <td> <code>filesystem</code> </td> <td align="center"> </td> <td align="center"> ○ </td> <td align="center"> </td> <td align="center"> ○ </td> </tr>
</table>


ACADEMIC INTEGRITY
-----
Please review the University of Illinois Student Code before starting,
particularly all subsections of Article 1, Part 4 Academic Integrity and Procedure [here](http://studentcode.illinois.edu/article1_part4_1-401.html).

**§ 1‑402 Academic Integrity Infractions**

(a).	Cheating. No student shall use or attempt to use in any academic exercise materials, information, study aids, or electronic data that the student knows or should know is unauthorized. Instructors are strongly encouraged to make in advance a clear statement of their policies and procedures concerning the use of shared study aids, examination files, and related materials and forms of assistance. Such advance notification is especially important in the case of take-home examinations. During any examination, students should assume that external assistance (e.g., books, notes, calculators, and communications with others) is prohibited unless specifically authorized by the Instructor. A violation of this section includes but is not limited to:

(1)	Allowing others to conduct research or prepare any work for a student without prior authorization from the Instructor, including using the services of commercial term paper companies. 

(2)	Submitting substantial portions of the same academic work for credit more than once or by more than one student without authorization from the Instructors to whom the work is being submitted. 

(3) Working with another person without authorization to satisfy an individual assignment.

(b) Plagiarism. No student shall represent the words, work, or ideas of another as his or her own in any academic endeavor. A violation of this section includes but is not limited to:

(1)	Copying: Submitting the work of another as one’s own. 

(2)	Direct Quotation: Every direct quotation must be identified by quotation marks or by appropriate indentation and must be promptly cited. Proper citation style for many academic departments is outlined in such manuals as the MLA Handbook or K.L. Turabian’s A Manual for Writers of Term Papers, Theses and Dissertations. These and similar publications are available in the University bookstore or library. The actual source from which cited information was obtained should be acknowledged.

(3)	Paraphrase: Prompt acknowledgment is required when material from another source is paraphrased or summarized in whole or in part. This is true even if the student’s words differ substantially from those of the source. A citation acknowledging only a directly quoted statement does not suffice as an acknowledgment of any preceding or succeeding paraphrased material. 

(4)	Borrowed Facts or Information: Information obtained in one’s reading or research that is not common knowledge must be acknowledged. Examples of common knowledge might include the names of leaders of prominent nations, basic scientific laws, etc. Materials that contribute only to one’s general understanding of the subject may be acknowledged in a bibliography and need not be immediately cited. One citation is usually sufficient to acknowledge indebtedness when a number of connected sentences in the paper draw their special information from one source.

(c) Fabrication. No student shall falsify or invent any information or citation in an academic endeavor. A violation of this section includes but is not limited to:

(1)	Using invented information in any laboratory experiment or other academic endeavor without notice to and authorization from the Instructor or examiner. It would be improper, for example, to analyze one sample in an experiment and covertly invent data based on that single experiment for several more required analyses. 

(2)	Altering the answers given for an exam after the examination has been graded. 

(3)	Providing false or misleading information for the purpose of gaining an academic advantage.

(d)	Facilitating Infractions of Academic Integrity. No student shall help or attempt to help another to commit an infraction of academic integrity, where one knows or should know that through one’s acts or omissions such an infraction may be facilitated. A violation of this section includes but is not limited to:

(1)	Allowing another to copy from one’s work. 

(2)	Taking an exam by proxy for someone else. This is an infraction of academic integrity on the part of both the student enrolled in the course and the proxy or substitute. 

(3)	Removing an examination or quiz from a classroom, faculty office, or other facility without authorization.

(e)	Bribes, Favors, and Threats. No student shall bribe or attempt to bribe, promise favors to or make threats against any person with the intent to affect a record of a grade or evaluation of academic performance. This includes conspiracy with another person who then takes the action on behalf of the student.

(f)	Academic Interference. No student shall tamper with, alter, circumvent, or destroy any educational material or resource in a manner that deprives any other student of fair access or reasonable use of that material or resource. 

(1)	Educational resources include but are not limited to computer facilities, electronic data, required/reserved readings, reference works, or other library materials. 

(2)	Academic interference also includes acts in which the student committing the infraction personally benefits from the interference, regardless of the effect on other students.


LEGAL
-----
Permission to use, copy, modify, and distribute this software and its
documentation for any purpose, without fee, and without written agreement is
hereby granted, provided that the above copyright notice and the following
two paragraphs appear in all copies of this software.

IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE

PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
