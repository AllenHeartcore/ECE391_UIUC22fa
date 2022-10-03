| Type | Finding it takes | Fixing it takes | Description | Solution |
| ---- | ---------------- | --------------- | ----------- | -------- |
| VGA | 5 mins | 10 mins | Wrong `LineCmp` value | Multiply the value by 2 |
| Addr | 2 mins | 10 mins | Accidential SegFault | Set the bar `buffer` as static (Otherwise, there's a<br>warning of "returning address of local variable")
| Addr | 3 mins | 20 mins | Overlapping bar | Copy the bar to `0x0000` with a modified `copy_bar` |
| Addr | 1 min | 10 mins | Stripped bar | Add a "for" loop to copy 4 planes of bar |
| Addr | 1 min | 30 mins | Glitching image | Set the starting addr of `target_img` to `0x0800`<br>(Reserve space for bar) |
| Buffer | 8 mins | 40 mins | Scattered font | Change array indexing from linear to plane-based |
| Msg | 1 min | 30 mins | "Deref. ptr to incomplete type"<br>when reading room name | Turn from `game_info.where->name`<br>to `room_name(game_info.where)` |
| Sync | 5 mins | 45 mins | Ignored status message | Turn from `show_status(status_msg)`<br>to `show_bar(status_msg)` (oh man)<br>Also, add locks before & after the read |
| GCC | 2 mins | 2 mins | "Discards qualifiers from ptr" | Add `const` before user-defined `char`s |
| GCC | 2 mins | 50 mins | "Deref. ptr to incomplete type"<br>when quoting from `modex.c` | Define `fill_palette_optim` non-`static`ly,<br>`extern` it in `photo.h`, and call it in `photo.c` |
| Array | 5 mins | 15 mins | Faulty `octree` index | Wrap an extra parenthesis around<br>`(pixel >> 11) & 0x1F` before `<< 1` |
| Mem | 5 mins | 15 mins | Cannot access original image data<br>in the second go | Add an extra `img_raw` array for storage<br>since the file pointer cannot move backwards |
| Array | 5 mins | 20 mins | An infinite loop when initializing<br>`covered` of size `387 * 290 = 112,230` | Change the iterator to type `uint32_t`<br>(`uint16_t` worked for `320 * 200`. Tricky!) |
| Mem | 2 mins | 10 mins | SegFault when processing more images<br>(`malloc` failed at later stages) | `free` the index recorders before returning<br>(DO NOT free non-`malloc`ed arrays!!!) |
| Array | 30 mins | 90 mins | Misplaced image parts | Turned to a unified `affiliation` array<br>to prevent excessive memory usage |
| Array | 5 mins | 20 mins | Noisy image & rare colors | Record the `node_id` in each node<br>so that it preserves after sorting |