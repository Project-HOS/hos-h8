OUTPUT_FORMAT("coff-h8300")
OUTPUT_ARCH(h8300h)
ENTRY("_hos_start")
MEMORY
{
	vectors : o = 0x0000, 	l = 0x200
	rom    	: o = 0x0200, 	l = 0x1fdff
	ram    	: o = 0xffef10, l = 0x1000
	stack	: o = 0xffff0c, l = 0x4
}
SECTIONS 				
{ 					
.vectors : {
	LONG(ABSOLUTE(_hos_start)) /*  Reset */
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int07)?ABSOLUTE(int07):ABSOLUTE(_int_default))
	LONG(DEFINED(int08)?ABSOLUTE(int08):ABSOLUTE(_int_default))
	LONG(DEFINED(int09)?ABSOLUTE(int09):ABSOLUTE(_int_default))
	LONG(DEFINED(int0a)?ABSOLUTE(int0a):ABSOLUTE(_int_default))
	LONG(DEFINED(int0b)?ABSOLUTE(int0b):ABSOLUTE(_int_default))
	LONG(DEFINED(int0c)?ABSOLUTE(int0c):ABSOLUTE(_int_default))
	LONG(DEFINED(int0d)?ABSOLUTE(int0d):ABSOLUTE(_int_default))
	LONG(DEFINED(int0e)?ABSOLUTE(int0e):ABSOLUTE(_int_default))
	LONG(DEFINED(int0f)?ABSOLUTE(int0f):ABSOLUTE(_int_default))
	LONG(DEFINED(int10)?ABSOLUTE(int10):ABSOLUTE(_int_default))
	LONG(DEFINED(int11)?ABSOLUTE(int11):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int14)?ABSOLUTE(int14):ABSOLUTE(_int_default))
	LONG(DEFINED(int15)?ABSOLUTE(int15):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int18)?ABSOLUTE(int18):ABSOLUTE(_int_default))
	LONG(DEFINED(int19)?ABSOLUTE(int19):ABSOLUTE(_int_default))
	LONG(DEFINED(int1a)?ABSOLUTE(int1a):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int1c)?ABSOLUTE(int1c):ABSOLUTE(_int_default))
	LONG(DEFINED(int1d)?ABSOLUTE(int1d):ABSOLUTE(_int_default))
	LONG(DEFINED(int1e)?ABSOLUTE(int1e):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int20)?ABSOLUTE(int20):ABSOLUTE(_int_default))
	LONG(DEFINED(int21)?ABSOLUTE(int21):ABSOLUTE(_int_default))
	LONG(DEFINED(int22)?ABSOLUTE(int22):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int24)?ABSOLUTE(int24):ABSOLUTE(_int_default))
	LONG(DEFINED(int25)?ABSOLUTE(int25):ABSOLUTE(_int_default))
	LONG(DEFINED(int26)?ABSOLUTE(int26):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int28)?ABSOLUTE(int28):ABSOLUTE(_int_default))
	LONG(DEFINED(int29)?ABSOLUTE(int29):ABSOLUTE(_int_default))
	LONG(DEFINED(int2a)?ABSOLUTE(int2a):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int2c)?ABSOLUTE(int2c):ABSOLUTE(_int_default))
	LONG(DEFINED(int2d)?ABSOLUTE(int2d):ABSOLUTE(_int_default))
	LONG(DEFINED(int2e)?ABSOLUTE(int2e):ABSOLUTE(_int_default))
	LONG(DEFINED(int2f)?ABSOLUTE(int2f):ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(ABSOLUTE(_int_default))
	LONG(DEFINED(int34)?ABSOLUTE(int34):ABSOLUTE(_int_default))
	LONG(DEFINED(int35)?ABSOLUTE(int35):ABSOLUTE(_int_default))
	LONG(DEFINED(int36)?ABSOLUTE(int36):ABSOLUTE(_int_default))
	LONG(DEFINED(int37)?ABSOLUTE(int37):ABSOLUTE(_int_default))
	LONG(DEFINED(int38)?ABSOLUTE(int38):ABSOLUTE(_int_default))
	LONG(DEFINED(int39)?ABSOLUTE(int39):ABSOLUTE(_int_default))
	LONG(DEFINED(int3a)?ABSOLUTE(int3a):ABSOLUTE(_int_default))
	LONG(DEFINED(int3b)?ABSOLUTE(int3b):ABSOLUTE(_int_default))
	LONG(DEFINED(int3c)?ABSOLUTE(int3c):ABSOLUTE(_int_default))

	FILL(0xff)
        }  > vectors
.text  : {
	*(.text) 				
	*(.strings)
	*(.rodata) 				
   	 _etext = . ; 
	} > rom
.tors : {
	___ctors = . ;
	*(.ctors)
	___ctors_end = . ;
	___dtors = . ;
	*(.dtors)
	___dtors_end = . ;
	}  > rom
.data : AT ( ADDR(.tors) + SIZEOF(.tors) ){
	___data = . ;
	*(.data)
	*(.tiny)
	 _edata = .;
	} > ram
	data_size = SIZEOF(.data);
.bss : {
	 _bss_start = . ;
	*(.bss)
	*(COMMON)
	 _end = . ;  
	}  > ram
	bss_size = SIZEOF(.bss);
.stack : {
	 _stack = . ; 
	*(.stack)
	}  > stack
.stab 0 (NOLOAD) : {
	[ .stab ]
	}
.stabstr 0 (NOLOAD) : {
	[ .stabstr ]
	}
}
