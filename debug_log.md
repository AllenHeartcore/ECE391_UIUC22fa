| Type | Finding it takes | Fixing it takes | Description | Solution |
| ---- | ---------------- | --------------- | ----------- | -------- |
| Ref | | | `show_status(game_info.where->name);` | |
| VGA | 5 mins | 10 mins | Wrong `LineCmp` value | Multiply the value by 2 |
| Addr | 2 mins | 10 mins | Accidential SegFault | Set the bar `buffer` as static (Otherwise, there's a<br>warning of "returning address of local variable")
| Addr | 3 mins | 20 mins | Overlapping bar | Copy the bar to `0x0000` with a modified `copy_bar` |
| Addr | 1 mins | 10 mins | Stripped bar | Add a "for" loop to copy 4 planes of bar |
| Addr | 1 mins | 30 mins | Glitching image | Set the starting addr of `target_img` to `0x0800`<br>(Reserve space for bar) |
| Buffer | 8 mins | 40 mins | Scattered font | Change array indexing from linear to plane-based |
| Sync | 5 mins | | Ignored status message | |