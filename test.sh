#!/bin/sh

# This script can be run anywhere if you set the variable below
#
# It runs all the tests with extension .og and creates a folder where all the logs will be stored
# This script has 5 steps:
# 1. Compile the .og file generating an .asm file
# 2. Generate the object file from the .asm file
# 3. Link to generate the executable file from the object file
# 4. Run the program and check if exited correctly
# 5. Check the output of the program with the expected output
#
# It has the following options:
# no-color: print output uncolored
# no-compact: print the output of each test to the console
# no-clean: keep all the logs
#
# Set the path to the folder with the tests
TESTS_PATH="../auto-tests"
# Set the path to the folder where the logs are going to be stored
LOG_FOLDER="log"
# Set the path to the compiler (og) already compiled
OG_PATH="../og"
#
# =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# DO NOT TOUCH AFTER THIS LINE
# =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

CLEAN=true
COMPACT=true
COLOR=true

if [ $# -ne 0 ]; then
    for i in $@
    do
        case ${i,,} in
            "no-clean")
                CLEAN=false
                ;;
            "no-compact")
                COMPACT=false
                ;;
            "no-color")
                COLOR=false
                ;;
            *)
                echo "Unknown option ${BOLD}$i${RESET}"
                ;;
        esac
    done
fi


cd $TESTS_PATH

if [ "$COLOR" = true ]; then
    RESET="\e[0m"
    BOLD="\e[1m"
    BLUE="\e[34m"
    ORANGE="\e[33m"
    RED="\e[31m"
    GREEN="\e[32m"
fi

if [ ! -d $LOG_FOLDER ]; then
    mkdir $LOG_FOLDER
fi

function clean_lines_up {
    for i in $(seq $1)
    do
        echo -e -n "\033[A\r\033[K"
    done
}

SUCCESS=0
TOTAL=$(ls *.og | wc -l)
CURRENT=0
for file in *.og
do
    CURRENT=$(($CURRENT + 1))
    util=${file%.og}
    echo "FILE: ${file} (${CURRENT}/${TOTAL})"
    echo
    echo -n "Generating asm... " | tee $LOG_FOLDER/${util}.log
    echo >> $LOG_FOLDER/${util}.log
    out=$($OG_PATH/og $file 2>&1 | tee -a $LOG_FOLDER/${util}.log)
    if [ -z "$out" ]; then
        echo "DONE"
    else
        echo "FAILED"
        if [ "$COMPACT" = true ]; then
            clean_lines_up 3
        fi
        continue
    fi

    echo >> $LOG_FOLDER/${util}.log
    echo -n "Producing object file... " | tee -a $LOG_FOLDER/${util}.log
    echo >> $LOG_FOLDER/${util}.log
    out=$(yasm -felf32 ${util}.asm 2>&1 | tee -a $LOG_FOLDER/${util}.log)
    if [ -z "$out" ]; then
        echo "DONE"
    else
        echo "FAILED"
        if [ "$COMPACT" = true ]; then
            clean_lines_up 4
        fi
        continue
    fi

    echo >> $LOG_FOLDER/${util}.log
    echo -n "Linking file... " | tee -a $LOG_FOLDER/${util}.log
    echo >> $LOG_FOLDER/${util}.log
    out=$(ld -L $HOME/compiladores/root/usr/lib/ -m elf_i386 -o ${util} ${util}.o -lrts 2>&1 | tee -a $LOG_FOLDER/${util}.log)
    if [ -z "$out" ]; then
        echo "DONE"
    else
        echo "FAILED"
        if [ "$COMPACT" = true ]; then
            clean_lines_up 5
        fi
        continue
    fi

    echo >> $LOG_FOLDER/${util}.log
    echo -n "Executing program... " | tee -a $LOG_FOLDER/${util}.log
    echo >> $LOG_FOLDER/${util}.log
    rm expected/${util}.mine &> /dev/null
    ./${util} 2>&1 | tee -a expected/${util}.mine $LOG_FOLDER/${util}.log > /dev/null

    if [ $? -eq 0 ]; then
        echo "DONE"
    else
        echo "FAILED"
        if [ "$COMPACT" = true ]; then
            clean_lines_up 6
        fi
        continue
    fi

    echo >> $LOG_FOLDER/${util}.log
    echo -n "Comparing outputs... " | tee -a $LOG_FOLDER/${util}.log
    echo >> $LOG_FOLDER/${util}.log

    if [ "$(diff -q -b expected/${util}.out expected/${util}.mine)" != "" ]; then
        diff -b expected/${util}.out expected/${util}.mine >> $LOG_FOLDER/${util}.log
        echo "FAILED"
        if [ "$COMPACT" = true ]; then
            clean_lines_up 7
        fi
        continue
    else
        echo "DONE"
    fi

    echo "Cleaning... "
    rm ${util}.o ${util}.asm expected/${util}.mine ${util}

    if [ "$CLEAN" = true ]; then
        rm $LOG_FOLDER/${util}.log
    fi

    SUCCESS=$(($SUCCESS + 1))
    if [ "$COMPACT" = true ]; then
        clean_lines_up 8
    fi
done
echo -e "${BOLD}${BLUE}FINAL RESULTS:${RESET}"
echo -e "${BOLD}Passed: ${GREEN}${SUCCESS}${RESET}"
echo -e "${BOLD}Failed: ${RED}$(($TOTAL - $SUCCESS))${RESET}"

perc=$(echo "scale=2; $SUCCESS * 100 / $TOTAL" | bc -l)

if [ $(echo "$perc > 25" | bc -l) -eq 0 ]; then
    color=$RED
elif [ $(echo "$perc > 50" | bc -l) -eq 0 ]; then
    color=$YELLOW
elif [ $(echo "$perc > 80" | bc -l) -eq 0 ]; then
    color=$ORANGE
else
    color=$GREEN
fi

echo -e "${BOLD}Percentage: ${color}${perc}% ($SUCCESS / $TOTAL)${RESET}"

if [ $SUCCESS -ne $TOTAL ]; then
    echo -e "${BOLD}${ORANGE}NOTE:${RESET} Check ${BOLD}${TESTS_PATH}/${LOG_FOLDER}${RESET} to see the full results"
    exit 1
fi
