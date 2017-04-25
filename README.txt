This program is sogang university's system programming project #1 Work.

>> In bash
Complie command : make
Execution command : ./20151571.out
Clean object code and execute file command : make clean

>> In sic simulator
Commands that can use in sicsim
	- Shell commands
		- h[elp]
			- Show available commands in sicsim.
		- hi[story]
			- Show history of valid user inputs.
		- d[ir]
			- Show current directory's files and directories.
		- q[uit]
			- quit sicsim machine program.
	- Memory commands
		- du[mp] [start, end]
			- Show contents of memory
			- options
				- default option (option 1) : du[mp]
					- Show contents of memory, printing next to previous memory address
					- Default showing amount : 160 Bytes.
					- Default previous address : 0
					- If previous memory address is 0xFFFFF, it starts from address '0'
					- If previous memory address is bigger than 0xFFF00, printing from previous memory address to 0xFFFFF
				- option 2 : du[mp] start
					- Show contents of memory, printing from start address
					- Default showing amount : 160 Bytes.
					- Save end address of printing memory as previous memory address
					- If start memory address is bigger than 0xFFF00, printing from previous memory address to 0xFFFFF
				- option 3 : du[mp] start, end
					- Show contents of memory, printing from start to end
					- [Warning!]
						- You must include only one comma(',') between start and end
						- Do not include comma(',') at the end of end address
						- Start address must be more than end address
						- ex)
							- du 4, 4, (X)
							- du ,4, 4 (X)
							- du 4  4  (X)
							- du 4, 4  (O)
							- du 4 , 4 (O)
							- du 4,4   (O)
					- Save end as previous memory address
			- It doesn't save invalid command's previous memory address
			- At option 1 and 2, if ending address is above than 0xFFFFF, ending address is automatically set as 0xFFFFF
		- e[dit] address, value
			- Edit memory as input value at that address
		- f[ill] start, end, value
			- Fill memory as input value from start to end
		- reset
			- Reset all memory as '0x00'
	- Opcode commands
		- opcodelist
			- Show opcodes in hash table
		- opcode mnemonic
			- Show opcode of mnemonic

    - assemble filename
        - Assembling file
    - type filename
        - Show file contents
    - symbol
        - Show the recent symbol table of the successful assemble file

    - Link following files
            - If file doesn't exist or can't link, loader will not work
        - bp
          - Set breakpoint
            - When run, if PC reach to bp, break execution and show registers
        - run
          - Run program
            - If loader successed, starting address of run is progaddr(start of
              program)
            - end of run is start address + program length
        - progaddr
          - Set loader start point
            - If you run, set start address as progaddr value
    
     -progaddr
        -Set loader start point

     -loader
         - Link following files
            - If file doesn't exist or can't link, loader will not work
            - If restart loader, reset all memory, and ESTAB.
    
     - run
        - Run program

    -debug
        -bp
            - bp [address]
                - Set breakpoint
            - bp clear
                - Clear all breakpoint
            - bp
                - Printing all breakpoint
    
