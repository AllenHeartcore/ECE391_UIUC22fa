### P1 Solution

1. Drawing a Non-Scrolling Status Bar
    - Registers
        - The `StartAddr` Register: `NumLinesBottom` * `BytesPerRow`
        - The `AttrModeCtrl` Register: Bit 5 (`PPM`) should be set to 1
        - The `LineCompare` field: scan line address of horizontal division
            - Bit 9 - `MaximumScanLine` Register
            - Bit 8 - `Overflow` Register
            - Bit 7:0 - `LineCompare` Register (`NumLinesTop`, or index of division)
    - Operations
        - `StartAddr` specifies……
            - (text mode -) the location of the first character, or
            - (graphics mode -) the position of the first byte of pixels
        - When the line counter's value reaches `LineCompare`……
            - the current line address is reset to 0
            - If `PixelPanningMode` is 1 then `PixelShiftCount` and `BytePanning`<br>are reset to 0 for the remainder of the display cycle
    - Constraints
        - Either both windows are panned by the same amount, or only the top pans
            - In this case, the bottom's panning values are fixed at 0
            - This effectively **limits the static status bar to the bottom**
        - The bottom window's "starting" memory address is fixed at 0
            - So the status bar must locate *before* the main window in the memory
        - The `PresetRowScan` field only applies to the top window
            - This means that **pixel-level scrolling** is only enabled for the top
            - The bottom has an effective `PresetRowScan` value of 0.

2. Changing the Color Palette
    - Registers
        - `DACAddrWriteMode` Register: the palette entry's index value
        - `DACData` Register: the palette entry's color values
    - Operations
        - `CLI()`
        - `DACAddrWriteMode` <- index value
        - `DACData` <- red value
        - `DACData` <- green value
        - `DACData` <- blue value
            - *Note: here the internal writing address automatically advances*
        - `STI()`
