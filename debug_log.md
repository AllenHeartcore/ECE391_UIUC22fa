| Type | Finding it takes | Fixing it takes | Description | Solution |
| ---- | ---------------- | --------------- | ----------- | -------- |
| Referencing | | | `show_status(game_info.where->name);` | |
| VGA | 5 mins | 10 mins | Wrong `LineCmp` value | Multiply the value by 2 |
| Addressing | 5 mins | 15 mins | Missing status bar | Move status bar's address before the image |
| Addressing | 2 mins | 10 mins | Accidential SegFault | Set the bar `buffer` as static<br>(Otherwise, there's a warning of<br>"returning address of local variable")
| Addressing | 5 mins | | Glitching image | |