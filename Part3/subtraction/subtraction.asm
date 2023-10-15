.data
minuend:    .word   32766       # minuend value
subtrahend: .word   10383       # subtrahend value
result:     .word   0           # Initialize result to 0
newline:    .asciiz "\n"        # Newline character

.text
.globl main

main:
    # Load minuend and subtrahend into registers
    lw  $t0, minuend
    lw  $t1, subtrahend

    # Call subtract function
    jal subtract

    # Display result
    move $a0, $t0                # Load result into $a0
    li   $v0, 1                  # Print integer service code
    syscall

    # Print newline
    li   $v0, 4                  # Print string service code
    la   $a0, newline
    syscall

    # Exit program
    li   $v0, 10                 # Exit service code
    syscall

subtract:
    # Function to subtract subtrahend from minuend
    # Arguments: $t0 = minuend, $t1 = subtrahend
    # Returns: $t0 = result

    # Initialize result to minuend
    move $t2, $t0

    loop:
        beqz $t1, done           # If subtrahend == 0, exit loop
        subi $t2, $t2, 1         # Subtract 1 from result
        subi $t1, $t1, 1         # Subtract 1 from subtrahend
        j    loop

    done:
        move $t0, $t2            # Store result in $t0
        jr   $ra                 # Return to caller