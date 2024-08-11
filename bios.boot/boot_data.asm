MSG_REAL_MODE db " Started in 16 - bit Real Mode " , 0
MSG_LOAD_KERNEL db " Loading kernel into memory. " , 0
DISK_ERROR_MSG db " Failed to load kernel", 0
LOAD_SUCC_MSG db " Kernel loaded succesfully", 0

BOOT_DRIVE db 0
; disk params
DISK_SPT db 0
DISK_LD_SECTOR dw KERNEL_N_SECTORS
DISK_CYLINDER  db KERNEL_SELECT_CYLINDER
DISK_HEAD db KERNEL_SELECT_HEAD
DISK_ST_SECTOR db KERNEL_START_SECTOR
DISK_N_SECTOR db 0