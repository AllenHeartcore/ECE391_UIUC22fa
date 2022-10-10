| Type | Finding it takes | Fixing it takes | Description | Solution |
| ---- | ---------------- | --------------- | ----------- | -------- |
| **CKPT 1** |
| VGA | 5 mins | 10 mins | Wrong `LineCmp` value | Multiply the value by 2 |
| Addr | 2 mins | 10 mins | Accidential SegFault, plus the warning<br>"returning address of local variable" | Set the bar `buffer` as static |
| Addr | 3 mins | 20 mins | Bar overlapped with image | Copy the bar to `0x0000` with a modified `copy_bar` |
| Addr | 1 min | 10 mins | Bar appears stripped | Add a "for" loop to copy 4 planes of bar |
| Addr | 1 min | 30 mins | Glitching image | Set the starting addr of `target_img` to `0x0800`<br>(Reserve space for bar) |
| Buffer | 8 mins | 40 mins | Scattered font | Change array indexing from linear to plane-based |
| Msg | 1 min | 30 mins | "Deref. ptr to incomplete type"<br>when reading room name | Turn from `game_info.where->name`<br>to `room_name(game_info.where)` |
| Sync | 5 mins | 45 mins | Ignored status message | Turn from `show_status(status_msg)`<br>to `show_bar(status_msg)` (oh man)<br>Also, add locks before & after the read |
| GCC | 2 mins | 2 mins | "Discards qualifiers from ptr" | Add `const` before user-defined `char`s |
| **CKPT 2-1** |
| Tux | 5 mins | 20 mins | Polling buttons | Reserve a global `buttons` var;<br>Add handler for `MTCP_BIOC_EVENT` |
| C90 | 2 mins | 30 mins | ISO C90 forbids mixed<br>declarations and code | Pack `case`s up as helper functions |
| Tux | 3 mins | 50 mins | `EINVAL` when sending command | Compile and install the driver first |
| Tux | 3 mins | 90 mins | Receive no response packet | Add an infinite loop in the test |
| Tux | 5 mins | 60 mins | Erroneous button mapping | `BIOC_EVENT` returns `80 80` when idle!<br>Map this case to `FF` by default |
| Tux | 5 mins | 40 mins | Button status no available to user | Retain button status if `BIOC_EVENT`<br>sends `80 80` to avoid transient signals |
| Tux | 3 mins | ------ | Clock advances per 3 seconds | **UNRESOLVED** |
| Tux | 10 mins | ------ | Buttons not in sync with ticks | **UNRESOLVED** |
| Tux | 5 mins | ------ | SegFault when cannot go<br>a certain way in Tux | **UNRESOLVED** |
| **CKPT 2-2** |
| GCC | 2 mins | 50 mins | "Deref. ptr to incomplete type"<br>when quoting from `modex.c` | Define `fill_palette_optim` non-`static`ly,<br>`extern` it in `photo.h`, and call it in `photo.c` |
| Array | 5 mins | 15 mins | Faulty `octree` index | Wrap an extra parenthesis around<br>`(pixel >> 11) & 0x1F` before `<< 1` |
| Mem | 5 mins | 15 mins | Cannot access original image data<br>in the second go | Add an extra `img_raw` array for storage<br>since the file pointer cannot move backwards |
| DType | 5 mins | 20 mins | An infinite loop when initializing<br>`covered` of size 387 * 290 = 112,230 | Change the iterator to type `uint32_t`<br>(`uint16_t` worked for 320 * 200. Tricky!) |
| Mem | 2 mins | 10 mins | SegFault when processing more images<br>(`malloc` failed at later stages) | `free` the index recorders before returning<br>(DO NOT free non-`malloc`ed arrays!!!) |
| DType | 20 mins | 60 mins | `covered` fails to mark<br>lower half of image | Change `octree[i]->affiliates` to `uint32_t`<br>(OVERRIDEN by latter changes) |
| Array | 10 mins | 30 mins | Misplaced image parts and<br>(more) accidential SegFaults | Turned to a unified `affiliation` array<br>to prevent excessive memory usage |
| Array | 5 mins | 20 mins | Noisy image & rare colors | Record the `node_id` in each node<br>so that it preserves after sorting |